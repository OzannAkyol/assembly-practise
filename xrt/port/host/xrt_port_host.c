/*
 * xrt_port_host.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include "xrt_port_host.h"

uint32_t port_set_os_priority_order = 0;
uint32_t port_request_context_switch = 0;


void xrt_port_set_os_priority_order(void){
    port_set_os_priority_order++;
}

uint32_t xrt_port_enter_critical(void){
    uint32_t old = 0;
    return old;
}

void xrt_port_exit_critical(uint32_t old){
    return;
}

void xrt_port_request_context_switch(void){
    port_request_context_switch++;
}

void xrt_port_init_stack_frame(TCB_t* node){
    uint32_t* sp = node->thread_sp;

    // HW Stack Frame
    *(--sp) = 0x01000000;            // xPSR
    *(--sp) = (uint32_t)node->fptr;  // PC (Thread running function)
	*(--sp) = (uint32_t)node->fptr;  // LR
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