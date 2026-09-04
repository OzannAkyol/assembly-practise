/*
 * xrt_thread.c
 *
 *  Created on: Apr 7, 2026
 *      Author: ozan.akyol
 */

#include <stdlib.h>
#include <stdbool.h>

#include "xrt_thread.h"
#include "xrt_system_calls.h"
#include "xrt_kernel_internal.h"
#include "xrt_port.h"

#define XRT_THREAD_MIN_STACK_WORDS      (32u)   /* HW frame(8) + SW frame(8) + nested exception margin */
#define XRT_STACK_ALIGNMENT_IN_BYTE     (8u)    /* AAPCS / exception frame requirement */

static ThreadId_t next_thread_id = THREAD_ID_IDLE;

static void xrt_thread_stack_frame_init(TCB_t* node);

bool xrt_thread_create(TCB_t*     tcb,
                       cdll_node* tcb_node,
                       void       (*exec_function)(void),
                       Priority_t priority,
                       uint32_t*  stack_base,
                       uint32_t   stack_size_in_word)
{
    if(tcb == NULL || tcb_node == NULL || exec_function == NULL || stack_base == NULL){
        return false;
    }
    if(priority >= MAX_NUM_OF_PRIORITY){
        return false;
    }
    if(stack_size_in_word < XRT_THREAD_MIN_STACK_WORDS){
        return false;
    }
    if(next_thread_id >= MAX_NUM_OF_THREAD){
        return false;
    }

    uint32_t* stack_top = stack_base + stack_size_in_word;

    if(((uintptr_t)stack_top % XRT_STACK_ALIGNMENT_IN_BYTE) != 0u){
        return false;
    }

    tcb -> thread_id              = next_thread_id;
    tcb -> fptr                   = exec_function;
    tcb -> base_priority          = priority;
    tcb -> current_priority       = priority;
    tcb -> state                  = THREAD_READY_STATE;
    tcb -> blocked_reason         = XRT_BLOCK_NONE;
    tcb -> wake_tick              = 0u;
    tcb -> thread_base_ptr        = stack_base;
    tcb -> thread_stack_size_word = stack_size_in_word;
    tcb -> thread_sp              = stack_top;
    tcb -> thread_node            = tcb_node;
    tcb -> currently_located_list = &xrtKernelReadyList;

    tcb_node -> data = tcb;
    tcb_node -> next = tcb_node;
    tcb_node -> prev = tcb_node;

    xrt_thread_stack_frame_init(tcb);

    if(!cdll_push_data_with_priority_order(&xrtKernelReadyList, tcb_node)){
        tcb -> currently_located_list = NULL;
        return false;
    }

    next_thread_id++;

    return true;
}

void xrt_thread_yield(void){
	XRT_SYSCALL_NO_ARG(SYS_CALL_THREAD_YIELD);
}

void xrt_thread_delay(uint32_t ms){
	XRT_SYSCALL_WITH_ARG(SYS_CALL_OS_DELAY, ms);
}

void xrt_thread_delay_from_svc(uint32_t arg, TCB_t* running_thread){
	running_thread -> wake_tick = arg;
	running_thread -> state = THREAD_BLOCKED_STATE;
	running_thread -> blocked_reason = XRT_THREAD_OS_DELAY;
	running_thread -> currently_located_list = &xrtKernelStoppedList;

	xrt_port_request_context_switch();
}


static void xrt_thread_stack_frame_init(TCB_t* node){
    uint32_t* sp = node->thread_sp;

    // HW Stack Frame
    *(--sp) = 0x01000000;            // xPSR
    *(--sp) = (uint32_t)node->fptr;  // PC (Thread running function)
    *(--sp) = 0xFFFFFFFD;            // LR (EXC_RETURN value, FPU not used for now)
    *(--sp) = 0;                     // R12
    *(--sp) = 0;                     // R3
    *(--sp) = 0;                     // R2
    *(--sp) = 0;                     // R1
    *(--sp) = 0;                     // R0

    // SW Stack Frame
    *(--sp) = 0;                     // R4
    *(--sp) = 0;                     // R5
    *(--sp) = 0;                     // R6
    *(--sp) = 0;                     // R7
    *(--sp) = 0;                     // R8
    *(--sp) = 0;                     // R9
    *(--sp) = 0;                     // R10
    *(--sp) = 0;                     // R11

    node -> thread_sp = sp;          // tcb stack pointer must show the sw sp base.
}
