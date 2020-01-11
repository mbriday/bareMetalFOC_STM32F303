#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"

void wait()
{
	volatile int i=0;
    for(i=0;i<2000000;i++);
}

void setup()
{
    pinMode(GPIOA,5,OUTPUT);
}

/* main function */
int main(void)
{
    int id = 10000;
    int state=0;
	setup();
	/* Infinite loop */
	while (1)
	{
        digitalWrite(GPIOA,5,state);
        state = 1-state;
		wait();
        Serial.printString((char*)("Hello world: "));
        Serial.printInt(id++,16);
        Serial.printchar('\n');
    }
}

