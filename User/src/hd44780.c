
#include "hd44780.h"

/* Private HD44780 structure */
typedef struct {
	uint8_t DisplayControl;
	uint8_t DisplayFunction;
	uint8_t DisplayMode;
	uint8_t Rows;
	uint8_t Cols;
	uint8_t currentX;
	uint8_t currentY;
} HD44780_Options_t;

/* Private functions */
static void TM_HD44780_InitPins(void);
static void TM_HD44780_Cmd(uint8_t cmd);
static void TM_HD44780_Cmd4bit(uint8_t cmd);
static void TM_HD44780_Data(uint8_t data);
static void TM_HD44780_CursorSet(uint8_t col, uint8_t row);

/* Private variable */
static HD44780_Options_t HD44780_Opts;

 const TPin LCD_D4 = {GPIOB,1};
 const TPin LCD_D5 = {GPIOB,0};
 const TPin LCD_D6 = {GPIOA,7};
 const TPin LCD_D7 = {GPIOA,6};
 
 const TPin LCD_E =  {GPIOB,2};
 const TPin LCD_RW = {GPIOB,10};
 const TPin LCD_RS = {GPIOB,11};
 
 const TPin* VDisplay[] = {&LCD_D4,&LCD_D5,&LCD_D6,&LCD_D7,&LCD_E,&LCD_RW,&LCD_RS};
 /* Команды */
typedef  enum 
   {
      HD44780_CLEARDISPLAY             = 0x01,
      HD44780_RETURNHOME               = 0x02,
      HD44780_ENTRYMODESET             = 0x04,
      HD44780_DISPLAYCONTROL           = 0x08,
      HD44780_CURSORSHIFT              = 0x10,
      HD44780_FUNCTIONSET              = 0x20,
      HD44780_SETCGRAMADDR             = 0x40,
      HD44780_SETDDRAMADDR             = 0x80
   }ComandsHD44780;
   
/* Флаги для режима ввода */
typedef enum 
   {
      HD44780_ENTRYRIGHT               = 0x00,
      HD44780_ENTRYLEFT                = 0x02,
      HD44780_ENTRYSHIFTINCREMENT      = 0x01,
      HD44780_ENTRYSHIFTDECREMENT      = 0x00
   }FlagsDisplayEntryHD44780;
   
 /* Флаги включения выключения дисплея */
typedef enum 
   {
      HD44780_DISPLAYON                = 0x04,
      HD44780_CURSORON                 = 0x02,
      HD44780_BLINKON                  = 0x01,
   }FlagsDisplayOnOffHD44780;   
 
   /* Флаги сдвига дисплея/курсора */
typedef enum 
   {
      HD44780_DISPLAYMOVE              = 0x08,
      HD44780_CURSORMOVE               = 0x00,
      HD44780_MOVERIGHT                = 0x04,
      HD44780_MOVELEFT                 = 0x00
   }FlagsShiftHD44780;

   /* Флаги настроек */   
typedef enum 
   {
      HD44780_8BITMODE                 = 0x10,
      HD44780_4BITMODE                 = 0x00,
      HD44780_2LINE                    = 0x08,
      HD44780_1LINE                    = 0x00,
      HD44780_5x10DOTS                 = 0x04,
      HD44780_5x8DOTS                  = 0x00,
   }FlagsSetHD44780;

//static void TM_HD44780_InitPins (void)
//{
//  ConfigVirualPort(VDisplay,7,OUT,_50MHz,NOPULL,PP);
//}

 static void HD44780_RS_LOW(void)
 {
 output_low(&LCD_RS);
 };
 
 static void HD44780_RS_HIGH(void)
 {
 output_hight(&LCD_RS);
 };
 
 static void HD44780_E_BLINK(void)
 {
   output_hight(&LCD_E);
   delay_us(50);
   output_low(&LCD_E);
   delay_us(50);
 };
   

   
