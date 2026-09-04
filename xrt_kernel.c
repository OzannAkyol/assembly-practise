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

#define IDLE_THREAD_STACK_SIZE		(64u)
__attribute__((aligned(8))) static uint32_t idle_stack[IDLE_THREAD_STACK_SIZE];

volatile bool is_os_kernel_started = false;
volatile bool is_os_first_cs_occurs = false;

static cdll_node idle_node;
static TCB_t idle_tcb;

TCB_List_t xrtKernelReadyList={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrtKernelRunningList ={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrtKernelStoppedList ={
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
	if(!xrt_kernel_list_init(&xrtKernelRunningList)){
		return false;
	}
	if(!xrt_kernel_list_init(&xrtKernelStoppedList)){
		return false;
	}
	if(!xrt_kernel_list_init(&xrtKernelReadyList)){
		return false;
	}
	if(!xrt_thread_create(&idle_tcb, &idle_node, idle_exec_function, IDLE_PRIORITY, idle_stack, IDLE_THREAD_STACK_SIZE)){
		return false;
	}
	xrt_port_set_os_priority_order();

	return true;
}

bool xrt_kernel_start(void){
	cdll_node* node = cdll_get_list_head(&xrtKernelReadyList);
	if(node != NULL && !is_os_kernel_started){
		TCB_t* ready_thread = (TCB_t*)node-> data;

		cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_thread -> thread_node);
		cdll_push_data_with_priority_order(&xrtKernelRunningList, ready_thread -> thread_node);
		ready_thread -> currently_located_list = &xrtKernelRunningList;
		ready_thread -> state = THREAD_RUNNING_STATE;

		is_os_first_cs_occurs = true;
		is_os_kernel_started = true;

		xrt_port_request_context_switch();

		return true;
	}

	return false;
}

void xrt_kernel_schedule(void){
	if(xrtKernelReadyList.head == NULL){
		//idle thread already loaded, and has lowest priority
		return;
	}

	TCB_t* running_thread = (TCB_t*)xrtKernelRunningList.head -> data;
	TCB_t* ready_thread   = (TCB_t*)xrtKernelReadyList.head -> data;

	/* A blocked thread must leave the running list regardless of its priority.
	 * Priority is only a tie-breaker for a thread that is still runnable. */
	if(running_thread -> state != THREAD_BLOCKED_STATE &&
	   running_thread -> current_priority > ready_thread -> current_priority){
		return;
	}

	cdll_remove_known_node_from_list(&xrtKernelRunningList, running_thread -> thread_node);

	if(running_thread -> state == THREAD_BLOCKED_STATE){
		//state, blocked reason and currently_located_list are set by the SVC handler
		cdll_push_data_with_priority_order(running_thread -> currently_located_list, running_thread -> thread_node);
	}
	else{
		running_thread -> state = THREAD_READY_STATE;
		cdll_push_data_with_priority_order(&xrtKernelReadyList, running_thread -> thread_node);
		running_thread -> currently_located_list = &xrtKernelReadyList;
	}

	cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_thread -> thread_node);
	ready_thread -> state = THREAD_RUNNING_STATE;
	cdll_insert_node_to_head(&xrtKernelRunningList, ready_thread -> thread_node);
	ready_thread -> currently_located_list = &xrtKernelRunningList;
}


static void idle_exec_function(void){
	while(1){
		__NOP();
	}
}
