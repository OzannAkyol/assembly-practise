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
__attribute__((naked))void xrt_first_time_store_contex(void);
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
		uint32_t* thread_pc_reg = thread_sp + STACKED_RETURN_VALUE_PC_OFFSET;
		uint32_t* r0_reg = thread_sp;

		uint8_t* svc_instruction_address = (uint8_t*)*thread_pc_reg;
		//Since the system use 2 byte- 16 bit thumb instruction,
		// we already know that PC point to next instruction from the point of causing svc exception handler,
		//so two byte minus offset to hw stacked pc(returnValue)
		svc_instruction_address -= system_instruction_size;
		uint8_t svc_immediate_val = (uint8_t)*svc_instruction_address;

		TCB_t* running_thread = (TCB_t*)cdll_get_list_head(&xrtKernelRunningList) -> data;

		switch (svc_immediate_val)
		{
			case 13: // thread yield property
			{
				SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
			}
			break;
			case 12: // os delay property
			{

				running_thread -> wake_tick = *r0_reg;
				running_thread -> state = THREAD_BLOCKED_STATE;
				running_thread -> blocked_reason = XRT_THREAD_OS_DELAY;
				running_thread -> currently_located_list = &xrtKernelStoppedList;

				SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
			}
			break;
			case 2: // os semaphore release property
			{
			    xrtSemaphore_t* semaphore_ptr = (xrtSemaphore_t*)*r0_reg; // it points to which semaphore resource taken.
			    // both binary and counting semaphore the semaphore value is reachable.
			    if(semaphore_ptr -> waiting_list.head != NULL){
			        TCB_t* waiting_thread = (TCB_t*)semaphore_ptr -> waiting_list.head -> data;

			        cdll_remove_known_node_from_list(&semaphore_ptr -> waiting_list, waiting_thread -> thread_node);
			        cdll_push_data_with_priority_order(&xrtKernelReadyList, waiting_thread -> thread_node);
			        waiting_thread -> currently_located_list  = &xrtKernelReadyList;

			        if(waiting_thread -> currentPriority > running_thread -> currentPriority){
			            SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
			        }
			    }
			    else{
			        if(semaphore_ptr -> semaphore_value < semaphore_ptr -> semaphore_max_value){
			            semaphore_ptr -> semaphore_value++;
			        }
			    }
			}
			break;

			case 3: // os semaphore take property
			{
			    xrtSemaphore_t* semaphore_ptr = (xrtSemaphore_t*)*r0_reg;
			    //it means that the resource could be taken.
			    if(semaphore_ptr -> semaphore_value > 0){
			        semaphore_ptr -> semaphore_value--;
			        break;
			    }
			    else{
			        running_thread -> state = THREAD_BLOCKED_STATE;
			        running_thread -> blocked_reason = XRT_THREAD_BLOCK_SEMAPHORE;
			        running_thread -> currently_located_list = &semaphore_ptr -> waiting_list;
			        SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
			    }

			}
			break;
			case 4: // mutex lock property.
			{
				xrtMutex_t* mutex_ptr = (xrtMutex_t*)*r0_reg;
				if(mutex_ptr -> mutexOwner == NULL ){
					mutex_ptr -> mutexOwner = running_thread;
				}
				else{
					if(mutex_ptr -> mutexOwner == running_thread){
						break;
					}

					if(running_thread -> currentPriority > mutex_ptr -> mutexOwner -> currentPriority){
						mutex_ptr -> mutexOwner -> currentPriority = running_thread -> currentPriority;
						//find the where owner located.
						cdll_list* located_list = mutex_ptr -> mutexOwner -> currently_located_list;
						if(located_list != &xrtKernelStoppedList){
							cdll_remove_known_node_from_list(located_list, mutex_ptr -> mutexOwner -> thread_node);
							cdll_push_data_with_priority_order(located_list, mutex_ptr -> mutexOwner -> thread_node);
						}
					}

					running_thread -> currently_located_list = &mutex_ptr -> waitingList;
					running_thread -> state = THREAD_BLOCKED_STATE;
					running_thread -> blocked_reason = XRT_THREAD_BLOCK_MUTEX;

					SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;

				}
			}
				break;
			case 5: // mutex unlock property.
			{
				xrtMutex_t* mutex_ptr = (xrtMutex_t*)*r0_reg;
				if(mutex_ptr -> mutexOwner == NULL){
					//to prevent unprivileged request
					break;
				}
				else{
					if(mutex_ptr -> mutexOwner != running_thread){
						break;
					}
					else{
						if(running_thread -> currentPriority > running_thread -> base_priority){
							running_thread -> currentPriority = running_thread -> base_priority;
						}

						if(mutex_ptr-> waitingList.head != NULL){

							TCB_t* removed_thread = mutex_ptr-> waitingList.head -> data;

							cdll_remove_known_node_from_list(&mutex_ptr-> waitingList, removed_thread -> thread_node);

							cdll_push_data_with_priority_order(&xrtKernelReadyList, removed_thread -> thread_node);
							removed_thread -> currently_located_list = &xrtKernelReadyList;
							removed_thread -> blocked_reason = XRT_BLOCK_NONE;
							removed_thread -> state = THREAD_READY_STATE;
							mutex_ptr -> mutexOwner = removed_thread;

							if(removed_thread -> currentPriority > running_thread -> currentPriority){
								SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
							}
						}
						else{
							mutex_ptr -> mutexOwner = NULL;
						}
					}
				}
			}
				break;

			default:
				break;
		}
	}
	else{
	//TODO: it is written for dummy os start need to try best way.
			is_os_first_cs_occurs = true;
			is_os_kernel_started = true;

			TCB_t* ready_thread = (TCB_t*)cdll_get_list_head(&xrtKernelReadyList) -> data;

			cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_thread -> thread_node);

			cdll_push_data_with_priority_order(&xrtKernelRunningList, ready_thread -> thread_node);
			ready_thread -> currently_located_list = &xrtKernelRunningList;
			ready_thread -> state = THREAD_RUNNING_STATE;

			SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
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

