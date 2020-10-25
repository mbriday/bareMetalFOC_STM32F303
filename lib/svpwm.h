#ifndef SVPWM_H
#define SVPWM_H
#include <stdint.h>

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

/* uncomment this symbol definition to turn on sector debugging.
 * The 'update' function will require an extra parameter which is the real
 * angle. Then it will compare the angle with the one calculated, and light on
 * led on PA.5 in case of problem.
 */
//#define DEBUG_SVPWM_SECTOR

/* uncomment for timing analysis
 * the 'update' time function is measured
 */
//#define TIMINGS_SVPWM_UPDATE

/* Space Vector PWM
 * First version: not optimized for speed!
 */
class svpwm
{
    uint32_t m_timingMax; /*perf analysis only */
    uint32_t m_timingMin; /*perf analysis only */
    /* store sqrt(3) as fix point 2.14
     * precision 1/16384 = 0,000061035
     * 0x6ED9 >> 14 = 1,731994629
     * sqrt(3)      = 1,732050808
     * error is 0.003%
     */
   static const int16_t sqrt3 = 0x6ED9;
    /* store 2/sqrt(3) as fix point 1.15
     * precision 1/32768 = 0,000030518
     * 0x93CD >> 15 = 1,154693604
     * 2/sqrt(3)    = 1,154700538
     * error is 0.0006%
     */
    static const uint32_t twoDivSqrt3 = 0x93CD;
    /* store 1/sqrt(3) as fix point 0.16
     * precision 1/65536 = 0,000030518
     * 0x93CD >> 16 = 0,577346802
     * 1/sqrt(3)    = 0,577350269
     * error is 0.0006%
     */
    const uint32_t oneDivSqrt3 = 0x93CD;
public:
    svpwm();
    /* initialize the pwm, based on TIM1 */
    void begin();
    //enable the 3 power half-briges.
    void enableOutputs();
    //disable the 3 power half-briges.
    //no more current in the motor.
    void disableOutputs();

    /* require an update of the pwm
     * inputs are:
     * int16_t Valpha => 16 most  significant bits
     * int16_t Vbeta  => 16 least significant bits
     * NOTE: Valpha < 18918!! (32767/sqrt(3))
     */
    #ifdef DEBUG_SVPWM_SECTOR
        void update(uint32_t ValphaBeta,unsigned int degree);
    #else
        void update(uint32_t ValphaBeta);
    #endif

    /* get the maximum time measured. In TIM7 ticks */
    uint32_t inline getTimingMax() {return m_timingMax;};
    /* get the maximum time measured. In TIM7 ticks */
    uint32_t inline getTimingMin() {return m_timingMin;};
};

extern svpwm Svpwm;

#endif // SVPWM_H
