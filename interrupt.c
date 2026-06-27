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
#define STACKED_RETURN_VALUE_PC_OFFSET 	(6u)
#define STACKED_R0_REGISTER_OFFSET 		(0u)

#define dummy_lr_value_offset 			(5u)
#define system_instruction_size 		(2u) //(2 byte goes)
#define PSP_BIT_POS						(2u)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
volatile bool flag = true;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void xrt_first_time_store_contex(void);
void xrt_change_context_list(void);
__attribute__((naked))void xrt_thread_store_context(void);
__attribute__((naked))void xrt_load_context(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern TCB_List_t xrtKernelReadyList;
extern TCB_List_t xrtKernelRunningList;
extern TCB_List_t xrtKernelStoppedList;
extern volatile bool is_os_kernel_started;
extern volatile bool is_os_first_cs_occurs;
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
	uint32_t lr_value = __get_LR();
	uint32_t lr_mask = (1u << PSP_BIT_POS);

	//It means that Process Stack Pointer used.
	if(lr_value & lr_mask){
		uint32_t* thread_sp = (uint32_t*)__get_PSP();
		thread_sp += STACKED_RETURN_VALUE_PC_OFFSET;

		uint8_t* store_value_ptr = (uint8_t*)*thread_sp;
		//Since the system use 2 byte- 16 bit thumb instruction,
		// we already know that PC point to next instruction from the point of causing svc exception handler,
		//so two byte minus offset to hw stacked pc(returnValue)
		store_value_ptr -= system_instruction_size;
		uint8_t svc_immediate_val = (uint8_t)*store_value_ptr;

		switch (svc_immediate_val) {
			case 13: // thread yield property
			{
				SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
			}
			break;
			case 12: // os delay property
			{
				uint32_t* _sp = (uint32_t*)__get_PSP();
				//point to stacked r0 register address
				_sp += STACKED_R0_REGISTER_OFFSET;
				uint32_t r0_val = *_sp;

				cdll_node* running = cdll_get_list_head(&xrtKernelRunningList);
				TCB_t* running_thread = (TCB_t*)running->data;

				running_thread -> wake_tick = r0_val;
				running_thread -> state = THREAD_BLOCKED_STATE;
				running_thread -> blocked_reason = XRT_THREAD_OS_DELAY;

				SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
			}
			break;

			default:
				break;
		}
	}

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


void xrt_first_time_store_contex(void){
	cdll_node* thread_node = xrtKernelReadyList.head;
	TCB_t* xrt_thread = (TCB_t*)thread_node->data;
	cdll_remove_known_node_from_list(&xrtKernelReadyList, thread_node);
	cdll_insert_node_to_head(&xrtKernelRunningList, thread_node);
	__set_PSP((uint32_t)xrt_thread->thread_sp);
}

void xrt_change_context_list(void){
	cdll_node* currently_running = xrtKernelRunningList.head;
	TCB_t* currently_running_thread = (TCB_t*)currently_running -> data;

	cdll_node* currently_ready_node = xrtKernelReadyList.head;
	TCB_t* currently_ready_thread = (TCB_t*)currently_ready_node -> data;

	if(currently_running_thread -> state == THREAD_BLOCKED_STATE){
		if(currently_running_thread -> blocked_reason == XRT_THREAD_OS_DELAY){
			cdll_remove_known_node_from_list(&xrtKernelRunningList, currently_running);
			cdll_insert_node_to_head(&xrtKernelStoppedList, currently_running);

			cdll_node* ready_node = cdll_get_list_head(&xrtKernelReadyList);
			if(ready_node != NULL){
				cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_node);
				TCB_t* ready_thread = (TCB_t*)ready_node -> data;
				ready_thread -> state = THREAD_RUNNING_STATE;
				cdll_insert_node_to_head(&xrtKernelRunningList, ready_node);
			}
		}
		else if(currently_running_thread->blocked_reason == XRT_THREAD_BLOCK_MUTEX){
			;;
		}
		else if(currently_running_thread->blocked_reason == XRT_THREAD_BLOCK_SEMAPHORE){
			;;
		}
	}
	else{
		if(currently_running_thread -> priority > currently_ready_thread -> priority){
			return;
		}
		else if(currently_running_thread -> priority == currently_ready_thread -> priority){
			cdll_remove_known_node_from_list(&xrtKernelRunningList, currently_running);
			currently_running_thread -> state = THREAD_READY_STATE;
			cdll_push_data_with_priority_order(&xrtKernelReadyList, currently_running);

			cdll_remove_known_node_from_list(&xrtKernelReadyList, currently_ready_node);
			currently_ready_thread -> state = THREAD_RUNNING_STATE;
			cdll_insert_node_to_head(&xrtKernelRunningList, currently_ready_node);
		}
		else{
		    // running.priority < ready.priority
		    // Şu an tek senaryo: os_delay'den uyanan yüksek priority thread ready'e döndü.
		    // 2. dal ile aynı davranış şimdilik doğru.
		    // TODO: mutex/semaphore unblock eklenince bu dal ayrıştırılmalı —
		    //       priority inheritance ve owner semantiği farklı ele alınmalı.

			//that's used for the os_delay since delayed thread is highest priority.
			cdll_remove_known_node_from_list(&xrtKernelRunningList, currently_running);
			currently_running_thread -> state = THREAD_READY_STATE;
			cdll_push_data_with_priority_order(&xrtKernelReadyList, currently_running);


			cdll_remove_known_node_from_list(&xrtKernelReadyList, currently_ready_node);
			currently_ready_thread -> state = THREAD_RUNNING_STATE;
			cdll_insert_node_to_head(&xrtKernelRunningList, currently_ready_node);
		}
	}
}

/*
 *@note It must be naked since the inside handler.
 *		GCC push the r7, it assigns sp to r7, and sp shows the handler's stack frame.
 * 		then you try to assign the r7 to related thread's stack, the program won't work
 * 		properly.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
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

    /* ──────── else branch ──────── */
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
  if(is_os_kernel_started){
	  if(tick_count % 1000  == 0){
		  SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	  }

	  if(xrtKernelStoppedList.head != NULL){
		  uint32_t count = xrtKernelStoppedList.size;
		  cdll_node* blocked_node = xrtKernelStoppedList.head;

		  while(count-- > 0){
			  cdll_node* next_node = blocked_node -> next;
			  TCB_t* blocked_thread = (TCB_t*)blocked_node -> data;

			  if(blocked_thread -> blocked_reason == XRT_THREAD_OS_DELAY){
				  blocked_thread-> wake_tick --;
				  if(blocked_thread-> wake_tick == 0){
					  blocked_thread -> blocked_reason = XRT_BLOCK_NONE;
					  blocked_thread -> state = THREAD_READY_STATE;
					  cdll_remove_known_node_from_list(&xrtKernelStoppedList, blocked_node);
					  cdll_push_data_with_priority_order(&xrtKernelReadyList, blocked_node);
				  }
			  }

			  blocked_node = next_node;
		  }
	  }
  }
  /* USER CODE END SysTick_IRQn 1 */
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
