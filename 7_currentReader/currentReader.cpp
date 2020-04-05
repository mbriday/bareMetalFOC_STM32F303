#include "stm32f3xx.h"
#include "currentReader.h"
#include "pinAccess.h"

/* configuration:
 *  - injected channels => preempt regular conversions.
 *  - synchronous with AHB => no clock domain resynchronization
 *  - sequence scan
 *
 */

void currentReader::begin()
{
    ADC_TypeDef *adcTab[]     = {CURRENT_U_ADC  ,CURRENT_V_ADC  ,CURRENT_W_ADC };
    GPIO_TypeDef *portTab[]   = {CURRENT_U_PORT ,CURRENT_V_PORT ,CURRENT_W_PORT};
    const uint32_t chan[]     = {CURRENT_U_CHAN ,CURRENT_V_CHAN ,CURRENT_W_CHAN};
    const uint32_t pin[]      = {CURRENT_U_PIN  ,CURRENT_V_PIN  ,CURRENT_W_PIN };

    //1 - ADC input clock - cpu max freq (ADC1/2)
    RCC->CFGR2 &= ~(RCC_CFGR2_ADCPRE12_Msk | RCC_CFGR2_ADCPRE34_Msk);
    RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV1 | RCC_CFGR2_ADCPRE34_DIV1;
    RCC->AHBENR |= RCC_AHBENR_ADC12EN | RCC_AHBENR_ADC34EN;
    __asm("nop");
    //reset peripheral (mandatory!)
    RCC->AHBRSTR |=   RCC_AHBRSTR_ADC12RST | RCC_AHBRSTR_ADC34RST;
    RCC->AHBRSTR &= ~(RCC_AHBRSTR_ADC12RST | RCC_AHBRSTR_ADC34RST);
    __asm("nop");

    //synchronous clock Fadc = HCLK/1
    ADC12_COMMON->CCR &= ~ADC12_CCR_CKMODE;
    ADC12_COMMON->CCR |= ADC12_CCR_CKMODE_0;
    ADC34_COMMON->CCR &= ~ADC34_CCR_CKMODE;
    ADC34_COMMON->CCR |= ADC34_CCR_CKMODE_0;

    for(int i=0;i<NB_ADC;i++)
    {

        //** calibration procedure (p.318) **

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


        //** sampling time channel config ** TODO ## TEST ##
        if(chan[i] < 10)
        {
            adcTab[i]->SMPR1 |= ADC_SMPR << (chan[i]*3U);
        } else {
            adcTab[i]->SMPR2 |= ADC_SMPR << ((chan[i]-10)*3);
        }

        //** start ADC (p.320)
        //make sure ADRDY is 0 (writing a 1)
        adcTab[i]->ISR = ADC_ISR_ADRDY;
        //1- set ADEN
        adcTab[i]->CR |= ADC_CR_ADEN;
        //2- wait until ADRDY = 1
        while(!(adcTab[i]->ISR & ADC_ISR_ADRDY));

        //** configure pin:
        pinAnalog(portTab[i],pin[i]);

        //** select hardware trigger: TIM1->TRGO2
        adcTab[i]->JSQR  = chan[i] << ADC_JSQR_JSQ1_Pos   //channel 1st conv
                           //TIM1 trigger TRGO2 signal associated to JEXT8
                         | ADC_JSQR_JEXTSEL_3
                           //external trigger enable
                         | ADC_JSQR_JEXTEN_0;
        //** define interrupt (debug only?)
        //adcTab[i]->IER = ADC_IER_JEOCIE;

        //**start injected conversion on next hw trigger (TIM1)
        adcTab[i]->CR |= ADC_CR_JADSTART;
    }
    //NVIC_EnableIRQ(ADC1_IRQn);
    //NVIC_SetPriority(ADC1_IRQn,0);
    //NVIC_EnableIRQ(ADC3_IRQn);
    //NVIC_SetPriority(ADC3_IRQn,0);
}

void currentReader::waitADCResult()
{
    while(!(CURRENT_U_ADC->ISR & ADC_ISR_JEOS)){}
    CURRENT_U_ADC->ISR = ADC_ISR_JEOS;
    while(!(CURRENT_V_ADC->ISR & ADC_ISR_JEOS)){}
    CURRENT_V_ADC->ISR = ADC_ISR_JEOS;
    while(!(CURRENT_W_ADC->ISR & ADC_ISR_JEOS)){}
    CURRENT_W_ADC->ISR = ADC_ISR_JEOS;
}

int32_t currentReader::getCurrentU()
{
  const int32_t raw = CURRENT_U_ADC->JDR1;
  //(1<<30)/1899 = 565424,8678
  const int32_t result = (raw-1936)*565425;
  return result;
}

int32_t currentReader::getCurrentV()
{
  const int32_t raw = CURRENT_V_ADC->JDR1;
  //(1<<30)/1899 = 565424,8678
  const int32_t result = (raw-1936)*565425;
  return result;
}

int32_t currentReader::getCurrentW()
{
  const int32_t raw = CURRENT_W_ADC->JDR1;
  //(1<<30)/1899 = 565424,8678
  const int32_t result = (raw-1936)*565425;
  return result;
}

float currentReader::getCurrentFloat(uint16_t raw)
{
    //electronic interface gives:
    // gain       1.53
    // out offset 1.56V => 1936
    // => voltage on ADC: 1.53xi+1.56
    // i is in [-1.02;1.14] amps
    //    i = (ADC-1936)/1899
    return (raw-1936.)/1899;
}

currentReader CurrentReader;
