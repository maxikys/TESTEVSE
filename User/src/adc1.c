#include "adc1.h"


#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VREFINT_CAL  =  ((uint16_t *)((uint32_t) 0x1FFFF7BA))

#define VDD_CALIB ((uint32_t) (3300))
#define VDD_APPLI ((uint32_t) (3300))
#define AVG_SLOPE ((uint32_t) (5336)) //AVG_SLOPE in ADC conversion step (@3.3V)/°C multiplied by 1000 for precision on the division

static uint32_t lastVddtime = 0; // Tms value of last Vdd measurement
static uint32_t VddValue = 0; // value of Vdd * 100 (for more precision measurements)
// check time of last Vdd measurement & refresh it value
#define CHKVDDTIME() do{if(!VddValue || Tms - lastVddtime > 999) getVdd();}while(0)

/**
 * Array for ADC raw values (before median filter by 9 elements):
 * 0 - Steppers current
 * 1 - Input voltage 12V
 * 2 - EndSwitch2 of motor1
 * 3 - EndSwitch1 of motor1
 * 4 - inner temperature
 * 5 - vref
 */
uint16_t ADC_array[NUMBER_OF_ADC_CHANNELS];

/**
 * @brief getADCval - calculate median value for `nch` channel
 * @param nch - number of channel
 * @return
 */
uint16_t getADCval(int nch){
    int i, addr = nch;
    register uint16_t temp;
#define PIX_SORT(a,b) { if ((a)>(b)) PIX_SWAP((a),(b)); }
#define PIX_SWAP(a,b) { temp=(a);(a)=(b);(b)=temp; }
    uint16_t p[9];
    for(i = 0; i < 9; ++i, addr += NUMBER_OF_ADC_CHANNELS) // first we should prepare array for optmed
        p[i] = ADC_array[addr];
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[1]) ; PIX_SORT(p[3], p[4]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[3]) ; PIX_SORT(p[5], p[8]) ; PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[3], p[6]) ; PIX_SORT(p[1], p[4]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[4], p[7]) ; PIX_SORT(p[4], p[2]) ; PIX_SORT(p[6], p[4]) ;
    PIX_SORT(p[4], p[2]) ;
    return p[4];
#undef PIX_SORT
#undef PIX_SWAP
}

void adc_setup(){
    // AIN: PA0..3, PA13, PA14. ADC_IN16 - inner temperature. ADC_IN17 - VREFINT
    /* (1) Enable the peripheral clock of the ADC */
    /* (2) Start HSI14 RC oscillator */
    /* (3) Wait HSI14 is ready */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* (1) */
    //RCC->CFGR |= RCC_CFGR_PPRE_2;
    RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0){}; /* (3) */
    /* (1) Ensure that ADEN = 0 */
    /* (2) Clear ADEN */
    /* (3) Launch the calibration by setting ADCAL */
    /* (4) Wait until ADCAL=0 */
    if ((ADC1->CR & ADC_CR_ADEN) != 0){ /* (1) */
        ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);  /* (2) */
    }
    ADC1->CR |= ADC_CR_ADCAL; /* (3) */
    while ((ADC1->CR & ADC_CR_ADCAL) != 0){} /* (4) */
    /* (1) Enable the ADC */
    /* (2) Wait until ADC ready */
    do{
        ADC1->CR |= ADC_CR_ADEN; /* (1) */
    }while ((ADC1->ISR & ADC_ISR_ADRDY) == 0); /* (2) */;
    /* (1) Select HSI14 by writing 00 in CKMODE (reset value) */
    /* (2) Select the continuous mode */
    /* (3) Select CHSEL0..3, 16,17 */
    /* (4) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than 17.1us */
    /* (5) Wake-up the VREFINT and Temperature sensor (only for VBAT, Temp sensor and VRefInt) */
    // ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; /* (1) */
    ADC1->CFGR1 |= ADC_CFGR1_CONT; /* (2)*/
    ADC1->CHSELR = ADC_CHSELR_CHSEL0 | ADC_CHSELR_CHSEL1 | ADC_CHSELR_CHSEL4 |
            ADC_CHSELR_CHSEL8 ; /* (3)*/
    ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; /* (4) */
    ADC->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN; /* (5) */
    // DMA for AIN
    /* (1) Enable the peripheral clock on DMA */
    /* (2) Enable DMA transfer on ADC and circular mode */
    /* (3) Configure the peripheral data register address */
    /* (4) Configure the memory address */
    /* (5) Configure the number of DMA tranfer to be performs on DMA channel 1 */
    /* (6) Configure increment, size, interrupts and circular mode */
    /* (7) Enable DMA Channel 1 */
    RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
    ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG; /* (2) */
    DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR)); /* (3) */
    DMA1_Channel1->CMAR = (uint32_t)(ADC_array); /* (4) */
    DMA1_Channel1->CNDTR = NUMBER_OF_ADC_CHANNELS; /* (5) */
    DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_CIRC; /* (6) */
    DMA1_Channel1->CCR |= DMA_CCR_EN; /* (7) */
    ADC1->CR |= ADC_CR_ADSTART; /* start the ADC conversions */
}

// return MCU temperature (degrees of celsius)
int32_t getTemp(){
    int32_t temperature; /* will contain the temperature in degrees Celsius */
    temperature = ((uint32_t) *TEMP30_CAL_ADDR - ((uint32_t) getADCval(4) * VDD_APPLI / VDD_CALIB)) * 1000;
    temperature = (temperature / AVG_SLOPE) + 30;
    return(temperature);
}

// return Vdd * 100 (V)
uint32_t getVdd(){

    return getADCval(4)*3300/(( 1 << 12 ) - 1);
}

//// return value of 12V * 100 (V)
//uint32_t getVmot(){
//    CHKVDDTIME();
//    uint32_t vmot = getADCval(1) * VddValue * the_conf.v12numerator;
//    vmot >>= 12;
//    vmot /= the_conf.v12denominator;
//    return vmot;
//}

//// return value of motors' current * 100 (A)
//uint32_t getImot(){
//    CHKVDDTIME();
//    uint32_t vmot = getADCval(0) * VddValue * the_conf.i12numerator;
//    vmot >>= 12;
//    vmot /= the_conf.i12denominator;
//    return vmot;
//}

//// end-switches status: 0 - don't activated, 1 - activated, 2 - user button, 4 - error
//// @param motnum - motor number  (0,1)
//// @param eswnum - switch number (0,1)
//ESW_status eswStatus(int motnum, int eswnum){
//    int idx;
//    if(motnum){ // motor 1 have no ADC - just 0 or 1
//        if(eswnum){ // ESW11 - PA14
//            if(GPIOA->IDR & 1<<14) return ESW_RELEASED;
//            else return ESW_HALL;
//        }else{ // ESW10 - PA13
//            if(GPIOA->IDR & 1<<13) return ESW_RELEASED;
//            else return ESW_HALL;
//        }
//    }else{ // motor 0
//        if(eswnum) idx = 2;
//        else idx = 3;
//    }
//    uint16_t thres = the_conf.ESW_thres, val = getADCval(idx);
//    // low sighal: 0..threshold - Hall activated
//    if(val < thres) return ESW_HALL;
//    // high signal: (4096-thres)..4096 - pullup
//    if(val > (uint16_t)0x1000 - thres) return ESW_RELEASED;
//    // middle signal: 0x800-thres..0x800+thres - user button active (47k pullup + 47k pulldown)
//    if(0x800 - thres < val && val < 0x800 + thres) return ESW_BUTTON;
//    // very strange, return err
//    return ESW_ERROR;
//}