/*
 * xrt_semaphore.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stddef.h>

#include "xrt_semaphore.h"
#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
#include "xrt_system_calls.h"

bool xrt_semaphore_init(xrt_semaphore_t* semaphore, uint16_t initial_value, uint16_t max_value){
	if(semaphore == NULL || initial_value > max_value || max_value == 0){
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
		xrt_kernel_wake_thread(waiting_thread);
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
    if(semaphore == NULL || semaphore -> semaphore_value == 0){
        return false;
    }

    semaphore -> semaphore_value--;

    return true;
}

void xrt_semaphore_release_from_svc(xrt_semaphore_t* semaphore){
    // both binary and counting semaphore the semaphore value is reachable.
	xrt_semaphore_signal_locked(semaphore);
}

void xrt_semaphore_take_from_svc(xrt_semaphore_t* semaphore, TCB_t* running_thread){
	if(!xrt_semaphore_try_take_locked(semaphore)){
		xrt_kernel_block_thread(running_thread, &semaphore -> waiting_list, XRT_THREAD_BLOCK_SEMAPHORE);
	}
}
