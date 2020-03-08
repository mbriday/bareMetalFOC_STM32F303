#include "stm32f3xx.h"
#include "adc.h"
#include "pinAccess.h"

/* configuration:
 *  - injected channels => preempt regular conversions.
 *  - synchronous with AHB => no clock domain resynchronization
 *  - sequence scan
 *
 */

adc::adc()
{
    ADC_TypeDef *adcTab[] = {ADC1,ADC2};

    //1 - ADC input clock - cpu max freq (ADC1/2)
    RCC->CFGR2 &= ~RCC_CFGR2_ADCPRE12_Msk;
    RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1;
    RCC->AHBENR |= RCC_AHBENR_ADC12EN;
    __asm("nop");
    //reset peripheral (mandatory!)
    RCC->AHBRSTR |=  RCC_AHBRSTR_ADC12RST;
    RCC->AHBRSTR &= ~RCC_AHBRSTR_ADC12RST;
    __asm("nop");

    //synchronous clock Fadc = HCLK/1
    ADC12_COMMON->CCR &= ~ADC12_CCR_CKMODE;
    ADC12_COMMON->CCR |= ADC12_CCR_CKMODE_0;

    //calibration procedure (p.318)
    for(int i=0;i<2;i++)
    {

        //1- ADVREGEN should be set to 01
        adcTab[i]->CR &= ~ADC_CR_ADVREGEN_Msk;
        adcTab[i]->CR |=  ADC_CR_ADVREGEN_0;
        //1b- wait for ADC stabilization time. 10us in worst case
        //a 1 000 000 loop requires 11 000 000 ticks with a 64MHz timer, with optim -O3
        //=> We need 640 ticks => ~ 60 loops. Let's say 100!
        for(volatile int j=0; j< 100;j++);

        //2- ADEN should be set to 0
        adcTab[i]->CR &= ~ADC_CR_ADEN;
        //3- ADCALDIF=0 (no differential input)
        adcTab[i]->CR &= ~ADC_CR_ADCALDIF;
        //4- ADCAL=1
        adcTab[i]->CR |= ADC_CR_ADCAL;
        //5- wait until ADCAL = 0
        while(adcTab[i]->CR & ADC_CR_ADCAL);
        //6- calibration is in ADC_CALFACT register.
    }

    /*******************************************/
    /* ** specific parts ** ADC in reset mode  */
    //timings: SMP=110 => 181.5 ADC clock cycles => 2.52us
    //total: sampling time + 12.5
    //Temperature => mini 2.2us on STM32F303RE
    ADC1->SMPR2  = 6 << ADC_SMPR2_SMP16_Pos;
    //timings: SMP=110 => 181.5 ADC clock cycles => 2.52us
    //VBat => mini 2.2us on STM32F303RE to get 1mV precision
    ADC1->SMPR2 |= 6 << ADC_SMPR2_SMP17_Pos;
    ADC12_COMMON->CCR |= ADC_CCR_TSEN | ADC_CCR_VBATEN;
    /*******************************************/

    for(int i=0;i<2;i++)
    {
        //start ADC (p.320)
        //make sure ADRDY is 0 (writing a 1)
        adcTab[i]->ISR = ADC_ISR_ADRDY;
        //1- set ADEN
        adcTab[i]->CR |= ADC_CR_ADEN;
        //2- wait until ADRDY = 1
        while(!(adcTab[i]->ISR & ADC_ISR_ADRDY));
    }

    // ************************************
    //specific for ADC channels
    // ************************************
    //set channels
    //ADC1->SQR1  = 16 << ADC_SQR1_SQ1_Pos; // T°   => ADC1_16
    //ADC2->SQR1  = 17 << ADC_SQR1_SQ1_Pos; // Vbat => ADC1_17
    ADC1->JSQR  = 16 << ADC_JSQR_JSQ1_Pos   //channel 1st conv
                | 17 << ADC_JSQR_JSQ2_Pos   //channel 2nd conv
                | ADC_JSQR_JL_0             //2 conversions
                //TIM1 trigger TRGO2 signal associated to JEXT8
                | ADC_JSQR_JEXTSEL_3
                //external trigger enable
                | ADC_JSQR_JEXTEN_0;
    //interrupt (JADSTART should NOT be set)
    ADC1->IER = ADC_IER_JEOCIE;
    NVIC_EnableIRQ(ADC1_IRQn);
    NVIC_SetPriority(ADC1_IRQn,0);
    //start injected conversion on next hw trigger (TIM1)
    ADC1->CR |= ADC_CR_JADSTART;
}

//only JEOC (debug)
extern "C" void ADC1_2_IRQHandler()
{
    volatile ADC_TypeDef * __attribute__((unused)) adc=ADC1;
    GPIOC->BSRR = 1 << 12; //set PC12
    ADC1->ISR = ADC_ISR_JEOC;
    for(volatile int i=0;i<5;i++);
    GPIOC->BSRR = 1 << (12+16); //reset PC12
}

void adc::waitADCResult()
{
    while(!(ADC1->ISR & ADC_ISR_JEOS)){}
    ADC1->ISR = ADC_ISR_JEOS;
}

adc Adc;
