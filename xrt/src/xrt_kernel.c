/*
 * xrt_kernel.c
 *
 *  Created on: Sep 1, 2026
 *      Author: ozi
 */
#include <stddef.h>

#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
#include "xrt_list.h"
#include "xrt_thread.h"

#include "xrt_mutex.h"
#include "xrt_semaphore.h"

#define IDLE_THREAD_STACK_SIZE		(64u)
__attribute__((aligned(8))) static uint32_t idle_stack[IDLE_THREAD_STACK_SIZE];

static volatile bool is_os_kernel_started = false;
volatile bool is_os_first_cs_occurs = false;

static cdll_node idle_node;
static TCB_t idle_tcb;

TCB_List_t xrt_kernel_ready_list={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrt_kernel_running_list ={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrt_kernel_stopped_list ={
		.head = NULL,
		.size = 0,
};

static void idle_exec_function(void);
static bool xrt_kernel_list_init(cdll_list* list);

static bool xrt_kernel_list_init(cdll_list* list){
    if(!cdll_init_list(list)){
        return false;
    }

    return true;
}

bool xrt_kernel_init(void){
	if(!xrt_kernel_list_init(&xrt_kernel_running_list)){
		return false;
	}
	if(!xrt_kernel_list_init(&xrt_kernel_stopped_list)){
		return false;
	}
	if(!xrt_kernel_list_init(&xrt_kernel_ready_list)){
		return false;
	}
	if(!xrt_thread_create(&idle_tcb, &idle_node, idle_exec_function, IDLE_PRIORITY, idle_stack, IDLE_THREAD_STACK_SIZE)){
		return false;
	}
	xrt_port_set_os_priority_order();

	return true;
}

bool xrt_kernel_start(void){
	cdll_node* node = cdll_get_list_head(&xrt_kernel_ready_list);
	if(node != NULL && !is_os_kernel_started){
		TCB_t* ready_thread = (TCB_t*)node-> data;

		xrt_kernel_move_thread(ready_thread, &xrt_kernel_running_list);
		ready_thread -> state = THREAD_RUNNING_STATE;

		is_os_first_cs_occurs = true;
		is_os_kernel_started = true;

		xrt_port_request_context_switch();

		return true;
	}

	return false;
}

void xrt_kernel_schedule(void){
	if(xrt_kernel_ready_list.head == NULL){
		//idle thread already loaded, and has lowest priority
		return;
	}

	TCB_t* running_thread = (TCB_t*)xrt_kernel_running_list.head -> data;
	TCB_t* ready_thread   = (TCB_t*)xrt_kernel_ready_list.head -> data;

	/* A blocked thread must leave the running list regardless of its priority.
	 * Priority is only a tie-breaker for a thread that is still runnable. */
	if(running_thread -> state != THREAD_BLOCKED_STATE &&
	   running_thread -> current_priority > ready_thread -> current_priority){
		return;
	}

	cdll_remove_known_node_from_list(&xrt_kernel_running_list, running_thread -> thread_node);

	if(running_thread -> state == THREAD_BLOCKED_STATE){
		//state, blocked reason and currently_located_list are set by the SVC handler
		xrt_kernel_push_thread_by_priority(running_thread -> currently_located_list, running_thread);
	}
	else{
		running_thread -> state = THREAD_READY_STATE;
		xrt_kernel_push_thread_by_priority(&xrt_kernel_ready_list, running_thread);
		running_thread -> currently_located_list = &xrt_kernel_ready_list;
	}

	ready_thread -> state = THREAD_RUNNING_STATE;
	xrt_kernel_move_thread(ready_thread, &xrt_kernel_running_list);
}

void xrt_kernel_check_sleeping_threads(void){
	uint32_t old_prio = xrt_kernel_enter_critical();

	if(is_os_kernel_started){
		if(xrt_kernel_stopped_list.head != NULL){
		  cdll_node* current = xrt_kernel_stopped_list.head;
		  uint32_t count = xrt_kernel_stopped_list.size;

		  while(count-- > 0){
			  cdll_node* next_node = current->next;
			  TCB_t* blocked_thread = (TCB_t*)current->data;

			  if(blocked_thread->blocked_reason == XRT_THREAD_OS_DELAY){
				  blocked_thread->wake_tick--;
				  if(blocked_thread->wake_tick == 0){
					  blocked_thread->blocked_reason = XRT_BLOCK_NONE;
					  blocked_thread->state = THREAD_READY_STATE;

					  xrt_kernel_move_thread(blocked_thread, &xrt_kernel_ready_list);
				  }
			  }
			  current = next_node;
		  }
	   }

		xrt_port_request_context_switch();
	}
	 xrt_kernel_exit_critical(old_prio);
}

void xrt_kernel_reorder_thread(TCB_t* thread){
	xrt_kernel_move_thread(thread, thread->currently_located_list);
}

void xrt_kernel_move_thread(TCB_t* thread, cdll_list* dst){
	if(dst == NULL || thread == NULL){
		return;
	}

	cdll_remove_known_node_from_list(thread -> currently_located_list, thread -> thread_node);
	xrt_kernel_push_thread_by_priority(dst, thread);
	thread -> currently_located_list = dst;
}

void xrt_kernel_wake_thread(TCB_t* thread){
    if(thread == NULL){
        return;
    }

    thread -> state = THREAD_READY_STATE;
    thread -> blocked_reason = XRT_BLOCK_NONE;
    xrt_kernel_push_thread_by_priority(&xrt_kernel_ready_list, thread);
    thread -> currently_located_list = &xrt_kernel_ready_list;

    TCB_t* running_thread = (TCB_t*)xrt_kernel_running_list.head->data;

    if(thread -> current_priority > running_thread -> current_priority){
        xrt_port_request_context_switch();
    }
}

void xrt_kernel_block_thread(TCB_t* thread, cdll_list* wait_list, ThreadBlockedReason_t reason){
    if(thread == NULL || wait_list == NULL){
        return;
    }

    thread -> state = THREAD_BLOCKED_STATE;
    thread -> blocked_reason = reason;
    thread -> currently_located_list = wait_list;

    xrt_port_request_context_switch();
}

bool xrt_kernel_push_thread_by_priority(cdll_list* list, TCB_t* thread){
    if(list == NULL || thread == NULL){
        return false;
    }

    cdll_node* current = cdll_get_list_head(list);

    if(current == NULL){
        return cdll_insert_node_to_tail(list, thread->thread_node);
    }

    uint8_t remaining = list->size;

    while(remaining-- > 0){
        TCB_t* existing = (TCB_t*)current->data;

        if(existing->current_priority < thread->current_priority){
            return cdll_insert_node_before(list, current, thread->thread_node);
        }

        current = current->next;
    }

    return cdll_insert_node_to_tail(list, thread->thread_node);
}

static void idle_exec_function(void){
	while(1){
		__NOP();
	}
}
