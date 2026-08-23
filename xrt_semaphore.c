/*
 * xrt_semaphore.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stddef.h>
#include "main.h"

#include "xrt_semaphore.h"
#include "xrt_thread.h"

extern TCB_List_t xrtKernelReadyList;

void xrt_semaphore_init(xrtSemaphore_t* semaphore, uint16_t initial_value, uint16_t max_value){
    cdll_init_list(&semaphore -> waiting_list);
    semaphore -> semaphore_value = initial_value;
    semaphore -> semaphore_max_value = max_value;
}

void xrt_semaphore_release(xrtSemaphore_t* semaphore_p){
    register uint32_t* r0 __asm("r0") = (uint32_t*)semaphore_p;
    __asm volatile("SVC #2" : : "r"(r0));
}

void xrt_semaphore_take(xrtSemaphore_t* semaphore_p){
    register uint32_t* r0 __asm("r0") = (uint32_t*)semaphore_p;
    __asm volatile("SVC #3" : : "r"(r0));
}

void xrt_semaphore_release_from_ISR(xrtSemaphore_t* semaphore_p){
	uint32_t old_pri = xrt_enter_critical_section();

	xrt_semaphore_signal_locked(semaphore_p);

	xrt_exit_critical_section(old_pri);
}

void xrt_semaphore_signal_locked(xrtSemaphore_t* semaphore_p){
	if(semaphore_p -> waiting_list.head != NULL){
		TCB_t* thread = (TCB_t*)cdll_get_list_head(&semaphore_p-> waiting_list)->data;
		cdll_remove_known_node_from_list(&semaphore_p-> waiting_list, thread -> thread_node);

		thread -> state = THREAD_READY_STATE;
		thread -> blocked_reason = XRT_BLOCK_NONE;
		cdll_push_data_with_priority_order(&xrtKernelReadyList, thread -> thread_node);
		thread -> currently_located_list = &xrtKernelReadyList;

		SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	}
	else{
		semaphore_p -> semaphore_value++;
		if(semaphore_p -> semaphore_value > semaphore_p -> semaphore_max_value){
			semaphore_p -> semaphore_value = semaphore_p -> semaphore_max_value;
		}
	}
}

