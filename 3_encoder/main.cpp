#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "encoder.h"

/* main function */
int main(void)
{
    int idx = 0;
    char c;
    pinMode(GPIOA,5,OUTPUT); /* user led */
    /* Infinite loop */
	while (1)
    {
        idx++;
        if(idx == 500000)
        {
            idx = 0;
            Serial.printString("encoder: ");
            Serial.printInt(Encoder.getValue());
            Serial.printchar('\n');
        }
        if(Serial.readchar(c) && c=='a')
        {
            digitalToggle(GPIOA,5);
        }
	}
}
