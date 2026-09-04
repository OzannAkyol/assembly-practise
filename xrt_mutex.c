/*
 * xrt_mutex.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stdbool.h>
#include <stddef.h>

#include "xrt_list.h"
#include "xrt_mutex.h"
#include "xrt_kernel_internal.h"
#include "xrt_port.h"
#include "xrt_system_calls.h"

bool xrt_mutex_init(xrtMutex_t* mutex_obj){
	mutex_obj -> mutexOwner = NULL;
	return cdll_init_list(&mutex_obj -> waitingList);
}

void xrt_mutex_lock(xrtMutex_t* mutex_obj){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_MUTEX_LOCK, mutex_obj);
}

void xrt_mutex_unlock(xrtMutex_t* mutex_obj){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_MUTEX_UNLOCK, mutex_obj);
}

void xrt_mutex_lock_from_svc(xrtMutex_t* mutex_ptr, TCB_t* running_thread){
	if(mutex_ptr -> mutexOwner == running_thread){
		return;
	}

	if(mutex_ptr -> mutexOwner == NULL ){
		mutex_ptr -> mutexOwner = running_thread;
	}
	else{
		if(running_thread -> current_priority > mutex_ptr -> mutexOwner -> current_priority){
			mutex_ptr -> mutexOwner -> current_priority = running_thread -> current_priority;
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

		xrt_port_request_context_switch();
	}
}

void xrt_mutex_unlock_from_svc(xrtMutex_t* mutex_ptr, TCB_t* running_thread){
	if(mutex_ptr -> mutexOwner != running_thread){
		return;
	}
	else{
		if(running_thread -> current_priority > running_thread -> base_priority){
			running_thread -> current_priority = running_thread -> base_priority;
		}

		if(mutex_ptr-> waitingList.head != NULL){
			TCB_t* removed_thread = mutex_ptr-> waitingList.head -> data;

			cdll_remove_known_node_from_list(&mutex_ptr-> waitingList, removed_thread -> thread_node);

			cdll_push_data_with_priority_order(&xrtKernelReadyList, removed_thread -> thread_node);
			removed_thread -> currently_located_list = &xrtKernelReadyList;
			removed_thread -> blocked_reason = XRT_BLOCK_NONE;
			removed_thread -> state = THREAD_READY_STATE;
			mutex_ptr -> mutexOwner = removed_thread;

			if(removed_thread -> current_priority > running_thread -> current_priority){
				xrt_port_request_context_switch();
			}
		}
		else{
			mutex_ptr -> mutexOwner = NULL;
		}
	}
}
