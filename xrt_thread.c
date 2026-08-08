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

#define reserved_start_stack_size		64
uint32_t reserved_start_stack[reserved_start_stack_size];

void tcb_reserved_function();

TCB_t tcb_reserved = {
        .fptr = tcb_reserved_function,
        .ThreadStackSize = reserved_start_stack_size,
        .base_priority = HIGH_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_RESERVED,
        .thread_base_ptr = reserved_start_stack,
        .thread_sp = reserved_start_stack + reserved_start_stack_size - 1
};

__attribute__((naked)) static void xrt_init_stack_frame(TCB_t* node);


volatile bool is_os_kernel_started = false;
volatile bool is_os_first_cs_occurs = false;

void tcb_reserved_function(){
	__disable_irq();
	is_os_kernel_started = true;
	is_os_first_cs_occurs = true;
	__enable_irq();
	while(1){

	}
}

uint32_t x;

void xrt_create_svcall(){
	xrt_init_stack_frame(&tcb_reserved);
	__set_CONTROL(0x2);
	x = __get_CONTROL();
	(void)x;

	uint32_t value = (uint32_t)tcb_reserved.thread_sp;
	__set_PSP(value);

	tcb_reserved.fptr();

}
void xrt_set_os_priority_order(void){
	NVIC_SetPriority(SVCall_IRQn, 15);
	NVIC_SetPriority(PendSV_IRQn, 15);
	NVIC_SetPriority(SysTick_IRQn, 14);
}
void xrt_thread_start(){
	xrt_create_svcall();
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

	xrt_init_stack_frame(node);
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

    node -> thread_sp = sp;          // tcb stack pointer must show the hardware sp base(R0).

    // SW Stack Frame
    *(--sp) = 0;                     // R4
    *(--sp) = 0;                     // R5
    *(--sp) = 0;                     // R6
    *(--sp) = 0;                     // R7
    *(--sp) = 0;                     // R8
    *(--sp) = 0;                     // R9
    *(--sp) = 0;                     // R10
    *(--sp) = 0;                     // R11
}
