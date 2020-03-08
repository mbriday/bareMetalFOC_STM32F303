#ifndef ADC_H
#define ADC_H
#include <stdint.h>

//first approach:
// 2 blocking functions that are polling for the ADC
// => 102 for 1 function, 206 for both
//    102/72 = 1.42 µs

class adc
{
public:
    adc();
    void waitADCResult();
};

extern adc Adc;

#endif // ADC_H
