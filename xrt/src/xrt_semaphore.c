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

bool xrt_semaphore_init(xrt_semaphore_t* semaphore, uint16_t initial_value, uint16_t max_value){
	if(semaphore == NULL || initial_value == 0 || max_value == 0){
		return false;
	}
    cdll_init_list(&semaphore -> waiting_list);
    semaphore -> semaphore_value = initial_value;
    semaphore -> semaphore_max_value = max_value;

    return true;
}

void xrt_semaphore_release(xrt_semaphore_t* semaphore_p){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_SEMAPHORE_RELEASE, semaphore_p);
}

void xrt_semaphore_take(xrt_semaphore_t* semaphore_p){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_SEMAPHORE_TAKE, semaphore_p);
}

void xrt_semaphore_release_from_ISR(xrt_semaphore_t* semaphore_p){
	uint32_t old_pri = xrt_kernel_enter_critical();

	xrt_semaphore_signal_locked(semaphore_p);

	xrt_kernel_exit_critical(old_pri);
}

void xrt_semaphore_signal_locked(xrt_semaphore_t* semaphore_p){
	if(semaphore_p -> waiting_list.head != NULL){
		TCB_t* waiting_thread = (TCB_t*)cdll_get_list_head(&semaphore_p-> waiting_list)->data;
		cdll_remove_known_node_from_list(&semaphore_p-> waiting_list, waiting_thread -> thread_node);

		waiting_thread -> state = THREAD_READY_STATE;
		waiting_thread -> blocked_reason = XRT_BLOCK_NONE;
		cdll_push_data_with_priority_order(&xrt_kernel_ready_list, waiting_thread -> thread_node);
		waiting_thread -> currently_located_list = &xrt_kernel_ready_list;

		TCB_t* running_thread = (TCB_t*)xrt_kernel_running_list.head -> data;

	    if(waiting_thread -> current_priority > running_thread -> current_priority){
	    	xrt_port_request_context_switch();
	    }
	}
	else{
		semaphore_p -> semaphore_value++;
		if(semaphore_p -> semaphore_value > semaphore_p -> semaphore_max_value){
			semaphore_p -> semaphore_value = semaphore_p -> semaphore_max_value;
		}
	}
}

/*
 * Attempts to decrement the semaphore without blocking.
 *
 * @note Must be called from inside a critical section. Returns false when the
 *       semaphore is not available; the caller is responsible for the fallback.
 */
bool xrt_semaphore_try_take_locked(xrt_semaphore_t* semaphore){
    if(semaphore == NULL || semaphore->semaphore_value == 0){
        return false;
    }

    semaphore->semaphore_value--;
    return true;
}

void xrt_semaphore_release_from_svc(xrt_semaphore_t* semaphore){
    // both binary and counting semaphore the semaphore value is reachable.
	xrt_semaphore_signal_locked(semaphore);
}

void xrt_semaphore_take_from_svc(xrt_semaphore_t* semaphore, TCB_t* running_thread){
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
