/*
 * xrt_app.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include "xrt_app.h"
#include "xrt_thread.h"
#include "xrt_kernel.h"
#include "xrt_mutex.h"
#include "main.h"

#define THREAD_STACK_SIZE   (128u)

__attribute__((aligned(8))) static uint32_t thread1_stack[THREAD_STACK_SIZE];
__attribute__((aligned(8))) static uint32_t thread2_stack[THREAD_STACK_SIZE];
__attribute__((aligned(8))) static uint32_t thread3_stack[THREAD_STACK_SIZE];

static cdll_node thread1_node;
static TCB_t     tcb1;

static cdll_node thread2_node;
static TCB_t     tcb2;

static cdll_node thread3_node;
static TCB_t     tcb3;

static xrt_mutex_t mutex1;
static xrt_mutex_t mutex2;

static void thread1_exec_function(void);
static void thread2_exec_function(void);
static void thread3_exec_function(void);

bool xrt_app_init(void){

	if(!xrt_thread_create(&tcb1, &thread1_node, thread1_exec_function,
	                      HIGH_PRIORITY, thread1_stack, THREAD_STACK_SIZE)){
		return false;
	}
	if(!xrt_thread_create(&tcb2, &thread2_node, thread2_exec_function,
	                      MEDIUM_PRIORITY, thread2_stack, THREAD_STACK_SIZE)){
		return false;
	}
	if(!xrt_thread_create(&tcb3, &thread3_node, thread3_exec_function,
	                      LOW_PRIORITY, thread3_stack, THREAD_STACK_SIZE)){
		return false;
	}

	if(!xrt_mutex_init(&mutex1)){
		return false;
	}
	if(!xrt_mutex_init(&mutex2)){
		return false;
	}

	return true;
}

void xrt_app_start(void){
	xrt_kernel_start();
}

/*
 * Stepped restore scenario.
 *
 * thread3 (low)    holds mutex1 and mutex2 across a sleep
 * thread1 (high)   queues on mutex1
 * thread2 (medium) queues on mutex2
 *
 * While both are queued thread3 runs at high. Releasing mutex1 should leave
 * it at medium, because mutex2 still owes a medium waiter. Releasing mutex2
 * should take it back to low.
 */

static void thread1_exec_function(void){

	while(1){
		xrt_thread_delay(1000);

		xrt_mutex_lock(&mutex1);
		xrt_mutex_unlock(&mutex1);
	}
}

static void thread2_exec_function(void){
	float a = 10.0f;
	float b = 10.0f;
	float c = a*b;
	while(1){
		xrt_thread_delay(200);

		xrt_mutex_lock(&mutex2);
		xrt_mutex_unlock(&mutex2);
	}
}

static void thread3_exec_function(void){
	float a = 10.0f;
	float b = 10.0f;
	float c = a*b;
	while(1){
		xrt_mutex_lock(&mutex1);
		xrt_mutex_lock(&mutex2);

		/* both waiters queue up here, and thread3 is lifted to high */
		xrt_thread_delay(1000);

		xrt_mutex_unlock(&mutex1);
		/* breakpoint here: tcb3.current_priority should read medium */

		xrt_mutex_unlock(&mutex2);
		/* breakpoint here: tcb3.current_priority should read low */

		xrt_thread_delay(2000);
	}
}
