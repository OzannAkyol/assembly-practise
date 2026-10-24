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
#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
#include "xrt_port.h"
#include "xrt_system_calls.h"

bool xrt_mutex_init(xrt_mutex_t* mutex_obj){
	if(mutex_obj == NULL){
		return false;
	}

	mutex_obj -> mutex_owner = NULL;
	return cdll_init_list(&mutex_obj -> waiting_list);
}

void xrt_mutex_lock(xrt_mutex_t* mutex_obj){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_MUTEX_LOCK, mutex_obj);
}

void xrt_mutex_unlock(xrt_mutex_t* mutex_obj){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_MUTEX_UNLOCK, mutex_obj);
}

void xrt_mutex_lock_from_svc(xrt_mutex_t* mutex_ptr, TCB_t* running_thread){
	if(mutex_ptr -> mutex_owner == running_thread){
		return;
	}

	if(mutex_ptr -> mutex_owner == NULL ){
		mutex_ptr -> mutex_owner = running_thread;
	}
	else{
		if(running_thread -> current_priority > mutex_ptr -> mutex_owner -> current_priority){
			mutex_ptr -> mutex_owner -> current_priority = running_thread -> current_priority;
			//find the where owner located.
			cdll_list* located_list = mutex_ptr -> mutex_owner -> currently_located_list;
			if(located_list != &xrt_kernel_stopped_list){
				xrt_kernel_reorder_thread(mutex_ptr -> mutex_owner);
			}
		}
		xrt_kernel_block_thread(running_thread, &mutex_ptr->waiting_list, XRT_THREAD_BLOCK_MUTEX);
	}
}

void xrt_mutex_unlock_from_svc(xrt_mutex_t* mutex_ptr, TCB_t* running_thread){
	if(mutex_ptr -> mutex_owner != running_thread){
		return;
	}
	else{
		if(running_thread -> current_priority > running_thread -> base_priority){
			running_thread -> current_priority = running_thread -> base_priority;
		}

		if(mutex_ptr-> waiting_list.head != NULL){
			TCB_t* removed_thread = mutex_ptr-> waiting_list.head -> data;

			cdll_remove_known_node_from_list(&mutex_ptr-> waiting_list, removed_thread -> thread_node);
			xrt_kernel_wake_thread(removed_thread);

			mutex_ptr -> mutex_owner = removed_thread;
		}
		else{
			mutex_ptr -> mutex_owner = NULL;
		}
	}
}
