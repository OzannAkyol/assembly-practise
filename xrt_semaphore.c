/*
 * xrt_semaphore.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stddef.h>

#include "xrt_semaphore.h"
#include "xrt_thread.h"
#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
#include "xrt_system_calls.h"

void xrt_semaphore_init(xrtSemaphore_t* semaphore, uint16_t initial_value, uint16_t max_value){
    cdll_init_list(&semaphore -> waiting_list);
    semaphore -> semaphore_value = initial_value;
    semaphore -> semaphore_max_value = max_value;
}

void xrt_semaphore_release(xrtSemaphore_t* semaphore_p){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_SEMAPHORE_RELEASE, semaphore_p);
}

void xrt_semaphore_take(xrtSemaphore_t* semaphore_p){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_SEMAPHORE_TAKE, semaphore_p);
}

void xrt_semaphore_release_from_ISR(xrtSemaphore_t* semaphore_p){
	uint32_t old_pri = xrt_kernel_enter_critical();

	xrt_semaphore_signal_locked(semaphore_p);

	xrt_kernel_exit_critical(old_pri);
}

void xrt_semaphore_signal_locked(xrtSemaphore_t* semaphore_p){
	if(semaphore_p -> waiting_list.head != NULL){
		TCB_t* thread = (TCB_t*)cdll_get_list_head(&semaphore_p-> waiting_list)->data;
		cdll_remove_known_node_from_list(&semaphore_p-> waiting_list, thread -> thread_node);

		thread -> state = THREAD_READY_STATE;
		thread -> blocked_reason = XRT_BLOCK_NONE;
		cdll_push_data_with_priority_order(&xrtKernelReadyList, thread -> thread_node);
		thread -> currently_located_list = &xrtKernelReadyList;

		xrt_port_request_context_switch();

	}
	else{
		semaphore_p -> semaphore_value++;
		if(semaphore_p -> semaphore_value > semaphore_p -> semaphore_max_value){
			semaphore_p -> semaphore_value = semaphore_p -> semaphore_max_value;
		}
	}
}

void xrt_semaphore_release_from_svc(xrtSemaphore_t* semaphore, TCB_t* running_thread){
    // both binary and counting semaphore the semaphore value is reachable.
    if(semaphore -> waiting_list.head != NULL){
        TCB_t* waiting_thread = (TCB_t*)semaphore -> waiting_list.head -> data;

        cdll_remove_known_node_from_list(&semaphore -> waiting_list, waiting_thread -> thread_node);
	    cdll_push_data_with_priority_order(&xrtKernelReadyList, waiting_thread -> thread_node);
	    waiting_thread -> currently_located_list  = &xrtKernelReadyList;
	    waiting_thread -> blocked_reason = XRT_BLOCK_NONE;
	    waiting_thread -> state = THREAD_READY_STATE;
	    if(waiting_thread -> current_priority > running_thread -> current_priority){
	    	xrt_port_request_context_switch();
	      }
	 }
	 else{
		 if(semaphore -> semaphore_value < semaphore -> semaphore_max_value){
			 semaphore -> semaphore_value++;
		 }
    }
}

void xrt_semaphore_take_from_svc(xrtSemaphore_t* semaphore, TCB_t* running_thread){
	//it means that the resource could be taken.
	if(semaphore -> semaphore_value > 0){
		semaphore -> semaphore_value--;
	}
	else{
		running_thread -> state = THREAD_BLOCKED_STATE;
		running_thread -> blocked_reason = XRT_THREAD_BLOCK_SEMAPHORE;
	    running_thread -> currently_located_list = &semaphore -> waiting_list;
	    xrt_port_request_context_switch();
	  }
}
