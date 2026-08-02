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

__attribute__((naked)) static void xrt_init_stack_frame(TCB_t* node){
	__asm("push {r7, lr}");

	__asm("add r7, r0, #0");
	__asm("add r7 , r7 , #4"); 	// get related thread's stack pointer.

	__asm("ldr r6, [r7, #0]"); // now r5 points the start address of thread stack frame.

	__asm("ldr r1, =0x01000000"); // assign xPSR value.
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("ldr r1, [r7, #16]"); // take the thread's function.
	__asm("str r1, [r6, #0]");	// assign it as a PC value.
	__asm("sub r6, #4");

	__asm("ldr r1, =0xfffffffd"); // LR value, for now, none of them use floating-point unit
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r12
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r0
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r1
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r2
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r3
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r4
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r5
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r6
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r7
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r8
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r9
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r10
	__asm("str r1, [r6, #0]");
	__asm("sub r6, #4");

	__asm("mov r1, 0");			// r11
	__asm("str r1, [r6, #0]");

	__asm("add r6, 32"); //it point to thread's hw stack frame
	__asm("str r6, [r7, #0]"); // now I guess, we have to update thread's stack pointer value(It should points the software stack frame).

	__asm("pop {r7, lr}");
	__asm("bx lr");
}
