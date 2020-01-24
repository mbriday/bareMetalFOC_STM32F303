#ifndef SVPWM_H
#define SVPWM_H

#define ENU_PIN  13
#define ENV_PIN  14
#define ENW_PIN  15
#define EN_PORT  GPIOB

#define INU_PIN  8
#define INV_PIN  9
#define INW_PIN  10
#define IN_PORT  GPIOA

/* alternate function for each channel */
#define AFR_INU 6   //TIM1_CH1
#define AFR_INV 6   //TIM1_CH2
#define AFR_INW 6   //TIM1_CH3

/* Space Vector PWM
 * First version: not optimized for speed!
 */
class svpwm
{
public:
    svpwm();
    //enable the 3 power half-briges.
    void enableOutputs();
    //disable the 3 power half-briges.
    //no more current in the motor.
    void disableOutputs();

    void begin();
};

extern svpwm Svpwm;

#endif // SVPWM_H
