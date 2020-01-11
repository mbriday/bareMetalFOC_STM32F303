#include "stm32f3xx.h"

/* system clock 
 * - use internal HSI RC oscillator => 8MHz
 * - PLL source is HSI/2 => 4MHz
 * - configure PLLx16    => HCLK = 64MHz
 * - flash needs 2 wait states (F>48MHz)
 * - AHB prescaler not set (1)
 * - APB1 clocks sets to 32MHz (36 MHz max): but x2 for timers 2,3,6,7
 * - APB2 clocks sets to 64MHz.
 *
 * */
void SystemInit()
{
	// Must set flash latency (2 wait states): F>48MHz
	FLASH->ACR |= 2 << FLASH_ACR_LATENCY_Pos ; // add two wait states
	
	//clock on APB1/2 => max 36MHz
	//not divided for timers TIMx
	RCC->CFGR |= 4 << RCC_CFGR_PPRE1_Pos; 

	//make sure that HSI is the system clock
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;		// reset SW configuration (HSI).
	
	RCC->CR &= ~(RCC_CR_PLLON);			// turn off PLL
	while(RCC->CR & RCC_CR_PLLRDY);		// wait until pll is stopped

	RCC->CFGR |= (RCC_CFGR_PLLMUL16);	// PLL multiply
	RCC->CR |= RCC_CR_PLLON;			// turn on PLL
	while(!(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR |= RCC_CFGR_SW_PLL;		// PLL is system clock
}
