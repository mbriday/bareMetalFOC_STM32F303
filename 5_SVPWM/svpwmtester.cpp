#include <math.h>
#include "svpwmtester.h"
#include "pinAccess.h"
#include "stm32f3xx.h"

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

void svpwmTester::update(const unsigned int angleDeg)
{
    const float angleR = angleDeg*3.14159265/180;
    mValpha = mAmplitude * cos(angleR);
    mVbeta  = mAmplitude * sin(angleR);
}

unsigned int svpwmTester::getData()
{
    return (mValpha<<16) | mVbeta;
}

/*
 * ~97 us with soft FPU   (+-2us)
 * ~27.6 us with soft FPU (stable) => 3.5x faster
 */
extern "C" void TIM6_DAC_IRQHandler() {
    static int degree = 0;
    GPIOA->BSRR = 1 << 5;
    degree = (degree+1) % 360;
    SvpwmTester.update(degree);
    GPIOA->BSRR = 1 << (5+16);
    TIM6->SR &= ~TIM_SR_UIF;	//acknowledge
}

//only one instance (hardware timer associated)
svpwmTester SvpwmTester;
