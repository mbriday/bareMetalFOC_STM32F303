#ifndef __ADC_H__
#define __ADC_H__

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

//use only ADC1
//potentiometer associated to PA3 <=> ADC1_IN4
//basic configuration: no differential input, single conversion mode.
void ADCInit();

//return 12 bits ADC value from PA3. 
uint16_t ADCRead();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
