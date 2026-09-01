/*
 * xrt_thread.c
 *
 *  Created on: Apr 7, 2026
 *      Author: ozan.akyol
 */

#include <stdlib.h>
#include <stdbool.h>

#include "xrt_thread.h"
#include "main.h"


static void xrt_thread_stack_init(TCB_t* node);

volatile bool is_os_kernel_started = false;
volatile bool is_os_first_cs_occurs = false;

#define KERNEL_BASEPRIO_THRESHOLD		(5u)
#define KERNEL_BASEPRI  (KERNEL_BASEPRIO_THRESHOLD << (8U - __NVIC_PRIO_BITS))   // 5 << 4 = 0x50

extern TCB_List_t xrtKernelReadyList;
extern TCB_List_t xrtKernelRunningList;

uint32_t xrt_enter_critical_section(void){
    uint32_t old = __get_BASEPRI();
    __set_BASEPRI(KERNEL_BASEPRI);
    return old;
}

void xrt_exit_critical_section(uint32_t old){
    __set_BASEPRI(old);
}

void xrt_thread_start(void){
	TCB_t* ready_thread = (TCB_t*)cdll_get_list_head(&xrtKernelReadyList) -> data;

	cdll_remove_known_node_from_list(&xrtKernelReadyList, ready_thread -> thread_node);

	cdll_push_data_with_priority_order(&xrtKernelRunningList, ready_thread -> thread_node);
	ready_thread -> currently_located_list = &xrtKernelRunningList;
	ready_thread -> state = THREAD_RUNNING_STATE;

	is_os_first_cs_occurs = true;
	is_os_kernel_started = true;

	SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

void xrt_set_os_priority_order(void){
    NVIC_SetPriority(SVCall_IRQn, 15);
    NVIC_SetPriority(PendSV_IRQn, 15);
    NVIC_SetPriority(SysTick_IRQn, 14);
}

bool xrt_thread_list_init(TCB_List_t* list){
    if(list == NULL){
        return false;
    }

    if(cdll_init_list(list)){
        return true;
    }

    return true;
}

bool xrt_thread_init(TCB_List_t* list ,TCB_t* node){
    if(list == NULL || node == NULL){
        return false;
    }
    xrt_thread_stack_init(node);

    node -> thread_node -> data = node;
    cdll_push_data_with_priority_order(list, node -> thread_node);
    node -> currently_located_list = list;
    return true;
}

void xrt_thread_yield(void){
    __asm("SVC #13");  // yield
}

void xrt_thread_delay(uint32_t ms){
    register uint32_t r0 __asm("r0") = ms;
    __asm volatile("SVC #12" : : "r"(r0));
}

static void xrt_thread_stack_init(TCB_t* node){
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
