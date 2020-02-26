/**************************************************************************//**
 * @file     startup_ARMCM4.c
 * @brief    CMSIS Core Device Startup File for
 *           ARMCM4 Device
 * @version  V5.3.1
 * @date     09. July 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

//#if defined (ARMCM4)
//  #include "ARMCM4.h"
//#elif defined (ARMCM4_FP)
//  #include "ARMCM4_FP.h"
//#else
//  #error device not specified!
//#endif
#include <stdint.h>
#include "stm32f3xx.h"
#include "core_cm4.h"

/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t _sidata;    //@ in flash where init global vars are stored
extern uint32_t _sdata;     //start @ in RAM for global vars
extern uint32_t _edata;     //end @ in RAM for global vars
extern uint32_t __bss_start__;  //start @ in RAM for 0 init vars
extern uint32_t __bss_end__;    //end @ in RAM for 0 init vars
extern uint32_t _estack;        //top of stack
extern uint32_t _siccmram;      //@ in flash for code that should be copied to ccmram
extern uint32_t _sccmram ;      //start @ in ccmram (time critical code)
extern uint32_t _eccmram ;      //end @ in ccmram


/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );


/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern void _start     (void) __attribute__((noreturn)); /* PreeMain (C library entry point) */


/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void) ;
void Reset_Handler  (void) ;


/*----------------------------------------------------------------------------
  User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
//<h> Stack Configuration
//  <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
//</h>
#define  __STACK_SIZE  0x00000400
static uint8_t stack[__STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

//<h> Heap Configuration
//  <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
//</h>
#define  __HEAP_SIZE   0x00000C00
#if __HEAP_SIZE > 0
static uint8_t heap[__HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
#endif


/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Exceptions */
void NMI_Handler				(void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler			(void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler			(void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler       	(void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler     	(void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            	(void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler       	(void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         	(void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        	(void) __attribute__ ((weak, alias("Default_Handler")));

/* interrupts */
void WWDG_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler				(void) __attribute__ ((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler				(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_TSC_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_TX_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_RX0_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_RX1_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_SCE_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_TIM15_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler	(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void USART2_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void USART3_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void TIM7_DAC2_IRQHandler		(void) __attribute__ ((weak, alias("Default_Handler")));
void COMP2_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void COMP4_6_IRQHandler			(void) __attribute__ ((weak, alias("Default_Handler")));
void FPU_IRQHandler				(void) __attribute__ ((weak, alias("Default_Handler")));

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
extern const pFunc __Vectors[240];
       const pFunc __Vectors[240] __attribute__ ((section(".isr_vector"))) = {
    (pFunc)((int)(&_estack)),                 /*     Initial Stack Pointer */
	Reset_Handler,                            /*     Reset Handler */
	NMI_Handler,                              /* -14 NMI Handler */
	HardFault_Handler,                        /* -13 Hard Fault Handler */
	MemManage_Handler,                        /* -12 MPU Fault Handler */
	BusFault_Handler,                         /* -11 Bus Fault Handler */
	UsageFault_Handler,                       /* -10 Usage Fault Handler */
	0,                                        /*     Reserved */
	0,                                        /*     Reserved */
	0,                                        /*     Reserved */
	0,                                        /*     Reserved */
	SVC_Handler,                              /*  -5 SVCall Handler */
	DebugMon_Handler,                         /*  -4 Debug Monitor Handler */
	0,                                        /*     Reserved */
	PendSV_Handler,                           /*  -2 PendSV Handler */
	SysTick_Handler,                          /*  -1 SysTick Handler */

	/* Interrupts */
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMP_STAMP_IRQHandler,
	RTC_WKUP_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_TSC_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	CAN_TX_IRQHandler,
	CAN_RX0_IRQHandler,
	CAN_RX1_IRQHandler,
	CAN_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM15_IRQHandler,
	TIM1_UP_TIM16_IRQHandler,
	TIM1_TRG_COM_TIM17_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	0,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	0,
	0,
	SPI1_IRQHandler,
	0,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
    TIM6_DAC_IRQHandler,
	TIM7_DAC2_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	COMP2_IRQHandler,
	COMP4_6_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	FPU_IRQHandler
    /* Interrupts xx .. 224 are left out */
};

void SystemInit();
int main();
void __libc_init_array();
/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void) {
    uint32_t *pSrc, *pDest;

    /* Firstly it copies data from read only memory to RAM.
     *
     */
    pSrc  = &_sidata;
    pDest = &_sdata;
    while( pDest < &_edata ) *pDest++ = *pSrc++;

    /* copy critical code to CCM SRAM
     * to speed it up (no wait states)
     */
    pSrc  = &_siccmram;
    pDest = &_sccmram;
    while( pDest < &_eccmram) *pDest++ = *pSrc++;
    /* TODO: lock the CCM Sram SYSCFG->RCR */

    /* This part of work usually is done in C library startup code.
     * Otherwise, define this macro to enable it in this startup.
     *
     * The BSS section is specified by following symbols
     *   __bss_start__: start of the BSS section.
     *   __bss_end__: end of the BSS section.
     *
     * Both addresses must be aligned to 4 bytes boundary.
     */
    pDest = &__bss_start__;
    while(pDest < &__bss_end__ ) *pDest++ = 0UL;

#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
#endif

    SystemInit();                             /* CMSIS System Initialization */
    //_start();                                 /* Enter PreeMain (C library entry point) */
    __libc_init_array();
    main();
    while(1);
}



/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void) {

  while(1);
}


