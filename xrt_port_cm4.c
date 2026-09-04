/*
 * xrt_port_cm4.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include "xrt_port_cm4.h"

__attribute__((naked))void xrt_thread_store_context(void){
	__asm("mrs r0,  PSP;");
	__asm("sub r0, r0, #32");

	__asm("ldr r1, =xrtKernelRunningList");
	__asm("ldr r1, [r1, #0]"); // r1 points the head node
	__asm("ldr r1, [r1, #0]"); // r1 points the head node->data
	__asm("add r1, r1, #4");   // r1 points the head node->data + 4 = related threads stack pointer address pointer

	__asm("str r0, [r1, #0]");	// store the currently using stack pointer to tcb->sp(sw stack frame).

	__asm("str r4,  [r0, #28];");
	__asm("str r5,  [r0, #24];");
	__asm("str r6,  [r0, #20];");
	__asm("str r7,  [r0, #16];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r0, #12];");
	__asm("str r9,  [r0, #8];");
	__asm("str r10, [r0, #4];");
	__asm("str r11, [r0, #0];");

	__asm("bx lr;");
}

__attribute__((naked))void xrt_load_context(void){
	__asm("ldr r0, =xrtKernelRunningList");
	__asm("ldr r0, [r0, #0]"); // load the address value of head node's
	__asm("ldr r0, [r0, #0]");  // take the node's tcb address.

	__asm("add r0, #4");		// move the thread's stack pointer area.
	__asm("ldr r0, [r0, #0]");	// take the thread's stack pointer value.

	__asm("ldr r4, [r0, #28]");
	__asm("ldr r5, [r0, #24]");
	__asm("ldr r6, [r0, #20]");
	__asm("ldr r7, [r0, #16]");
	__asm("ldr r8, [r0, #12]");
	__asm("ldr r9, [r0, #8]");
	__asm("ldr r10,[r0, #4]");
	__asm("ldr r11, [r0, #0];");

	__asm("add r0, r0, #32");
	__asm("msr PSP, r0");	//set psp to thread's hardware stack pointer.

	__asm("bx lr");
}

void xrt_port_set_os_priority_order(void){
    NVIC_SetPriority(SVCall_IRQn, 15);
    NVIC_SetPriority(PendSV_IRQn, 15);
    NVIC_SetPriority(SysTick_IRQn, 14);
}
