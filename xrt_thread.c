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

void xrt_thread_start(){
    __asm("SVC #6");  // os start
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

bool xrt_thread_init(TCB_List_t* list ,TCB_t* node, cdll_node* thread_node){
    if(list == NULL || node == NULL){
        return false;
    }
    xrt_thread_stack_init(node);

    cdll_push_data_with_priority_order(list, thread_node);
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
