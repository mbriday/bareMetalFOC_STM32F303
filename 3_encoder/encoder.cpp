#include "encoder.h"
#include "stm32f3xx.h"

encoder::encoder()
{
    //use TIM2 encoder function
    // TIM2_CH1 is PA0 (pin 28 - connector ST-Morpho CN7) (or PA5/PA15)
    // TIM2_CH2 is PB3 (pin 31 - connector ST-Morpho CN10)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;  //clock for GPIOA,B
    __asm("nop");                       //wait until GPIOA,B clock are Ok.
    //TIM2_CH1 and TIM2_CH2 are alternate function AF1
    //p.40 of datasheet (physical part)
    GPIOA->OTYPER |= GPIO_OTYPER_OT_0;              //open drain
    GPIOB->OTYPER |= GPIO_OTYPER_OT_3;              //open drain
    GPIOA->AFR[0] |= 1 << GPIO_AFRL_AFRL0_Pos;	    //alternate func is AF1
    GPIOB->AFR[0] |= 1 << GPIO_AFRL_AFRL3_Pos;	    //alternate func is AF1
    GPIOA->MODER  &= ~(3 << GPIO_MODER_MODER0_Pos); //reset PA5 config
    GPIOB->MODER  &= ~(3 << GPIO_MODER_MODER3_Pos); //reset PB3 config
    GPIOA->MODER  |= 2 << GPIO_MODER_MODER0_Pos;    //alternate function
    GPIOB->MODER  |= 2 << GPIO_MODER_MODER3_Pos;    //alternate function

    //input clock = 64MHz. Required?
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    //reset peripheral (mandatory!)
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    __asm("nop");
    TIM2->CCMR1 |= 1 << TIM_CCMR1_CC1S_Pos; //Input Compare 1 on TI1
    TIM2->CCMR1 |= 1 << TIM_CCMR1_CC2S_Pos; //IC2 on TI2
    TIM2->CCER  |= TIM_CCER_CC1P;			//invert TIM2_CH1 polarity (sense)
    TIM2->SMCR  |= 3 << TIM_SMCR_SMS_Pos;	//encoder mode: SMS=011 (mode x4)
    TIM2->CR1   |= TIM_CR1_CEN;				//config reg : enable
}



int32_t encoder::getValue()
{
    return (int32_t)(TIM2->CNT);
}
