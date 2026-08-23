#include "main.h"
#include "gpio.h"

#include <stdint.h>
#include "cmsis_gcc.h"
#include <stdbool.h>
#include "xrt_thread.h"
#include "xrt_list.h"
#include "xrt_semaphore.h"
#include "xrt_mutex.h"
#include "xrt_queue.h"

void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

#define NUM_OF_THREAD_IN_SYSTEM (4)

#define THREAD1_STACK_SIZE		1024
__attribute__((aligned(8)))uint32_t thread1_stack[THREAD1_STACK_SIZE];

#define THREAD2_STACK_SIZE		1024
__attribute__((aligned(8)))uint32_t thread2_stack[THREAD2_STACK_SIZE];

#define THREAD3_STACK_SIZE		1024
__attribute__((aligned(8)))uint32_t thread3_stack[THREAD3_STACK_SIZE];

#define THREAD4_STACK_SIZE		1024
__attribute__((aligned(8)))uint32_t thread4_stack[THREAD4_STACK_SIZE];

#define THREAD5_STACK_SIZE		1024
__attribute__((aligned(8)))uint32_t thread5_stack[THREAD5_STACK_SIZE];

void t1_dummy_function(void);
void t2_dummy_function(void);
void t3_dummy_function(void);
void t4_dummy_function(void);
void t5_dummy_function(void);

#define BLOCK_SIZE   64
#define NUM_BLOCKS   4
uint8_t data_queue1[NUM_BLOCKS * BLOCK_SIZE];

#define QUEUE_SIZE		(64)
char send_data[QUEUE_SIZE];
char receive_data[QUEUE_SIZE];

xrtSemaphore_t read_access_semaphore;
xrtSemaphore_t write_access_semaphore;

xrtQueue_t queue1 = {
		.queue_data = data_queue1,
		.block_size = BLOCK_SIZE, // 256 byte
		.queue_capacity = sizeof(data_queue1),
		.write_access_sem = &write_access_semaphore,
		.read_access_sem = &read_access_semaphore,
};

#define BINARY_SEMAPHORE_MAX_VALUE			(1u)
#define BINARY_SEMAPHORE_START_UP_VALUE		(0u)
//primitives
xrtSemaphore_t semaphore1 = {
		.semaphore_max_value = BINARY_SEMAPHORE_MAX_VALUE,
		.semaphore_value = BINARY_SEMAPHORE_START_UP_VALUE,
};

xrtMutex_t mutex1= {
		.mutexOwner = NULL,
};

void t1_dummy_function(void){
	int a = 5;
	int b = 10;

	(void)a;
	(void)b;

	while(1){
		xrt_thread_delay(1500);
//		xrt_mutex_lock(&mutex1);
		if(xrt_queue_receive(&queue1, receive_data)){
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);

			xrt_thread_delay(1500);
		}
//		xrt_mutex_unlock(&mutex1);
	}
}

void t2_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
//		xrt_thread_delay(1000);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
	}
}

void t3_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;


	while(1){
//		xrt_mutex_lock(&mutex1);
		if(xrt_queue_send(&queue1, send_data)){
			HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);

			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
			xrt_thread_delay(750);
		}
//		xrt_mutex_unlock(&mutex1);
	}
}

void t4_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	}
}

void t5_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){

		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
	}
}

cdll_node thread1_node;
cdll_node thread2_node;
cdll_node thread3_node;
cdll_node thread4_node;
cdll_node thread5_node;

TCB_t tcb1 = {
        .fptr = t1_dummy_function,
        .thread_sp = thread1_stack + THREAD1_STACK_SIZE,
        .ThreadStackSize = THREAD1_STACK_SIZE,
        .base_priority = HIGH_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_1,
        .thread_base_ptr = thread1_stack,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
		.currentPriority = HIGH_PRIORITY,
		.thread_node = &thread1_node,
};

TCB_t tcb2 = {
        .fptr = t2_dummy_function,
        .ThreadStackSize = THREAD2_STACK_SIZE,
        .base_priority = MEDIUM_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_2,
        .thread_base_ptr = thread2_stack,
        .thread_sp = thread2_stack + THREAD2_STACK_SIZE,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
		.currentPriority = MEDIUM_PRIORITY,
		.thread_node = &thread2_node,
};

TCB_t tcb3 = {
        .fptr = t3_dummy_function,
        .ThreadStackSize = THREAD3_STACK_SIZE,
        .base_priority = MEDIUM_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_3,
        .thread_base_ptr = thread3_stack,
        .thread_sp = thread3_stack + THREAD3_STACK_SIZE,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
		.currentPriority = MEDIUM_PRIORITY,
		.thread_node = &thread3_node,
};

TCB_t tcb4 = {
        .fptr = t4_dummy_function,
        .ThreadStackSize = THREAD4_STACK_SIZE,
        .base_priority = LOW_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_4,
        .thread_base_ptr = thread4_stack,
        .thread_sp = thread4_stack + THREAD4_STACK_SIZE,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
		.currentPriority = LOW_PRIORITY,
		.thread_node = &thread4_node,
};

TCB_t tcb5 = {
        .fptr = t5_dummy_function,
        .ThreadStackSize = THREAD5_STACK_SIZE,
        .base_priority = LOW_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_5,
        .thread_base_ptr = thread5_stack,
        .thread_sp = thread5_stack + THREAD5_STACK_SIZE,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
		.currentPriority = LOW_PRIORITY,
		.thread_node = &thread5_node,
};


cdll_list list;

TCB_List_t xrtKernelReadyList={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrtKernelRunningList ={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrtKernelStoppedList ={
		.head = NULL,
		.size = 0,
};


int main(void)
{

  FPU->FPCCR &= ~(FPU_FPCCR_LSPEN_Msk | FPU_FPCCR_ASPEN_Msk);

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();

  for(int i = 0; i < THREAD2_STACK_SIZE; ++i){
	  thread1_stack[i] = 11;
	  thread2_stack[i] = 11;
	  thread3_stack[i] = 11;
	  thread4_stack[i] = 11;
	  thread5_stack[i] = 11;
  }

  for(size_t i = 0; i < QUEUE_SIZE; i++){
	  send_data[i] = 'A' + (i % 26);
  }

  xrt_thread_list_init(&xrtKernelRunningList);
  xrt_thread_list_init(&xrtKernelStoppedList);
  xrt_thread_list_init(&xrtKernelReadyList);

  xrt_thread_init(&xrtKernelReadyList, &tcb1);
  xrt_thread_init(&xrtKernelReadyList, &tcb2);
  xrt_thread_init(&xrtKernelReadyList, &tcb3);
  xrt_thread_init(&xrtKernelReadyList, &tcb4);
  xrt_thread_init(&xrtKernelReadyList, &tcb5);

//  xrt_semaphore_init(&semaphore1, semaphore1.semaphore_value, semaphore1.semaphore_max_value);
  xrt_queue_init(&queue1);
//  xrt_mutex_init(&mutex1);
  xrt_set_os_priority_order();

  xrt_thread_start();

  while(1)
  {
	  ;;
  }
}
