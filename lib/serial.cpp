#include "stm32f3xx.h"
#include "serial.h"

serial Serial;

/* USART2 is available through the ST-Link MCU to support a virtual COM port:
 * - PA2 (TX) => alternative config 7
 * - PA3 (RX) => alternative config 7
 * configuration 115200 8N1
 */

serial::serial()
{
    m_txBufHead = 0;
    m_txBufTail = 0;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;              //clock for GPIOA
    __asm("nop");                                   //wait until GPIOA clock is Ok.
    GPIOA->OTYPER |= GPIO_OTYPER_OT_2;              //open drain
    GPIOA->OTYPER |= GPIO_OTYPER_OT_3;              //open drain
    GPIOA->AFR[0] |= 7 << GPIO_AFRL_AFRL2_Pos;      //alternate func is AF1
    GPIOA->AFR[0] |= 7 << GPIO_AFRL_AFRL3_Pos;      //alternate func is AF1
    GPIOA->MODER  &= ~(3 << GPIO_MODER_MODER2_Pos); //reset PA2 config
    GPIOA->MODER  &= ~(3 << GPIO_MODER_MODER3_Pos); //reset PA3 config
    GPIOA->MODER  |= 2 << GPIO_MODER_MODER2_Pos;    //alternate function
    GPIOA->MODER  |= 2 << GPIO_MODER_MODER3_Pos;    //alternate function

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
    __asm("nop");

    USART2->CR2 = 0; //default
    USART2->CR3 = 0; //default
    USART2->BRR = (640000/1152) >> 1; //115200 bauds
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    NVIC_EnableIRQ(USART2_IRQn);
}

extern "C" void USART2_IRQHandler()
{
    //interrupt updates m_txBufTail index!!

    //only "TX Empty interrupt" is enabled
    char c = Serial.m_txBuffer[Serial.m_txBufTail];
    Serial.m_txBufTail = (Serial.m_txBufTail+1)%SERIAL_TX_BUFFER_SIZE;
    if(Serial.m_txBufTail == Serial.m_txBufHead) { //empty
        USART2->CR1 &= ~USART_CR1_TXEIE; //remove the interrupt.
    }
    //send
    USART2->TDR = c;
}

void serial::printchar(char c)
{
    if(c == '\n') printchar('\r');   // newline should be preceeded by CR

    //routine updates m_txBufHead index
    const int next=(m_txBufHead+1) % SERIAL_TX_BUFFER_SIZE;
    while(next == m_txBufTail); //TX buffer full. Wait
    m_txBuffer[m_txBufHead] = c;
    m_txBufHead = next;
    USART2->CR1 |= USART_CR1_TXEIE; //TX empty interrupt
}

void serial::printString(const char* str)
{
    while(*str) printchar(*str++);
}

void serial::printInt(int32_t val, int base, int fieldWidth)
{
    if(base != 10 && base != 16)
    {
        printString("<base error>");
        return;
    }

    bool negative = false;
    char buffer[11]; /* min val: -2 000 000 000 => 11 chars */
    int index = 0;

    if (val < 0)
    {
      negative = true;
      val = -val;
    }
    if (val == 0) buffer[index++] = '0';
    else
    {
        while (val != 0)
        {
            char digit;
            int remaining = val % base;
            if(remaining<10) digit = (char)remaining + '0';
            else digit=(char)remaining-10+'A';
            buffer[index++] = digit;
            val = val / base;
        }
    }
    if (negative) buffer[index++] = '-';
    int i;
    for (i = index; i < fieldWidth; i++) printchar(' ');
    for (i = index - 1; i >= 0; i--)
    {
        printchar(buffer[i]);
    }
}
