/*
 * xrt_mutex.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stdbool.h>
#include <stddef.h>

#include "xrt_mutex.h"
#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
#include "xrt_syscalls_internal.h"
#include "xrt_system_calls.h"

static void xrt_mutex_set_owner(xrt_mutex_t* mutex, TCB_t* thread);
static void xrt_mutex_clear_owner(xrt_mutex_t* mutex);
static void start_boost_chain(xrt_mutex_t* m, TCB_t* t);
static void remove_boost_chain(TCB_t* running_thread);

bool xrt_mutex_init(xrt_mutex_t* mutex_obj){
	if(mutex_obj == NULL){
		return false;
	}

	mutex_obj -> mutex_owner = NULL;

	mutex_obj -> m_node.data = mutex_obj;
	mutex_obj -> m_node.next = &mutex_obj-> m_node;
	mutex_obj -> m_node.prev = &mutex_obj-> m_node;

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

	if(mutex_ptr -> mutex_owner == NULL){
		xrt_mutex_set_owner(mutex_ptr, running_thread);
	}
	else{
		start_boost_chain(mutex_ptr, running_thread);
		//used there for now.
		running_thread -> mutex_blocked_on = mutex_ptr;
		xrt_kernel_block_thread(running_thread, &mutex_ptr->waiting_list, XRT_THREAD_BLOCK_MUTEX);
	}
}

void xrt_mutex_unlock_from_svc(xrt_mutex_t* mutex_ptr, TCB_t* running_thread){
	if(mutex_ptr -> mutex_owner != running_thread){
		return;
	}
	else{
		xrt_mutex_clear_owner(mutex_ptr);

		if(running_thread -> current_priority > running_thread -> base_priority){
			remove_boost_chain(running_thread);
		}

		if(mutex_ptr-> waiting_list.head != NULL){
			TCB_t* removed_thread = mutex_ptr-> waiting_list.head -> data;
			cdll_remove_known_node_from_list(&mutex_ptr-> waiting_list, removed_thread -> thread_node);
			removed_thread -> mutex_blocked_on = NULL;

			xrt_mutex_set_owner(mutex_ptr, removed_thread);

			xrt_kernel_wake_thread(removed_thread);
		}
	}
}

static void remove_boost_chain(TCB_t* running_thread){
	cdll_node* current = cdll_get_list_head(&running_thread ->mutex_held);
	uint8_t remaining = running_thread ->mutex_held.size;

	Priority_t highest = running_thread -> base_priority;

	// we need to traverse held mutexes.
	while(remaining-- > 0){
		xrt_mutex_t* held  = (xrt_mutex_t*)current ->data;

		cdll_node*   waiter_node = cdll_get_list_head(&held -> waiting_list);
		if(waiter_node != NULL){
			TCB_t* waiter = (TCB_t*)waiter_node -> data;

			if(waiter-> current_priority > highest){
				highest = waiter -> current_priority;
			}
		}
		current = current -> next;
	}

	running_thread -> current_priority = highest;
}

static void start_boost_chain(xrt_mutex_t* m, TCB_t* t){
	if(t -> current_priority > m -> mutex_owner -> current_priority){
		//we boost first.
		m -> mutex_owner -> current_priority = t -> current_priority;

		//find the where owner located.
		cdll_list* located = m -> mutex_owner -> currently_located_list;
		if(located != &xrt_kernel_stopped_list){
			xrt_kernel_reorder_thread(m -> mutex_owner);
		}
		//check owner wait another mutex,
		if(m -> mutex_owner -> blocked_reason == XRT_THREAD_BLOCK_MUTEX &&
		   m -> mutex_owner -> mutex_blocked_on != NULL){
			// when owner wait another mutex, if needed boost other owner too.
			xrt_mutex_t* m_dest = m->mutex_owner->mutex_blocked_on;
			start_boost_chain(m_dest, t);
		}
	}
}

static void xrt_mutex_set_owner(xrt_mutex_t* mutex, TCB_t* thread){
	mutex -> mutex_owner = thread;
	cdll_insert_node_to_tail(&thread->mutex_held, &mutex->m_node);
}

static void xrt_mutex_clear_owner(xrt_mutex_t* mutex){
	cdll_remove_known_node_from_list(&mutex->mutex_owner->mutex_held, &mutex->m_node);
	mutex -> mutex_owner = NULL;
}
