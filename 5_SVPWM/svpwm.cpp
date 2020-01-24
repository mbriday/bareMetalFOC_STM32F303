#include "svpwm.h"
#include "pinAccess.h"

svpwm Svpwm;

svpwm::svpwm() {}


void svpwm::begin()
{
    pinMode(GPIOA,6,OUTPUT); //IO for interrupt

    pinMode(EN_PORT,ENU_PIN,OUTPUT);
    pinMode(EN_PORT,ENV_PIN,OUTPUT);
    pinMode(EN_PORT,ENW_PIN,OUTPUT);

    //Ok, configure the pwm:
    //These are alternate functions
    pinAlt(IN_PORT,INU_PIN,AFR_INU);
    pinAlt(IN_PORT,INV_PIN,AFR_INV);
    pinAlt(IN_PORT,INW_PIN,AFR_INW);

    //we deal with timer TIM1 only
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    __asm("nop");
    RCC->APB2RSTR |=  RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
    __asm("nop");

    TIM1->PSC = 2-1;   // 32 MHz
    TIM1->ARR = 1000;  // 16 KHz (2*ARR, center aligned)

    //PWM config
    //center aligned mode 1. All others to default
    TIM1->CR1 = 1 << TIM_CR1_CMS_Pos;
    //PWM mode 1: active if < comparison, else inactive, in up
    //            CCRx increases => duty cycle increases
    TIM1->CCMR1 = 6 << TIM_CCMR1_OC1M_Pos |
                  6 << TIM_CCMR1_OC2M_Pos |
                  TIM_CCMR1_OC1PE         |
                  TIM_CCMR1_OC2PE;
    TIM1->CCMR2 = 6 << TIM_CCMR2_OC3M_Pos |
                  TIM_CCMR2_OC3PE;
    TIM1->BDTR |= TIM_BDTR_MOE; //main output enable
    TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;

    //set initial ratios
    TIM1->CCR1 = 800; // 80%
    TIM1->CCR2 = 600; // 60%
    TIM1->CCR3 = 400; // 40%

    //start timer.
    TIM1->CR1 |= TIM_CR1_CEN;

    TIM1->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}

extern "C" void TIM1_UP_TIM16_IRQHandler()
{
    GPIOA->BSRR = 1 << 6;      // set IO

    //debugging.
    volatile TIM_TypeDef __attribute__((unused)) *tim=TIM1;

    TIM1->SR &= ~TIM_SR_UIF;
    GPIOA->BSRR = 1 << (6+16); // reset IO
}


void svpwm::enableOutputs()
{
    digitalWrite(EN_PORT,ENU_PIN,1);
    digitalWrite(EN_PORT,ENV_PIN,1);
    digitalWrite(EN_PORT,ENW_PIN,1);
    TIM1->BDTR |= TIM_BDTR_MOE; //main output enable
}

void svpwm::disableOutputs()
{
    digitalWrite(EN_PORT,ENU_PIN,0);
    digitalWrite(EN_PORT,ENV_PIN,0);
    digitalWrite(EN_PORT,ENW_PIN,0);
    TIM1->BDTR &= ~TIM_BDTR_MOE; //main output disable
}
