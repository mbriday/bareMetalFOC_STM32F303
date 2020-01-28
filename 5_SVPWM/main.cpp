#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "encoder.h"
#include "svpwm.h"
#include "svpwmtester.h"
#include "fixmath.h"

void setup()
{
    pinMode(GPIOA,5,OUTPUT); //user led
    //Svpwm.begin();
    SvpwmTester.begin();
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
