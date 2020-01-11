#include "stm32f3xx.h"
#include "pinAccess.h"

void wait()
{
	volatile int i=0;
    for(i=0;i<200000;i++);
}

void setup()
{
    pinMode(GPIOA,5,OUTPUT);
}

/* main function */
int main(void)
{
	setup();
	/* Infinite loop */
	while (1)
	{
        digitalToggle(GPIOA,5);
		wait();
	}
}

