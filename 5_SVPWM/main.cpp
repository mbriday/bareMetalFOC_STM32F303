#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "encoder.h"
#include "svpwm.h"
#include "svpwmtester.h"
#include "fixmath.h"

//configure TIM7 as a stopwatch (performances evaluation)
void configTIM7()
{
    //start TIM7
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM7RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
    __asm("nop");

    //configure TIM7
    TIM7->PSC = 0;           //max speed
    TIM7->ARR = 0xFFFF;      //Full range
    TIM7->CR1 = TIM_CR1_CEN; //enable, all other fields to 0
}

void setup()
{
    Serial.printString("Start\n");
    pinMode(GPIOA,5,OUTPUT); //user led
    configTIM7();
    Svpwm.begin();
    Svpwm.enableOutputs();
    SvpwmTester.begin();
    //debugFixMath();
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
