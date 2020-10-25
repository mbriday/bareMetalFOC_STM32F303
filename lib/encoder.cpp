#include "stm32f3xx.h"
#include "encoder.h"
#include "pinAccess.h"

encoder Encoder;

encoder::encoder()
{
#if ENCODER_INTERFACE == ENCODER_QUADRATURE
    //use TIM2 encoder function
    // TIM2_CH1 is PA15 (pin 17 - connector ST-Morpho CN7) (or PA0/PA5)
    // TIM2_CH2 is PB3  (pin 31 - connector ST-Morpho CN10)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;  //clock for GPIOA,B
    __asm("nop");                       //wait until GPIOA,B clock are Ok.

    //TIM2_CH1 and TIM2_CH2 are alternate function AF1
    //p.40 of datasheet (physical part)
    pinAlt(GPIOA,15,1);
    pinAlt(GPIOB,3,1);

    //input clock = 72MHz. Required?
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    __asm("nop");
    //reset peripheral (mandatory!)
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
    __asm("nop");

    TIM2->CCMR1 |= 1U << TIM_CCMR1_CC1S_Pos; //Input Compare 1 on TI1
    TIM2->CCMR1 |= 1U << TIM_CCMR1_CC2S_Pos; //IC2 on TI2
    TIM2->CCER  |= TIM_CCER_CC1P;            //invert TIM2_CH1 polarity (sense)
    TIM2->SMCR  |= 3U << TIM_SMCR_SMS_Pos;   //encoder mode: SMS=011 (mode x4)
    TIM2->CR1   |= TIM_CR1_CEN;              //config reg : enable
#elif ENCODER_INTERFACE == ENCODER_SPI
    //1 - input clock = 72MHz.
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    __asm("nop");
    //reset peripheral (mandatory!)
    RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
//    pinAlt()
    #error "ENCODER_INTERACE SPI not done yet"

#else
#error "ENCODER_INTERACE is not set"
#endif
}

int32_t encoder::getValue()
{
    return (int32_t)(TIM2->CNT);
}

void encoder::setValue(int32_t val)
{
    TIM2->CNT = (uint32_t)val;
}
