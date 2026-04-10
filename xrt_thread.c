/*
 * xrt_thread.c
 *
 *  Created on: Apr 7, 2026
 *      Author: ozan.akyol
 */

#include <stdlib.h>
#include <stdbool.h>

#include "xrt_thread.h"

__attribute__((naked)) static void xrt_init_stack_frame(TCB_t* node);


bool xrt_thread_list_init(TCB_List_t* list){
	if(list == NULL){
		return false;
	}

	if(cdll_init_list(list)){
		return true;
	}

	return true;
}

extern TCB_t tcb2;
bool xrt_thread_init(TCB_List_t* list ,TCB_t* node, cdll_node* thread_node){
	if(list == NULL || node == NULL){
		return false;
	}

	//TODO: start threads from ISR
	if(node == &tcb2){

		xrt_init_stack_frame(node);
	}

	return (cdll_insert_node_to_list(list, thread_node) == true);
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

	__asm("str r6, [r7, #0]"); // now I guess, we have to update thread's stack pointer value(It should points the software stack frame).

	__asm("pop {r7, lr}");
	__asm("bx lr");
}
