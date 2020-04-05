#ifndef ADC_H
#define ADC_H
#include <stdint.h>
#include "stm32f3xx.h"

#define CURRENT_U_PIN  1
#define CURRENT_V_PIN  1
#define CURRENT_W_PIN  7

#define CURRENT_U_PORT GPIOA //PA1
#define CURRENT_V_PORT GPIOB //PB1 (or PB11)
#define CURRENT_W_PORT GPIOA //PA7

#define CURRENT_U_ADC  ADC1
#define CURRENT_V_ADC  ADC3
#define CURRENT_W_ADC  ADC2

#define CURRENT_U_CHAN 2 //ADC1_IN2
#define CURRENT_V_CHAN 1 //ADC3_IN1
#define CURRENT_W_CHAN 4 //ADC2_IN4

#define NB_ADC         3 //ADC1,2 and 3

//timings: SMP=110 => 181.5 ADC clock cycles => 2.52us
//total: sampling time + 12.5
#define ADC_SMPR       (6U) //sampling time for each channel

class currentReader
{
public:
    currentReader() {};
    /** init the 3 ADC inputs to read current for the 3 shunts.
     *  IMPORTANT NOTE: it assumes that there is only 1 channel
     *  used for each ADC!!
     */
    void begin();
    /** wait for the 3 ADC results (that should occur at the same time...)
     */
    void waitADCResult();
    /** return the ADC raw value for phase U,V and W
     * Note that the ADC should have ended its conversion (waitADCResult())
     */
    inline uint16_t getRawU() {return (uint16_t)(CURRENT_U_ADC->JDR1);};
    inline uint16_t getRawV() {return (uint16_t)(CURRENT_V_ADC->JDR1);};
    inline uint16_t getRawW() {return (uint16_t)(CURRENT_W_ADC->JDR1);};

    /** get the current value from RAW ADC result */
    float getCurrentFloat(uint16_t raw);

    /** return the current in A, with a 2.30 encoded fixpoint value. phase U
     * Note that the ADC should have finished its conversion (waitADCResult())
     */
    int32_t getCurrentU();
    /** return the current in A, with a 2.30 encoded fixpoint value. phase V
     * Note that the ADC should have finished its conversion (waitADCResult())
     */
    int32_t getCurrentV();
    /** return the current in A, with a 2.30 encoded fixpoint value. phase W
     * Note that the ADC should have finished its conversion (waitADCResult())
     */
    int32_t getCurrentW();
};

extern currentReader CurrentReader;

#endif // ADC_H
