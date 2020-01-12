#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"

void setup()
{
    pinMode(GPIOA,5,OUTPUT);

    //start TIM6
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
    __asm("nop");

    //configure TIM6
    TIM6->PSC = 64000-1;     //tick@1ms
    TIM6->ARR = 200-1;       //200ms
    TIM6->CR1 = TIM_CR1_CEN; //enable, all other fields to 0

    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

extern "C" void TIM6_DAC_IRQHandler()
{
    static int state = 0;
    state = 1-state;
    digitalWrite(GPIOA,5,state);

    TIM6->SR &= ~TIM_SR_UIF;
}

/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
    while (1);
}
