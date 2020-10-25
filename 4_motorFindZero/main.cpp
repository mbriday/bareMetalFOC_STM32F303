#include "encoder.h"
#include "pinAccess.h"
#include "serial.h"
#include "stm32f3xx.h"

#define ENU_PIN 13
#define ENV_PIN 14
#define ENW_PIN 15
#define ENU_PORT GPIOB
#define ENV_PORT GPIOB
#define ENW_PORT GPIOB

#define INU_PIN 8
#define INV_PIN 9
#define INW_PIN 10
#define INU_PORT GPIOA
#define INV_PORT GPIOA
#define INW_PORT GPIOA

void setup() {
  pinMode(GPIOA, 5, OUTPUT);

  pinMode(ENU_PORT, ENU_PIN, OUTPUT);
  pinMode(ENV_PORT, ENV_PIN, OUTPUT);
  pinMode(ENW_PORT, ENW_PIN, OUTPUT);
  pinMode(INU_PORT, INU_PIN, OUTPUT);
  pinMode(INV_PORT, INV_PIN, OUTPUT);
  pinMode(INW_PORT, INW_PIN, OUTPUT);

  digitalWrite(ENU_PORT, ENU_PIN, 1);
  digitalWrite(ENV_PORT, ENV_PIN, 1);
  digitalWrite(ENW_PORT, ENW_PIN, 1);
  digitalWrite(INU_PORT, INU_PIN, 0);
  digitalWrite(INV_PORT, INV_PIN, 0);
  digitalWrite(INW_PORT, INW_PIN, 0);

  // start TIM6
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  __asm("nop");
  RCC->APB1RSTR |= RCC_APB1RSTR_TIM6RST;
  RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM6RST;
  __asm("nop");

  // configure TIM6
  TIM6->PSC = 72000 - 1;    // tick@1ms
  TIM6->EGR = TIM_EGR_UG;   // update event => load PSC
  TIM6->CNT = 0xFFFF - 200; // 200ms
  TIM6->SR = 0;

  digitalWrite(GPIOA, 5, 1);
  digitalWrite(INU_PORT, INU_PIN, 1);
  TIM6->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;
  while (!(TIM6->SR & TIM_SR_UIF))
    ; // waiting loop
  Encoder.setValue(0);
  digitalWrite(INU_PORT, INU_PIN, 0);
  digitalWrite(GPIOA, 5, 0);
}

void wait() {
  volatile int i = 0;
  for (i = 0; i < 2000000; i++)
    ;
}

/* main function */
int main(void) {
  setup();
  /* Infinite loop */
  while (1) {
    Serial.printString("pos: ");
    Serial.printInt(Encoder.getValue());
    Serial.printchar('\n');
    wait();
  }
}
