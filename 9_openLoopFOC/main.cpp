//picocom --omap igncr --imap lfcrlf --baud 115200 --parity n --flow none --databits 8 --stopbits 1 --echo --noreset --noinit /dev/ttyACM0

#include "stm32f3xx.h"
#include "pinAccess.h"
#include "serial.h"
#include "encoder.h"
#include "fixmath.h"
#include "svpwm.h"
#include <stdlib.h>
#include <string.h>

#define ARM_MATH_CM4
#include "arm_math.h"

#define NB_POLES 7

extern "C" uint32_t getCPUFreq(); //from startup_clock.

void initMotorPosition()
{
  Svpwm.begin();
  // configure TIM6
  TIM6->PSC = 72000 - 1;    // tick@1ms
  TIM6->EGR = TIM_EGR_UG;   // update event => load PSC
  TIM6->CNT = 0xFFFF - 400; // 200ms
  TIM6->SR = 0;

  Svpwm.update(800 << 16); //only Valpha 80%
  Svpwm.enableOutputs();
  TIM6->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;
  while (!(TIM6->SR & TIM_SR_UIF))
    ; // waiting loop
  Encoder.setValue(0);
  Svpwm.disableOutputs();
}

void setup()
{
    pinMode(GPIOA,5,OUTPUT); /* user led */

    //start TIM6 and TIM7
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN | RCC_APB1ENR_TIM7EN;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM6RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
    __asm("nop");
    RCC->APB1RSTR |=  RCC_APB1RSTR_TIM7RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
    __asm("nop");

    initMotorPosition();
    //Svpwm.enableOutputs();

    //configure TIM7 as stopwatch:
    TIM7->PSC = 0; //max freq
    TIM7->EGR = TIM_EGR_UG;   // update event => load PSC
    TIM7->CNT = 0;
    TIM7->CR1 = TIM_CR1_CEN;

    //configure TIM6 => 10KHz
    TIM6->CR1 = 0; //reinit TIM6
    TIM6->PSC = 72-1;  //tick@1us
    TIM6->EGR = TIM_EGR_UG;   // update event => load PSC
    TIM6->ARR = 100-1;     //100us => 10KHz
    TIM6->CR1 = TIM_CR1_CEN; //enable, all other fields to 0

    //start interrupts.
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC_SetPriority(TIM6_DAC_IRQn,0); //max prio.
}

#define LOG_SIZE 32

typedef struct {
    int id;
    int elPos;  //electric position. Base for calculus.
    int pos;    //value returned by the encoder.
    //int16_t s;
    //int16_t c;
    //int16_t alpha;
    //int16_t beta;
    int16_t duration;
} info_t;

typedef struct {
    info_t info[LOG_SIZE];
    int idxR = 0;
    int idxW = 0;
} buf_t;

buf_t buffer;


/* open loop:
 * - get angle
 * - compute inverse park
 * - set svpwm
 * => 72MHz, debug mode
 *    => max 402 ticks => 5.58us => max ~179KHz
 * => 72MHz, svpwm with CCMRAM, release
 *    => max 168 ticks => 2.33us => max ~428KHz
 */
extern "C" void TIM6_DAC_IRQHandler()
{
    static int id = 0;
    static int p = 0;
    static uint16_t duration = 0;
    TIM7->CNT = 0;
    GPIOA->BSRR = GPIO_BSRR_BS_5; //led
    /* ack */
    TIM6->SR &= ~TIM_SR_UIF;

    const int32_t Vq = 400;
    const int32_t Vd = 0;

    //first => get angle
    p++;
    const int32_t pos = Encoder.getValue();
    const int32_t elPos = ((pos >> 2)*NB_POLES) & 0x3FF; // electric position, mod 1024
    const uint32_t sc=sincos(elPos);   // mod 1024
    const int16_t cI = (int16_t)(sc & 0xffff);
    const int16_t sI = (int16_t)(sc >> 16);

    //inverse park:
    // * Valpha = Vd.cos - Vq.sin
    // * Vbeta  = Vd.sin + Vq.cos
    const int32_t valphaI = ((Vd*cI)-(Vq*sI)) >> 15;
    const int32_t vbetaI  = ((Vd*sI)+(Vq*cI)) >> 15;


    //then svpwm
    Svpwm.update((valphaI<<16)|(vbetaI&0xFFFF));

    //time elapsed in interrupt

    if(TIM7->CNT > duration) duration = TIM7->CNT;
    GPIOA->BSRR = GPIO_BSRR_BR_5; //reset led (PA.5)

    //log
    if(((buffer.idxR + 1) % LOG_SIZE) != buffer.idxW){
        const int idxW = buffer.idxW;
        buffer.info[idxW].id = id;
        buffer.info[idxW].elPos = elPos;
        buffer.info[idxW].pos = pos;
        //buffer.info[idxW].s = sI;
        //buffer.info[idxW].c = cI;
        //buffer.info[idxW].alpha = valphaI;
        //buffer.info[idxW].beta = vbetaI;
        buffer.info[idxW].duration = duration;
        buffer.idxW = (buffer.idxW + 1) % LOG_SIZE;
    }
    id++;
}

void bp()
{
    static int bpState = 0;
    static bool output = false;

    switch (bpState) {
    case 0: if(digitalRead(GPIOC,13) == 0) bpState = 1; break; //OFF
    case 1: bpState = 2; break; //push
    case 2: if(digitalRead(GPIOC,13) == 1) bpState = 0; break; //on
    }
    if(bpState == 1) {
        if(output) Svpwm.disableOutputs();
        else Svpwm.enableOutputs();
        output = !output;
    }
}

/* main function */
int main(void)
{
    info_t tmp;
    Serial.printString("Start\n");
    pinMode(GPIOC,13,INPUT_PULLUP);
    setup();
    /* Infinite loop */
    while (1)
    {
        if(buffer.idxR != buffer.idxW)
        {
            __disable_irq();
            memcpy(&tmp,&buffer.info[buffer.idxR],sizeof(info_t));
            buffer.idxR = (buffer.idxR + 1) % LOG_SIZE;
            __enable_irq();
            Serial.printInt(tmp.id,10,5);
            Serial.printString(" ; ");
            Serial.printInt(tmp.elPos,10,4);
            Serial.printString(" ; ");
            Serial.printInt(tmp.pos,10,6);
            Serial.printString(" ; ");
            Serial.printInt(tmp.duration);
            Serial.printchar('\n');
        }
        bp();
    }
}
