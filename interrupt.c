/* USER CODE BEGIN Header */
/**
  **************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  **************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "stm32h7xx_it.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "xrt_thread.h"
#include "xrt_list.h"
#include "cmsis_gcc.h"
#include "xrt_semaphore.h"
#include "xrt_mutex.h"
#include "xrt_system_calls.h"
#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define STACKED_R0_OFFSET               (0u)
#define STACKED_PC_OFFSET               (6u)
#define THUMB_INSTRUCTION_SIZE_IN_BYTE  (2u)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
/* USER CODE END EV */

/**************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/**************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
    volatile uint32_t hfsr = SCB->HFSR;
    volatile uint32_t cfsr = SCB->CFSR;
    volatile uint32_t mmfar = SCB->MMFAR;
    volatile uint32_t bfar = SCB->BFAR;
    (void)hfsr;
    (void)cfsr;
    (void)mmfar;
    (void)bfar;
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */

void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */
	uint32_t* thread_sp = (uint32_t*)__get_PSP();
	uint32_t* svc_arg    = (uint32_t*)*thread_sp;

	thread_sp += STACKED_PC_OFFSET;

	uint8_t* svc_instruction = (uint8_t*)*thread_sp;
	svc_instruction -= THUMB_INSTRUCTION_SIZE_IN_BYTE;

	uint8_t svc_number = *svc_instruction;

	xrt_system_calls_dispatch(svc_number, svc_arg);
/* USER CODE END SVCall_IRQn 0 */

/* USER CODE BEGIN SVCall_IRQn 1 */

/* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/*
 *@note It must be naked since the inside handler.
 *		GCC push the r7, it assigns sp to r7, and sp shows the handler's stack frame.
 * 		then you try to assign the r7 to related thread's stack, the program won't work
 * 		properly.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm("push {r0, lr}");

	__asm("mov r0, #0x50");
	__asm("msr basepri, r0");

    __asm("LDR     r3, =is_os_first_cs_occurs");
    __asm("LDRB    r2, [r3]");
    __asm("CMP     r2, #0");
    __asm("BEQ     else_branch");

    /* ──────── if(flag == true) ──────── */
    __asm("MOV     r2, #0");
    __asm("STRB    r2, [r3]");
    __asm("bl xrt_load_context");

    __asm("add sp, sp, #8");
    __asm("mov r0, #0");
	__asm("msr basepri, r0");
	__asm("ldr lr, =0xFFFFFFFD");
    __asm("bx lr");

    /* ──────── else branch ──────── */
    __asm("else_branch:");

    __asm("bl xrt_thread_store_context");
    __asm("bl xrt_kernel_schedule");
    __asm("bl xrt_load_context");

    __asm("PendSV_Handler_exit:");

    __asm("mov r0, #0");
	__asm("msr basepri, r0");

    __asm("pop {r0, lr}");
	__asm("bx lr");
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
//  uint32_t tick_count = HAL_GetTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  if(is_os_kernel_started){
	 uint32_t old_prio = xrt_kernel_enter_critical();

	 xrt_port_request_context_switch();

	  if(xrtKernelStoppedList.head != NULL){
		  cdll_node* current = xrtKernelStoppedList.head;
		  uint32_t count = xrtKernelStoppedList.size;

		  while(count-- > 0){
		      cdll_node* next_node = current->next;
		      TCB_t* blocked_thread = (TCB_t*)current->data;

		      if(blocked_thread->blocked_reason == XRT_THREAD_OS_DELAY){
		          blocked_thread->wake_tick--;
		          if(blocked_thread->wake_tick == 0){
		              blocked_thread->blocked_reason = XRT_BLOCK_NONE;
		              blocked_thread->state = THREAD_READY_STATE;
		              cdll_remove_known_node_from_list(&xrtKernelStoppedList, current);
		              cdll_push_data_with_priority_order(&xrtKernelReadyList, current);
		              blocked_thread->currently_located_list = &xrtKernelReadyList;
		          }
		      }
		      current = next_node;
		  }
	  }
	  xrt_kernel_exit_critical(old_prio);
  }
  /* USER CODE END SysTick_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
