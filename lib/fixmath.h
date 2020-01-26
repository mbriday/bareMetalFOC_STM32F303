#ifndef FIXMATH_H
#define FIXMATH_H
#include <stdint.h>

/* math functions in fix point arithmetic, to save computation time.
 * angles are intergers, using 10 bits: 1024 points/round
 * sin/cos are encoded in 1.15
 */

/* if this define is set, use the fix point values
 * if this is not set, use the standard math.h lib
 *
 * NOTE: the STM32 use a IEE754P single precision FPU
 * when using float contant, use 2.5f instead
 * of 2.5 (double by default), to use the FPU.
 */
#define __USE_PRECALCULATED_COS_TAB__

#ifdef __USE_PRECALCULATED_COS_TAB__
/* angle direcly in steps (1024 steps/round)
 * \return sinus, with a 1.15 fix point encoding
 */
int16_t sin(const int angle);

/* angle direcly in steps (1024 steps/round)
 * \return cosinus, with a 1.15 fix point encoding
 */
int16_t cos(int angle);

/* compute both sinus and cosinus.
 * \return sinus   with 1.15 encoding in 16 most  significant bits
 * \return cosinus with 1.15 encoding in 16 least significant bits
 */
uint32_t sincos(int angle);

#ifdef DEBUG
//output sin/cos signal on serial line for debugging
void debugFixMath();
#endif

#else //__USE_PRECALCULATED_COS_TAB__
    #include <math.h>
#endif

#endif // FIXMATH_H
