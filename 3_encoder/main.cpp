#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "encoder.h"

void wait()
{
    volatile int i=0;
    for(i=0;i<2000000;i++);
}

/* main function */
int main(void)
{
	/* Infinite loop */
	while (1)
	{
        Serial.printString("encoder: ");
        Serial.printInt(Encoder.getValue());
        Serial.printchar('\n');
        wait();
	}
}
