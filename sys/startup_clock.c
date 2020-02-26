#include "stm32f3xx.h"

/* system clock:
 * %%%% STM32F303x8 => 64MHz %%%% STM32F303xE => 72MHz %%%%
 * - use internal HSI RC oscillator => 8MHz
 * - PLL source is HSI => 8MHz
 * - configure PLLx9    => HCLK = 64 or 72MHz
 * - flash needs 2 wait states (F>48MHz)
 * - AHB prescaler not set (1)
 * - APB1 clocks sets to 32 or 36MHz (36 MHz max): but x2 for timers 2,3,6,7
 * - APB2 clocks sets to 64 or 72MHz.
 *
 * */
void SystemInit()
{
	// Must set flash latency (2 wait states): F>48MHz
    FLASH->ACR |= 2 << FLASH_ACR_LATENCY_Pos  // add two wait states
               |  FLASH_ACR_PRFTBE;           // enable prefetch buffer
	
    //clock on APB1 => max 36MHz
	//not divided for timers TIMx
	RCC->CFGR |= 4 << RCC_CFGR_PPRE1_Pos; 

	//make sure that HSI is the system clock
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;		// reset SW configuration (HSI).
	
	RCC->CR &= ~(RCC_CR_PLLON);			// turn off PLL
    while(RCC->CR & RCC_CR_PLLRDY){}	// wait until pll is stopped

    RCC->CFGR &= ~(RCC_CFGR_PLLSRC_Msk); // reset PLLSRC
    #if defined(STM32F303xE)
        // remove prediv PLL by 2 (for 303RE, not available for 303K8)
        RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_PREDIV;
        RCC->CFGR |= (RCC_CFGR_PLLMUL9);	// PLL multiply x9  => 72MHz
    #elif defined(STM32F303x8)
        RCC->CFGR |= (RCC_CFGR_PLLMUL16);	// PLL multiply x16 => 64MHz
    #else
        #error "clock configuration undefined"
    #endif
    RCC->CR |= RCC_CR_PLLON;			// turn on PLL
    while(!(RCC->CR & RCC_CR_PLLRDY)){}

	RCC->CFGR |= RCC_CFGR_SW_PLL;		// PLL is system clock
}

uint32_t getCPUFreq()
{
    #if defined(STM32F303xE)
        return 72000000;
    #elif defined(STM32F303x8)
        return 64000000;
    #else
        #error "clock configuration undefined"
    #endif
}
