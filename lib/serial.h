#ifndef SERIAL_H
#define SERIAL_H
#include <string>
#include <stdint.h>

#define SERIAL_TX_BUFFER_SIZE 512
#define SERIAL_RX_BUFFER_SIZE 128

class serial
{
public:

    /* TX FIFO buffer
     * private data (updated in interrupt handler, outside class members)
     */
    volatile char m_txBuffer[SERIAL_TX_BUFFER_SIZE];

    /* TX head of TX FIFO index (write index).
     * points to the first empty location.
     * private data (updated in interrupt handler, outside class members)
     */
    volatile int m_txBufHead;

    /* TX tail of TX FIFO index (read index)
     * points to the next byte to send.
     * private data (updated in interrupt handler, outside class members)
     */
    volatile int m_txBufTail;

    /* RX FIFO buffer
     * private data (updated in interrupt handler, outside class members)
     */
    volatile char m_rxBuffer[SERIAL_RX_BUFFER_SIZE];

    /* RX head of RX FIFO index (write index)
     * private data (updated in interrupt handler, outside class members)
     */
    volatile int m_rxBufHead;

    /* RX tail of RX FIFO index (read index)
     * private data (updated in interrupt handler, outside class members)
     */
    volatile int m_rxBufTail;
    serial();

    // print a single ASCII char on the serial line
    void printchar(char c);

    // print a NULL terminated ASCII string on the serial line
    void printString(const char* str);

    /* print a 32 bit signed integer on the serial line
     * @param base       defines the basis (decimal by default)
     * @param fieldwidth defines the minimal string length (spaces added)
     */
    void printInt(int32_t val, int base=10, int fieldWidth=1);

    /* waiting loop to re-synchronise output on Serial.
     * mainly for debug purpose
     */
    void waitForTXComplete();

    /* print a single ASCII char on the serial line
     * return false if buffer is empty
     */
    bool readchar(char &c);
};

extern serial Serial; //serial USART2 over USB

#endif // SERIAL_H
