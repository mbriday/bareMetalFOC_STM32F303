#include "svpwm.h"
#include "pinAccess.h"
#include "fixmath.h"

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
    //changing ARR will have an impact on vector length! update it carefully!
    TIM1->ARR = 1024;  // 16 KHz (2*ARR, center aligned)

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
    TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;

    //set initial ratios
    TIM1->CCR1 = 800; // 80%
    TIM1->CCR2 = 600; // 60%
    TIM1->CCR3 = 400; // 40%

    disableOutputs();
    //start timer.
    TIM1->CR1 |= TIM_CR1_CEN;

    //TIM1->DIER |= TIM_DIER_UIE;
    //NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
    #ifdef DEBUG_SVPWM_SECTOR
        pinMode(GPIOA,5,OUTPUT);
    #endif
    m_timingMax = 0;
    m_timingMin = 0xFFFFFFFF;
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
    TIM1->BDTR &= ~TIM_BDTR_MOE; //main output disable
    digitalWrite(EN_PORT,ENU_PIN,0);
    digitalWrite(EN_PORT,ENV_PIN,0);
    digitalWrite(EN_PORT,ENW_PIN,0);
}

#ifdef DEBUG_SVPWM_SECTOR
void svpwm::update(uint32_t ValphaBeta,unsigned int degree)
#else
//__attribute__((section ( ".ccmram" ))) /* set this critical function in CCM Sram (no wait state) */
void svpwm::update(uint32_t ValphaBeta)
#endif
{
    #ifdef TIMINGS_SVPWM_UPDATE
        TIM7->CNT = 0;
    #endif

    const int16_t Valpha = (int16_t)(ValphaBeta >> 16);
    const int16_t Vbeta  = (int16_t)(ValphaBeta & 0xFFFF);
    const int16_t ValphaS3 = (int16_t)((Valpha * sqrt3) >> 14);
    int sector = 0;

    //ok, first search for the good sector:
    if(Vbeta >= 0)
    {
        if(Valpha >= 0)
        {
            if(Vbeta - ValphaS3 <= 0) sector = 1;
            else sector = 2;
        } else {
            if((Vbeta + ValphaS3) <= 0) sector = 3;
            else sector = 2;
        }
    } else {
        if(Valpha>= 0)
        {
            if(Vbeta + ValphaS3 <= 0) sector = 5;
            else sector = 6;
        } else {
            if((Vbeta - ValphaS3) <= 0) sector = 5;
            else sector = 4;
        }
    }

#ifdef DEBUG_SVPWM_SECTOR
    const unsigned int mod = degree % 1024;
    bool fail = false;
    switch(sector)
    {
         case 1: if(mod >  169             ) fail = true; break;
         case 2: if(mod <= 169 || mod > 339) fail = true; break;
         case 3: if(mod <= 339 || mod > 512) fail = true; break;
         case 4: if(mod <= 512 || mod > 681) fail = true; break;
         case 5: if(mod <= 681 || mod > 855) fail = true; break;
         case 6: if(mod <= 855             ) fail = true; break;
         default: fail = true;
    }
    if(fail)
    {
        GPIOA->BSRR = 1 << 5;
    }
#endif //DEBUG_SVPWM_SECTOR

    //Valpha and Vbeta are on [0,TIM1->ARR-1] => [0,1023]
    switch(sector)
    {
    case 1:{
         const uint16_t T2    = (Vbeta * twoDivSqrt3) >> 15;
         const uint16_t T1    = Valpha - (T2>>1);
         const uint16_t Toff2 = (1024-T1-T2) >> 1;

         TIM1->CCR3 = Toff2;			//C
         TIM1->CCR2 = TIM1->CCR3+T2;	//B
         TIM1->CCR1 = TIM1->CCR2+T1;	//A
        break;}
    case 2:{
         const uint16_t VbetaDivSqrt3  = (Vbeta * oneDivSqrt3) >> 16;
         const uint16_t T2 = VbetaDivSqrt3 + Valpha;
         const uint16_t T3 = VbetaDivSqrt3 - Valpha;
         const uint16_t Toff2 = (1024-T2-T3) >> 1;

         TIM1->CCR3 = Toff2;			//C
         TIM1->CCR1 = TIM1->CCR3+T2;	//A
         TIM1->CCR2 = TIM1->CCR1+T3;	//B
         break;}
    case 3:{
         const uint16_t VbetaDivSqrt3  = (Vbeta * oneDivSqrt3) >> 16;
         const uint16_t T3   = VbetaDivSqrt3 << 1; //x2
         const uint16_t T4 = - (VbetaDivSqrt3 + Valpha);
         const uint16_t Toff2 = (1024-T3-T4) >> 1;

         TIM1->CCR1 = Toff2;			//A
         TIM1->CCR3 = TIM1->CCR1+T4;	//C
         TIM1->CCR2 = TIM1->CCR3+T3;	//B
         break;}
    case 4:{
         const uint16_t VbetaDivSqrt3  = (Vbeta * oneDivSqrt3) >> 16;
         const uint16_t T5   = (-VbetaDivSqrt3) << 1; //x2
         const int16_t  T4 = -Valpha + VbetaDivSqrt3;
         const uint16_t Toff2 = (1024-T4-T5) >> 1;

         TIM1->CCR1 = Toff2;			//A
         TIM1->CCR2 = TIM1->CCR1+T4;	//B
         TIM1->CCR3 = TIM1->CCR2+T5;	//C
         break;}
    case 5:{
         const uint16_t MVbetaDivSqrt3  = (-Vbeta * oneDivSqrt3) >> 16;
         const int16_t  T5 = MVbetaDivSqrt3 - Valpha;
         const uint16_t T6 = MVbetaDivSqrt3 + Valpha;
         const uint16_t Toff2 = (1024-T5-T6) >> 1;

         TIM1->CCR2 = Toff2;			//B
         TIM1->CCR1 = TIM1->CCR2+T6;	//A
         TIM1->CCR3 = TIM1->CCR1+T5;	//C
         break;}
    case 6:{
         const uint16_t T6    = (-Vbeta * twoDivSqrt3) >> 15;
         const int16_t  T1    = Valpha - (T6>>1);
         const uint16_t Toff2 = (1024-T1-T6) >> 1;

         TIM1->CCR2 = Toff2;		    //B
         TIM1->CCR3 = TIM1->CCR2+T6;	//C
         TIM1->CCR1 = TIM1->CCR3+T1;	//A
         break;}
    default:
         break;
    }
    #ifdef TIMINGS_SVPWM_UPDATE
        uint32_t sw = TIM7->CNT;
        if(sw > m_timingMax) m_timingMax = sw;
        if(sw < m_timingMin) m_timingMin = sw;
    #endif
}

