/*
 * xrt_app.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include "xrt_app.h"
#include "xrt_thread.h"
#include "xrt_kernel.h"
#include "xrt_semaphore.h"
#include "main.h"

#define THREAD1_STACK_SIZE		128
__attribute__((aligned(8)))static uint32_t thread1_stack[THREAD1_STACK_SIZE];

#define THREAD2_STACK_SIZE		128
__attribute__((aligned(8)))static uint32_t thread2_stack[THREAD2_STACK_SIZE];

#define THREAD3_STACK_SIZE		128
__attribute__((aligned(8)))static uint32_t thread3_stack[THREAD3_STACK_SIZE];

#define THREAD4_STACK_SIZE		128
__attribute__((aligned(8)))static uint32_t thread4_stack[THREAD4_STACK_SIZE];

#define THREAD5_STACK_SIZE		128
__attribute__((aligned(8))) static uint32_t thread5_stack[THREAD5_STACK_SIZE];

static cdll_node thread1_node;
static TCB_t tcb1;

static cdll_node thread2_node;
static TCB_t tcb2;

static cdll_node thread3_node;
static TCB_t tcb3;

static cdll_node thread4_node;
static TCB_t tcb4;

static cdll_node thread5_node;
static TCB_t tcb5;

#define BINARY_SEMAPHORE_MAX_VALUE			(1u)
#define BINARY_SEMAPHORE_START_UP_VALUE		(0u)

xrtSemaphore_t semaphore1;

static void thread1_exec_function(void);
static void thread2_exec_function(void);
static void thread3_exec_function(void);
static void thread4_exec_function(void);
static void thread5_exec_function(void);

bool xrt_app_init(void){

	if(!xrt_thread_create(&tcb1, &thread1_node, thread1_exec_function, HIGH_PRIORITY, thread1_stack, THREAD1_STACK_SIZE)){
		return false;
	}
	if(!xrt_thread_create(&tcb2, &thread2_node, thread2_exec_function, MEDIUM_PRIORITY, thread2_stack, THREAD2_STACK_SIZE)){
		return false;
	}
	if(!xrt_thread_create(&tcb3, &thread3_node, thread3_exec_function, MEDIUM_PRIORITY, thread3_stack, THREAD3_STACK_SIZE)){
		return false;
	}
	if(!xrt_thread_create(&tcb4, &thread4_node, thread4_exec_function, LOW_PRIORITY, thread4_stack, THREAD4_STACK_SIZE)){
		return false;
	}

	if(!xrt_thread_create(&tcb5, &thread5_node, thread5_exec_function, LOW_PRIORITY, thread5_stack, THREAD5_STACK_SIZE)){
		return false;
	}

	xrt_semaphore_init(&semaphore1, BINARY_SEMAPHORE_START_UP_VALUE, BINARY_SEMAPHORE_MAX_VALUE);

	return true;
}

void xrt_app_start(void){
	xrt_kernel_start();
}

static void thread1_exec_function(void){
	int a = 5;
	int b = 10;

	(void)a;
	(void)b;

	while(1){
		xrt_semaphore_take(&semaphore1);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		xrt_thread_delay(1000);
	}
}

static void thread2_exec_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		xrt_thread_delay(5000);
	}
}

static void thread3_exec_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;


	while(1){
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		xrt_thread_delay(5000);
	}
}

static void thread4_exec_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		xrt_thread_delay(5000);
	}
}

static void thread5_exec_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		xrt_thread_delay(1000);
		xrt_semaphore_release(&semaphore1);
	}
}
