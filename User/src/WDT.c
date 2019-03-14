#include "WDT.h"

		static const uint16_t ReloadKey = 0xAAAA;
		static const uint16_t EnableKey = 0xCCCC;
		static const uint16_t AccessKey = 0x5555;

		static const uint16_t ClockFreq = 40000;
		
		 void WDTStart(unsigned periodMSec)
		{
			const unsigned MaxReload = (1 << 12) - 1;
			unsigned reload  = (periodMSec * ClockFreq) / 1000;
			unsigned prescaller = 0;
			unsigned divider = 4;
			while(reload / divider > MaxReload)
			{
				divider*=2;
				prescaller++;
			}
			reload /= divider;
			if(divider > 256)
			{
				prescaller = 7;
				reload = MaxReload;
			}
			IWDG->KR = AccessKey;
			IWDG->PR = prescaller;
			IWDG->RLR = reload;
			IWDG->KR = ReloadKey;
			IWDG->KR = EnableKey;
		}
		
		void WDTDisable(void)
		{
			
		}
		
		void WDTReset(void)
		{
			IWDG->KR = ReloadKey;
		}
		
		bool WDTResetBy(void)
		{
			return RCC->CSR & RCC_CSR_IWDGRSTF;
		}
