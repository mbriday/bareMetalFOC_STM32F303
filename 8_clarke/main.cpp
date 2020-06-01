#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include <stdlib.h>

#define ARM_MATH_CM4
#include "arm_math.h"
//test the Clarke/Park transformations,
// using the ARM Cortex CMSIS DSP lib
// get computation time required

extern "C" uint32_t getCPUFreq(); //from startup_clock.
void setup()
{
    pinMode(GPIOA,5,OUTPUT); /* user led */

    //start TIM6 and TIM7
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN | RCC_APB1ENR_TIM7EN;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM7RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
    __asm("nop");

    //configure TIM7 as stopwatch:
    TIM7->PSC = 0; //max freq
    TIM7->CR1 = TIM_CR1_CEN;

    //configure TIM6 (@72MHz, cannot get a 1ms input freq!)
    TIM6->PSC = getCPUFreq()/10000-1;  //tick@100us
    TIM6->ARR = 10000-1;     //1000ms
    TIM6->CR1 = TIM_CR1_CEN; //enable, all other fields to 0

    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC_SetPriority(TIM6_DAC_IRQn,0); //max prio.
}

float evalClarke(const int alone, const int r)
{
    volatile uint32_t cnt;
    float result = 0;

    volatile float ia = 0.123456*r;
    volatile float ib = 0.234567*r;
    float ialpha = 0.0;
    float ibeta = 0.0;
    TIM7->CNT = 0;
    arm_clarke_f32(ia,ib,&ialpha,&ibeta);
    cnt = TIM7->CNT;
    result = ialpha*ibeta;
    Serial.printString("clarke f32    : ");
    Serial.printInt(cnt-alone);
    Serial.printString(" - ");

    volatile q31_t ia31 = 0x123456*r;
    volatile q31_t ib31 = 0x234567*r;
    q31_t ialphaQ31;
    q31_t ibetaQ31;
    TIM7->CNT = 0;
    arm_clarke_q31(ia31,ib31,&ialphaQ31,&ibetaQ31);
    cnt = TIM7->CNT;
    result += (float) ialphaQ31*ibetaQ31;
    Serial.printString("q31: ");
    Serial.printInt(cnt-alone);
    Serial.printchar('\n');
    return result;
}

float evalInvClarke(const int alone, const int r)
{
    volatile uint32_t cnt;
    float result = 0;

    volatile float ialpha = 0.123456*r;
    volatile float ibeta  = 0.234567*r;
    float ia = 0.0;
    float ib = 0.0;
    TIM7->CNT = 0;
    arm_inv_clarke_f32(ialpha,ibeta,&ia,&ib);
    cnt = TIM7->CNT;
    result = ia*ib;
    Serial.printString("inv clarke f32: ");
    Serial.printInt(cnt-alone);
    Serial.printString(" - ");

    volatile q31_t ialpha31 = 0x123456*r;
    volatile q31_t ibeta31 = 0x234567*r;
    q31_t iaQ31;
    q31_t ibQ31;
    TIM7->CNT = 0;
    arm_inv_clarke_q31(ialpha31,ibeta31,&iaQ31,&ibQ31);
    cnt = TIM7->CNT;
    result += (float) iaQ31*ibQ31;
    Serial.printString("q31: ");
    Serial.printInt(cnt-alone);
    Serial.printchar('\n');
    return result;
}

float evalPark(const int alone, const int r)
{
    volatile uint32_t cnt;
    float result = 0;

    volatile float ia = 0.123456*r;
    volatile float ib = 0.234567*r;
    float ialpha = 0.0;
    float ibeta = 0.0;
    float c = cos(1.567);
    float s = sin(1.567);
    TIM7->CNT = 0;
    arm_park_f32(ia,ib,&ialpha,&ibeta,s,c);
    cnt = TIM7->CNT;
    result = ialpha*ibeta;
    Serial.printString("park f32      : ");
    Serial.printInt(cnt-alone);
    Serial.printString(" - ");

    volatile q31_t ia31 = 0x123456*r;
    volatile q31_t ib31 = 0x234567*r;
    q31_t ialphaQ31;
    q31_t ibetaQ31;
    q31_t cQ31 = (q31_t)(c * (1<<31)) ;
    q31_t sQ31 = (q31_t)(s * (1<<31)) ;
    TIM7->CNT = 0;
    arm_park_q31(ia31,ib31,&ialphaQ31,&ibetaQ31,sQ31,cQ31);
    cnt = TIM7->CNT;
    result += (float) ialphaQ31*ibetaQ31;
    Serial.printString("q31: ");
    Serial.printInt(cnt-alone);
    Serial.printchar('\n');

    return result;
}

float evalInvPark(const int alone, const int r)
{
    volatile uint32_t cnt;
    float result = 0;

    volatile float ialpha = 0.123456*r;
    volatile float ibeta  = 0.234567*r;
    float ia = 0.0;
    float ib = 0.0;
    float c = cos(1.567);
    float s = sin(1.567);
    TIM7->CNT = 0;
    arm_inv_park_f32(ialpha,ibeta,&ia,&ib,s,c);
    cnt = TIM7->CNT;
    result = ia*ib;
    Serial.printString("inv park f32  : ");
    Serial.printInt(cnt-alone);
    Serial.printString(" - ");

    volatile q31_t ialpha31 = 0x123456*r;
    volatile q31_t ibeta31 = 0x234567*r;
    q31_t iaQ31;
    q31_t ibQ31;
    q31_t cQ31 = (q31_t)(c * (1<<31)) ;
    q31_t sQ31 = (q31_t)(s * (1<<31)) ;
    TIM7->CNT = 0;
    arm_inv_park_q31(ialpha31,ibeta31,&iaQ31,&ibQ31,sQ31,cQ31);
    cnt = TIM7->CNT;
    result += (float) iaQ31*ibQ31;
    Serial.printString("q31: ");
    Serial.printInt(cnt-alone);
    Serial.printchar('\n');
    return result;
}

/* called each 1s. */
extern "C" void TIM6_DAC_IRQHandler()
{
    /* ack */
    TIM6->SR &= ~TIM_SR_UIF;

    /* led */
    static int state = 0;
    state = 1-state;
    digitalWrite(GPIOA,5,state);

    volatile uint32_t cnt;
    /* to be sure that optimisation won't
     * inline function calls, we use a random
     * input value!
     */
    const int r = rand() % 1000;


    Serial.printchar('\n');
    Serial.printchar('\n');
    /* first measure the counter alone
     * This offset will be removed from measures.
     */
    TIM7->CNT = 0;
    cnt = TIM7->CNT;
    Serial.printString("alone: ");
    Serial.printInt(cnt);
    Serial.printchar('\n');

    /* get the result so that the compiler have to
     * generate the output values calculation code.
     */
    volatile float __attribute__((unused)) a;
    a  = evalClarke(cnt,r);
    a += evalInvClarke(cnt,r);
    a += evalPark(cnt,r);
    a += evalInvPark(cnt,r);
}

/* main function */
int main(void)
{

    Serial.printString("Start\n");
    setup(); //configure TIM6 to generate interrupt @1Hz
    /* Infinite loop */
    while (1);
}
