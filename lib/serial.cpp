#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"

extern "C" uint32_t getCPUFreq(); //get cpu clock.
serial Serial;

extern "C" void __io_putchar(char c)
{
    Serial.printchar(c);
}
/* USART2 is available through the ST-Link MCU to support a virtual COM port:
 * - PA2 (TX) => alternative config 7
 * - PA3 (RX) => alternative config 7
 * configuration 115200 8N1
 */


serial::serial()
{
    m_txBufHead = 0;
    m_txBufTail = 0;

    m_rxBufHead = 0;
    m_rxBufTail = 0;

    pinAlt(GPIOA,2,7); // PA2 (TX) => alternative config 7
    pinAlt(GPIOA,3,7); // PA3 (RX) => alternative config 7

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
    __asm("nop");
    //select SYSCLK (CPU freq) for USART2: 64 or 72MHz (see startup_clock.c)
    RCC->CFGR3 &= ~RCC_CFGR3_USART2SW_Msk;
    RCC->CFGR3 |=  RCC_CFGR3_USART2SW_0;
    USART2->BRR = getCPUFreq()/115200; //115200 bauds

    USART2->CR2 = 0; //default
    USART2->CR3 = 0; //default
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE;
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn,1);
}

extern "C" void USART2_IRQHandler()
{
    if((USART2->ISR & USART_ISR_TXE) && (USART2->CR1 & USART_CR1_TXEIE))
    {
        char c = Serial.m_txBuffer[Serial.m_txBufTail];
        Serial.m_txBufTail = (Serial.m_txBufTail+1)%SERIAL_TX_BUFFER_SIZE;
        if(Serial.m_txBufTail == Serial.m_txBufHead) { //empty
            USART2->CR1 &= ~USART_CR1_TXEIE; //remove the interrupt.
        }
        //send (and ack)
        USART2->TDR = c;
    }
    if((USART2->ISR & USART_ISR_RXNE) && (USART2->CR1 & USART_CR1_RXNEIE))
    {
        const int next=(Serial.m_rxBufHead+1) % SERIAL_RX_BUFFER_SIZE;
        if(next == Serial.m_rxBufTail) {
            //buffer full => remove oldest data
            Serial.m_rxBufTail = (Serial.m_rxBufTail+1) % SERIAL_RX_BUFFER_SIZE;
        }
        char c = USART2->RDR;
        Serial.m_rxBuffer[Serial.m_rxBufHead] = c;
        Serial.m_rxBufHead = next;
    }
}

void serial::printchar(char c)
{
    volatile USART_TypeDef * __attribute__((unused)) usart=USART2;
    if(c == '\n') printchar('\r');   // newline should be preceeded by CR

    //routine updates m_txBufHead index
    USART2->CR1 &= ~USART_CR1_TXEIE; //remove interrupt
    const int next=(m_txBufHead+1) % SERIAL_TX_BUFFER_SIZE;
    if(next == m_txBufTail) //TX buffer full.
    {
        //remove oldest data
        m_txBufTail = (m_txBufTail+1) & SERIAL_TX_BUFFER_SIZE;
    }
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
            if(remaining<10) digit = (char)(remaining + (int)'0');
            else digit=(char)(remaining-10+(int)'A');
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

void serial::waitForTXComplete()
{
    /* The TX Interrupt is disabled when the TX buffer
     * is empty.
     */
    while(USART2->CR1 & USART_CR1_TXEIE);
}

bool serial::readchar(char &c){
    bool result = false;
    volatile USART_TypeDef * __attribute__((unused)) usart=USART2;
    if(m_rxBufTail != m_rxBufHead) { // not empty
        USART2->CR1 &= ~USART_CR1_RXNEIE; //remove rx interrupt
        c = m_rxBuffer[m_rxBufTail];
        m_rxBufTail = (m_rxBufTail+1)%SERIAL_TX_BUFFER_SIZE;
        result = true;
        USART2->CR1 |= USART_CR1_RXNEIE; //set rx interrupt again
    }
    return result;
}
