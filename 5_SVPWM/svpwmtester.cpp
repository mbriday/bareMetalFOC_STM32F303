#include "svpwmtester.h"
#include "pinAccess.h"
#include "stm32f3xx.h"
#include "fixmath.h"

svpwmTester::svpwmTester()
{

}

/*
 * The initial goal is ~600 rpm, each degree.
 * => 600/60 = 10 round/s
 * => 10*360 => 3600 Hz
 * If we get PSC= 64-1  => 1us
 *           ARR= 278-1 => 3597Hz
**/
void svpwmTester::begin()
{
    mAmplitude = 100; //10%

    pinMode(GPIOA,5,OUTPUT); //user led
    //start TIM6@3597 HZ
    //input clock = 64MHz.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    //reset peripheral (mandatory!)
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
    asm("nop");

    TIM6->PSC = 64-1;			    //prescaler  : tick@1us
    TIM6->ARR = 278-1;				//auto-reload: counts 100 ticks
    TIM6->DIER |= TIM_DIER_UIE;		//interrupt enable
    TIM6->CR1  |= TIM_CR1_CEN;		//config reg : enable

    //enable interrupt
    NVIC_SetPriority(TIM6_DAC_IRQn, 3);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void svpwmTester::setAmplitude(const unsigned int val)
{
    // constraint between 0 and 1000
    if(val > 1000) mAmplitude = 1000;
    else mAmplitude = val;
}

void svpwmTester::update(const unsigned int angle)
{
#ifdef __USE_PRECALCULATED_COS_TAB__
    const uint32_t sc = sincos(angle);
    const int16_t sinA = (int16_t)(sc >> 16);
    const int16_t cosA = (int16_t)(sc & 0xFFFF);
    mValpha = (mAmplitude * (cosA & 0xFFFF)) >> 15; /* as cos is in fix point 1.5 */
    mVbeta  = (mAmplitude * (sinA >> 16   )) >> 15; /* as sin is in fix point 1.5 */
#else
    mValpha = (int16_t)((float)mAmplitude * cos((float)angle*3.14159f/512));
    mVbeta  = (int16_t)((float)mAmplitude * sin((float)angle*3.14159f/512));
#endif
}

uint32_t svpwmTester::getData()
{
    return ((uint32_t)mValpha)<<16U | ((uint32_t)mVbeta & 0xFFFF);
}

/*
 * Test in debug mode:
 *  - ~110  us with soft float
 *  -  11.5 us with hard float ( x9.5 faster)
 *  -   3.8 us with fix point  (x28.9 faster)
 *  -   3.1 us with fix point and sincos (x35.7 faster)
 *
 * With Release mode (O3):
 *  - ~80.4 us with soft float
 *  -   5.1 us with hard float (x15.7 faster)
 *  -   1.5 us with fix point  (x53.6 faster)
 *  -   0.9 us with fix point and sincos (x89.3 faster)
 */
extern "C" void TIM6_DAC_IRQHandler() {
    static unsigned int degree = 0;
    GPIOA->BSRR = 1 << 5;
    degree = (degree+1) % 0x3FF; // modulo 1024
    SvpwmTester.update(degree);
    GPIOA->BSRR = 1 << (5+16);
    TIM6->SR &= ~TIM_SR_UIF;	//acknowledge
}

//only one instance (hardware timer associated)
svpwmTester SvpwmTester;
