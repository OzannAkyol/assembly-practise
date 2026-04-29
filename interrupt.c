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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

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

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

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
extern TCB_List_t xrtKernelReadyList;
extern TCB_List_t xrtKernelRunningList;
extern volatile bool is_os_kernel_started;
extern volatile bool is_os_first_cs_occurs;

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

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

volatile bool flag = true;
__attribute__((naked))void xrt_thread_store_context(void);
__attribute__((naked))void xrt_thread_store_context(void){
	__asm("push {r7, lr}");
	__asm("mrs r0,  PSP;");
	__asm("ldr r1, =xrtKernelRunningList");
	__asm("ldr r2, [r1, #0]"); // r2 points the head node
	__asm("ldr r3, [r2, #0]"); // r3 points the head node->data
	__asm("add r3, r3, #4");   // r3 points the head node->data + 4 = related threads stack pointer address pointer

	__asm("str r0, [r3, #0]");	// store the currently using stack pointer to tcb->sp.

	__asm("str r4,  [r0, #-4];");
	__asm("str r5,  [r0, #-8];");
	__asm("str r6,  [r0, #-12];");
	__asm("str r7,  [r0, #-16];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r0, #-20];");
	__asm("str r9,  [r0, #-24];");
	__asm("str r10, [r0, #-28];");
	__asm("str r11, [r0, #-32];");

	__asm("pop {r7, lr}");
	__asm("bx lr;");
}

__attribute__((naked))void xrt_load_context(void);
__attribute__((naked))void xrt_load_context(void){
	__asm("push {lr}");
	__asm("ldr r0, =xrtKernelRunningList");
	__asm("ldr r0, [r0, #0]"); // load the address value of head node's
	__asm("ldr r0, [r0, #0]");  // take the node's tcb address.

	__asm("add r0, #4");		// move the thread's stack pointer area.
	__asm("ldr r0, [r0, #0]");	// take the thread's stack pointer value.

	__asm("ldr r4, [r0, #-4]");
	__asm("ldr r5, [r0, #-8]");
	__asm("ldr r6, [r0, #-12]");
	__asm("ldr r7, [r0, #-16]");
	__asm("ldr r8, [r0, #-20]");
	__asm("ldr r9, [r0, #-24]");
	__asm("ldr r10,[r0, #-28]");
	__asm("ldr r11, [r0, #-32];");

	__asm("msr PSP, r0");	//set psp to thread's hardware stack pointer.
	__asm("pop {lr}");
	__asm("bx lr");

}

__attribute__((naked)) void xrt_first_time_store_contex(void);
__attribute__((naked)) void xrt_first_time_store_contex(void){
	__asm("push {r7, lr}");
	__asm("ldr r1, =xrtKernelRunningList");
	__asm("ldr r2, [r1, #0]"); // r2 points the head node
	__asm("ldr r3, [r2, #0]"); // r3 points the head node->data
	__asm("add r3, r3, #4");   // r3 points the head node->data + 4 = related threads stack pointer address pointer
	__asm("ldr r3, [r3, #0]");   // r3 points the head node->data + 4 = related threads stack pointer address pointer


	__asm("ldr r4, [r3, #-4]");
	__asm("ldr r5, [r3, #-8]");
	__asm("ldr r6, [r3, #-12]");
	__asm("ldr r7, [r3, #+32]"); // r7 points to pendSV handler.
	__asm("ldr r8, [r3, #-20]");
	__asm("ldr r9, [r3, #-24]");
	__asm("ldr r10, [r3, #-28]");
	__asm("ldr r11, [r3, #-32]");

	__asm("msr psp, r3");	// store the currently using stack pointer to process stack pointer.
	__asm("pop {r7, lr}");
	__asm("bx lr;");
}

__attribute__((naked))void xrt_change_context_list(void);
__attribute__((naked))void xrt_change_context_list(void){
	__asm("push	{r7}");
	__asm("sub	sp, #24");
	__asm("add	r7, sp, #0");

	__asm("push	{lr}");
	__asm("ldr r0, =xrtKernelRunningList");
	__asm("ldr r1, [r0, #0]");
	__asm("str r1, [r7, #0]");			//__asm("push r1");
	__asm("bl cdll_remove_known_node_from_list");
	__asm("ldr r1, [r7, #0]");			//__asm("pop r1");
	__asm("pop	{lr}");
//	cdll_node* running_node = xrtKernelRunningList.head;
//	cdll_remove_known_node_from_list(&xrtKernelRunningList, xrtKernelRunningList.head);

	__asm("push	{lr}");
//	cdll_insert_node_to_list(&xrtKernelReadyList, running_node);
	__asm("ldr r0, =xrtKernelReadyList");
	__asm("mov r1, r1"); //r1 has running_node address.
	__asm("str r0, [r7, #0]"); //__asm("push {r0}"); // r0 holds the xrtKernelReadyList address
	__asm("bl cdll_insert_node_to_list");
	__asm("ldr r0, [r7, #0]");//__asm("pop {r0}");
	__asm("pop	{lr}");
//	cdll_insert_node_to_list(&xrtKernelReadyList, running_node);

	__asm("push	{lr}");
	//removed node from xrtKernelReadyList
	__asm("mov r0, r0");	//r0 points to xrtKernelReadyList
	__asm("ldr r1, [r0, #0]");//r1 points the xrtKernelReadyList.head
	__asm("str r1, [r7, #0]");//__asm("push r1");
	__asm("bl cdll_remove_known_node_from_list");
	__asm("ldr r1, [r7, #0]");//__asm("pop r1");	//r1 points the xrtKernelReadyList.head
	__asm("pop	{lr}");
//	cdll_node* node = xrtKernelReadyList.head;
//	cdll_remove_known_node_from_list(&xrtKernelReadyList, xrtKernelReadyList.head);

	__asm("push	{lr}");
	__asm("ldr r0, =xrtKernelRunningList");
	__asm("mov r1, r1");
	__asm("bl cdll_insert_node_to_list");
	__asm("pop	{lr}");
//	cdll_insert_node_to_list(&xrtKernelRunningList, node);

	__asm("add	sp, #24");
	__asm("pop {r7}");
	__asm("bx lr");
}
/*
 *@note It must be naked since the inside handler.
 *		GCC push the r7, it assigns sp to r7, and sp shows the handler's stack frame.
 * 		then you try to assign the r7 to related thread's stack, the program won't work
 * 		properly.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
//	__asm("push {r7}");

    __asm("LDR     r3, =is_os_first_cs_occurs");
    __asm("LDRB    r2, [r3]");
    __asm("CMP     r2, #0"); 	//if the firstly, cs occurs.
    __asm("BEQ     else_branch");

    /* ──────── if(flag == true) ──────── */
    __asm("MOV     r2, #0");		//set flag false
    __asm("STRB    r2, [r3]");

    __asm("push {lr}");
    __asm("bl xrt_first_time_store_contex");
    __asm("pop {lr}");
    __asm("B PendSV_Handler_exit");

    __asm("else_branch:");

    __asm("push {lr}");
    __asm("bl xrt_thread_store_context");
    __asm("pop {lr}");

    __asm("push {lr}");
    __asm("bl xrt_change_context_list");
    __asm("pop {lr}");

    __asm("push {lr}");
    __asm("bl xrt_load_context");
    __asm("pop {lr}");

    __asm("PendSV_Handler_exit:");
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
  uint32_t tick_count = HAL_GetTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  if(is_os_kernel_started && tick_count % 150 == 0){
	  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
  /* USER CODE END SysTick_IRQn 1 */
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