void TM_HD44780_Init(uint8_t cols, uint8_t rows) {

	/* Init pinout */
	TM_HD44780_InitPins();
	
	/* At least 40ms */
	//HD44780_Delay(45000);
   delay_ms(10);
	
	/* Set LCD width and height */
	HD44780_Opts.Rows = rows;
	HD44780_Opts.Cols = cols;
	
	/* Set cursor pointer to beginning for LCD */
	HD44780_Opts.currentX = 0;
	HD44780_Opts.currentY = 0;
	
	HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
	if (rows > 1) {
		HD44780_Opts.DisplayFunction |= HD44780_2LINE;
	}
	
	/* Try to set 4bit mode */
	TM_HD44780_Cmd4bit(0x03);
	//HD44780_Delay(4500);
   delay_ms(10);
	
	/* Second try */
	TM_HD44780_Cmd4bit(0x03);
	//HD44780_Delay(4500);
   delay_ms(10);
	
	/* Third goo! */
	TM_HD44780_Cmd4bit(0x03);
	//HD44780_Delay(4500);	
   delay_ms(50);
	
	/* Set 4-bit interface */
	TM_HD44780_Cmd4bit(0x02);
	//HD44780_Delay(100);
   delay_ms(10);
	
	/* Set # lines, font size, etc. */
	TM_HD44780_Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

	/* Turn the display on with no cursor or blinking default */
	HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
	TM_HD44780_DisplayOn();

	/* Clear lcd */
	TM_HD44780_Clear();

	/* Default font directions */
	HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
	TM_HD44780_Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

	/* Delay */
	//HD44780_Delay(4500);
   delay_ms(10);
}

void TM_HD44780_Clear(void) {
	TM_HD44780_Cmd(HD44780_CLEARDISPLAY);
	//HD44780_Delay(3000);
   delay_ms(40);
}

void TM_HD44780_Puts(uint8_t x, uint8_t y, char* str) {
	TM_HD44780_CursorSet(x, y);
	while (*str) {
		if (HD44780_Opts.currentX >= HD44780_Opts.Cols) {
			HD44780_Opts.currentX = 0;
			HD44780_Opts.currentY++;
			TM_HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		}
		if (*str == '\n') {
			HD44780_Opts.currentY++;
			TM_HD44780_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		} else if (*str == '\r') {
			TM_HD44780_CursorSet(0, HD44780_Opts.currentY);
		} else {
			TM_HD44780_Data(*str);
			HD44780_Opts.currentX++;
		}
		str++;
	}
}

void TM_HD44780_DisplayOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_DisplayOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_BlinkOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_BLINKON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_BlinkOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_CursorOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_CURSORON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_CursorOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
	TM_HD44780_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void TM_HD44780_ScrollLeft(void) {
	TM_HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void TM_HD44780_ScrollRight(void) {
	TM_HD44780_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}

void TM_HD44780_CreateChar(uint8_t location, uint8_t *data) {
	uint8_t i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	TM_HD44780_Cmd(HD44780_SETCGRAMADDR | (location << 3));
	
	for (i = 0; i < 8; i++) {
		TM_HD44780_Data(data[i]);
	}
}

void TM_HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location) {
	TM_HD44780_CursorSet(x, y);
	TM_HD44780_Data(location);
}

/* Private functions */
static void TM_HD44780_Cmd(uint8_t cmd) {
	/* Command mode */
	HD44780_RS_LOW();
	
	/* High nibble */
	TM_HD44780_Cmd4bit(cmd >> 4);
	/* Low nibble */
	TM_HD44780_Cmd4bit(cmd & 0x0F);
}

static void TM_HD44780_Data(uint8_t data) {
	/* Data mode */
	HD44780_RS_HIGH();
	
	/* High nibble */
	TM_HD44780_Cmd4bit(data >> 4);
	/* Low nibble */
	TM_HD44780_Cmd4bit(data & 0x0F);
}

static void TM_HD44780_Cmd4bit(uint8_t cmd) {
	/* Set output port */

   SetPinValue(&LCD_D7,(cmd & 0x08));
   SetPinValue(&LCD_D6,(cmd & 0x04));
   SetPinValue(&LCD_D5,(cmd & 0x02));
   SetPinValue(&LCD_D4,(cmd & 0x01));
   
	HD44780_E_BLINK();
}

static void TM_HD44780_CursorSet(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	
	/* Go to beginning */
	if (row >= HD44780_Opts.Rows) {
		row = 0;
	}
	
	/* Set current column and row */
	HD44780_Opts.currentX = col;
	HD44780_Opts.currentY = row;
	
	/* Set location address */
	TM_HD44780_Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

static void TM_HD44780_InitPins(void)
{
   ConfigVirualPort(VDisplay,7,OUT,_50MHz,NOPULL,PP);
   WriteToVirtualPort(VDisplay,7,0x00);
}
