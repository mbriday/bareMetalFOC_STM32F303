#ifndef SVPWMTESTER_H
#define SVPWMTESTER_H
#include <stdint.h>

/* mode that computes the sin/cos for each degree in-line */
#define WITHFLOAT

/* To test the Space Vector PWM, in open loop
 * we need to send input commands of the vectors
 *
 * This class generate vectors that should be decoded
 * by the svpwm object.
 *
 * They will be generated periodically, using TIM6
 */
class svpwmTester
{
    unsigned int mAmplitude;
    int16_t mValpha;
    int16_t mVbeta;
public:
    svpwmTester();
    void begin();
    /* between 0 and 1000 */
    void setAmplitude(const unsigned int val);
    /* called by the interrupt
     * angle in degree
    */
    void update(const unsigned int angleDeg);
    /* get value:
     * Valpha bits [31:16]
     * Vbeta  bits [15: 0]
     */
    uint32_t getData();

    void getTimings();
};

extern svpwmTester SvpwmTester;

#endif // SVPWMTESTER_H