void xrt_change_context_list(void){

	if(xrtKernelReadyList.head == NULL){
		//must return idle thread
		return;
	}

	TCB_t* running_thread = (TCB_t*)xrtKernelRunningList.head ->data;
	TCB_t* ready_thread = (TCB_t*)xrtKernelReadyList.head -> data;

	if(running_thread -> state == THREAD_BLOCKED_STATE){
		cdll_remove_known_node_from_list(&xrtKernelRunningList, running_thread -> thread_node);
		//in SVC Handler change the thread state, reason and currently located list updated.
		cdll_push_data_with_priority_order(running_thread -> currently_located_list, running_thread -> thread_node);

		cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_thread -> thread_node);
		ready_thread-> state = THREAD_RUNNING_STATE;
		cdll_insert_node_to_head(&xrtKernelRunningList, ready_thread -> thread_node);
		ready_thread -> currently_located_list = &xrtKernelRunningList;
	}
	else{
		if(running_thread -> currentPriority > ready_thread -> currentPriority){
			return;
		}
		else{
		    // running.priority < ready.priority, occurs when os primitive(semaphore and mutex) used,
			cdll_remove_known_node_from_list(&xrtKernelRunningList, running_thread -> thread_node);
			running_thread -> state = THREAD_READY_STATE;
			cdll_push_data_with_priority_order(&xrtKernelReadyList, running_thread -> thread_node);
			running_thread -> currently_located_list = &xrtKernelReadyList;

			cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_thread -> thread_node);
			ready_thread -> state = THREAD_RUNNING_STATE;
			cdll_insert_node_to_head(&xrtKernelRunningList, ready_thread -> thread_node);
			ready_thread -> currently_located_list = &xrtKernelRunningList;
		}
	}
}

__attribute__((naked))void xrt_thread_store_context(void){
	__asm("mrs r0,  PSP;");
	__asm("sub r0, r0, #32");

	__asm("ldr r1, =xrtKernelRunningList");
	__asm("ldr r1, [r1, #0]"); // r1 points the head node
	__asm("ldr r1, [r1, #0]"); // r1 points the head node->data
	__asm("add r1, r1, #4");   // r1 points the head node->data + 4 = related threads stack pointer address pointer

	__asm("str r0, [r1, #0]");	// store the currently using stack pointer to tcb->sp(sw stack frame).

	__asm("str r4,  [r0, #28];");
	__asm("str r5,  [r0, #24];");
	__asm("str r6,  [r0, #20];");
	__asm("str r7,  [r0, #16];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r0, #12];");
	__asm("str r9,  [r0, #8];");
	__asm("str r10, [r0, #4];");
	__asm("str r11, [r0, #0];");

	__asm("bx lr;");
}

__attribute__((naked))void xrt_load_context(void){
	__asm("ldr r0, =xrtKernelRunningList");
	__asm("ldr r0, [r0, #0]"); // load the address value of head node's
	__asm("ldr r0, [r0, #0]");  // take the node's tcb address.

	__asm("add r0, #4");		// move the thread's stack pointer area.
	__asm("ldr r0, [r0, #0]");	// take the thread's stack pointer value.

	__asm("ldr r4, [r0, #28]");
	__asm("ldr r5, [r0, #24]");
	__asm("ldr r6, [r0, #20]");
	__asm("ldr r7, [r0, #16]");
	__asm("ldr r8, [r0, #12]");
	__asm("ldr r9, [r0, #8]");
	__asm("ldr r10,[r0, #4]");
	__asm("ldr r11, [r0, #0];");

	__asm("add r0, r0, #32");
	__asm("msr PSP, r0");	//set psp to thread's hardware stack pointer.
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
    __asm("LDR     r3, =is_os_first_cs_occurs");
    __asm("LDRB    r2, [r3]");
    __asm("CMP     r2, #0"); 	//if the firstly, cs occurs.
    __asm("BEQ     else_branch");

    /* ──────── if(flag == true) ──────── */
    __asm("MOV     r2, #0");		//set flag false
    __asm("STRB    r2, [r3]");

    __asm("push {lr}");
    __asm("bl xrt_load_context");
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
	__asm("ldr lr, =0xFFFFFFFD");
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
  }
  /* USER CODE END SysTick_IRQn 1 */
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
