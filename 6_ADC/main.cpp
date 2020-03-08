#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "adc.h"

//configure TIM1 with 1s interrupt
void configTIM1()
{
    //start TIM1
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    __asm("nop");
    RCC->APB2RSTR |=  RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
    __asm("nop");

    //configure TIM1
    TIM1->PSC = 7200;    //100us
    TIM1->ARR = 10000;   //1s
    TIM1->CR1 = TIM_CR1_CEN; //enable, all other fields to 0

    //TRGO2 associated to update event (for ADC)
    TIM1->CR2 &= ~TIM_CR2_MMS2_Msk;
    TIM1->CR2 |=  TIM_CR2_MMS2_1;

    TIM1->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
    NVIC_SetPriority(TIM1_UP_TIM16_IRQn,2);
}

/* measures, in ticks (1/72MHz)
 * 542 ticks => full soft, 2 calls, each with 1 regular conversion.
 * 466 ticks => injected, sequence with 2 conversions.
 * 430 ticks => injected, sequence with 2 conversions => release mode
 */

extern "C" void TIM1_UP_TIM16_IRQHandler()
{
    GPIOC->BSRR = 1 << 10; //set PC10
    TIM1->SR &= ~TIM_SR_UIF;
    Adc.waitADCResult();
    GPIOC->BSRR = 1 << 11; //set PC11

    Serial.printString("v1: ");
    Serial.printInt(ADC1->JDR1);
    Serial.printString(" v2: ");
    Serial.printInt(ADC1->JDR2);
    Serial.printchar('\n');

    GPIOC->BSRR = 1 << (10+16); //reset PC10
    GPIOC->BSRR = 1 << (11+16); //reset PC11
}

void setup()
{
    pinMode(GPIOC,10,OUTPUT);
    pinMode(GPIOC,11,OUTPUT);
    pinMode(GPIOC,12,OUTPUT);
    GPIOC->BSRR = 7 << 26; //reset PC10-12
    configTIM1();
    Serial.printString("Start\n");
    pinMode(GPIOA,5,OUTPUT); //user led
}

/* main function */
int main(void)
{
    setup();
    /* Infinite loop */
	while (1)
    {
    }
}
