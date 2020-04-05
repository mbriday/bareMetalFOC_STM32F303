#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "currentReader.h"

/** configure TIM1 with 1s interrupt
 *  associate trigger (TRGO) to the ADC
 */
void configTIM1()
{
    //start TIM1
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->APB2ENR; //force to read volatile register

    RCC->APB2RSTR |=  RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR; //force to read volatile register

    //configure TIM1
    TIM1->PSC = 7200;    //100us
    TIM1->ARR = 10000;   //1s
    TIM1->CR1 = TIM_CR1_CEN; //enable, all other fields to 0

    //TRGO2 associated to update event (for ADC)
    TIM1->CR2 &= ~TIM_CR2_MMS2_Msk;
    TIM1->CR2 |=  TIM_CR2_MMS2_1;

    TIM1->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
    NVIC_SetPriority(TIM1_UP_TIM16_IRQn,2);
}

extern "C" void TIM1_UP_TIM16_IRQHandler()
{
    uint16_t raw;
    GPIOC->BSRR = 1 << 10; //set PC10
    TIM1->SR &= ~TIM_SR_UIF;
    CurrentReader.waitADCResult();
    GPIOC->BSRR = 1 << 11; //set PC11

    asm("nop");

    Serial.printString("u: ");
    raw = CurrentReader.getRawU();
    Serial.printInt(raw);
    Serial.printString(" (");
    Serial.printInt(CurrentReader.getCurrentFloat(raw)*1000);
    Serial.printString(", ");
    Serial.printInt(((CurrentReader.getCurrentU()>>10)*1000)>>20);
    Serial.printString(") v: ");
    raw = CurrentReader.getRawV();
    Serial.printInt(raw);
    Serial.printString(" (");
    Serial.printInt(CurrentReader.getCurrentFloat(raw)*1000);
    Serial.printString(", ");
    Serial.printInt(((CurrentReader.getCurrentV()>>10)*1000)>>20);
    Serial.printString(") w: ");
    raw = CurrentReader.getRawW();
    Serial.printInt(raw);
    Serial.printString(" (");
    Serial.printInt(CurrentReader.getCurrentFloat(raw)*1000);
    Serial.printString(", ");
    Serial.printInt(((CurrentReader.getCurrentW()>>10)*1000)>>20);
    Serial.printString(")\n");

    GPIOC->BSRR = 1 << (10+16); //reset PC10
    GPIOC->BSRR = 1 << (11+16); //reset PC11
}

//debug: access to U,V and W as I/O
const int enable_pins[] = {13,14,15}; /* GPIOB */
const int in_pins[]     = { 8, 9,10}; /* GPIOA */

void setup()
{
    pinMode(GPIOC,10,OUTPUT);
    pinMode(GPIOC,11,OUTPUT);
    pinMode(GPIOC,12,OUTPUT);
    GPIOC->BSRR = 7 << 26; //reset PC10-12
    CurrentReader.begin(); //note: should be called before TIM1 config.
    configTIM1();
    pinMode(GPIOC,13,INPUT); //user button
    pinMode(GPIOA,5,OUTPUT); //user led

    for(int i = 0;i<3;i++)
    {
        pinMode(GPIOB,enable_pins[i],OUTPUT);
        digitalWrite(GPIOB,enable_pins[i],1);
        pinMode(GPIOA,in_pins[i],OUTPUT);
    }
    Serial.printString("Start\n");
}


/* main function */
int main(void)
{
    setup();
    int channel = 0;
    /* button state 0 released, 1 push, 2 pushed */
    int state = 0;
    /* Infinite loop */
	while (1)
    {
        if(digitalRead(GPIOC,13)==0)
        {
            if(state == 0) /* i.e. push */
            {
                channel = (channel +1) % 3;
                Serial.printString("chan ");
                Serial.printInt(channel);
                Serial.printchar('\n');
            }
            //pushed
            digitalWrite(GPIOA,5,1);
            digitalWrite(GPIOA,in_pins[channel],1);

            state = 2; /* now state is pushed */
        } else {
            state = 0;
            digitalWrite(GPIOA,5,0);
            digitalWrite(GPIOA,in_pins[channel],0);
        }
    }
}
