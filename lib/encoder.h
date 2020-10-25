#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>

/* Choose the encoder interface:
 * either SPI or QUADARATURE
 */
#define ENCODER_SPI        1
#define ENCODER_QUADRATURE 2
//#define ENCODER_INTERFACE  ENCODER_SPI
#define ENCODER_INTERFACE  ENCODER_QUADRATURE

/* The encoder use a hall effect sensor (AS5147P from AMS)
 * The interface may be:
 *  * SPI        (10 MHz, mode 1)
 *  * QUADRATURE (2 signals A and B)
 * The quadrature mode uses the TIM2
 * quadrature decoder capability
 * to handle the position encoder
 *
 * The resolution is 4096 steps/round
 */
class encoder
{

public:
    /* configure TIM2 */
    encoder();
    /* getter */
    int32_t getValue();
    /* setter */
    void setValue(int32_t val);

};

extern encoder Encoder;

#endif // ENCODER_H
