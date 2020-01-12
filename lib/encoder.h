#ifndef ENCODER_H
#define ENCODER_H
#include <stdint.h>

/* Use the TIM2 quadrature decoder capability
 * to handle the position encoder
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
