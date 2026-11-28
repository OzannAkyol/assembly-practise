/*
 * xrt_port_armv7m.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include <stddef.h>
#include "xrt_port_armv7m.h"
#include "xrt_system_calls.h"
#include "xrt_thread.h"

#define STACKED_R0_OFFSET               (0u)
#define STACKED_PC_OFFSET               (6u)
#define THUMB_INSTRUCTION_SIZE_IN_BYTE  (2u)

_Static_assert(offsetof(TCB_t, thread_sp) == 4u,
               "xrt_port_armv7m.c reads thread_sp at TCB offset 4; the field moved");
_Static_assert(offsetof(TCB_t, EXC_RET_VAL) == 48u,
               "xrt_port_armv7m.c reads exc_return at TCB offset 48; the field moved");

__attribute__((naked))void xrt_thread_store_context(uint32_t arg){
	__asm("mrs r3,  PSP;");

	__asm("ldr r1, =xrt_kernel_running_list");
	__asm("ldr r1, [r1, #0]"); // r1 points the head node
	__asm("ldr r1, [r1, #0]"); // r1 points the head node->data

	__asm("mov r2, r1");	 // r2 points the head node->data
	__asm("add r2, #48");	 // r2 points exc_val area,
	__asm("str r0, [r2, #0]");     // I guess I write the value to exact place.

	__asm("add r1, r1, #4");   // r1 points the head node->data + 4 = related threads stack pointer address pointer

	//determine the extended frame or not.
	__asm("UBFX	r0, r0, #4, #1");
	__asm("CBNZ r0, bit_is_set");

	__asm("sub r3, r3, #96");
	__asm("str r3, [r1, #0]");	// store the currently using stack pointer to tcb->sp(sw stack frame).
	__asm("str r4,  [r3, #92];");
	__asm("str r5,  [r3, #88];");
	__asm("str r6,  [r3, #84];");
	__asm("str r7,  [r3, #80];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r3, #76];");
	__asm("str r9,  [r3, #72];");
	__asm("str r10, [r3, #68];");
	__asm("str r11, [r3, #64];");
	__asm("vstr s16, [r3, #60];");
	__asm("vstr s17, [r3, #56];");
	__asm("vstr s18, [r3, #52];");
	__asm("vstr s19, [r3, #48];");
	__asm("vstr s20, [r3, #44];");
	__asm("vstr s21, [r3, #40];");
	__asm("vstr s22, [r3, #36];");
	__asm("vstr s23, [r3, #32];");
	__asm("vstr s24, [r3, #28];");
	__asm("vstr s25, [r3, #24];");
	__asm("vstr s26, [r3, #20];");
	__asm("vstr s27, [r3, #16];");
	__asm("vstr s28, [r3, #12];");
	__asm("vstr s29, [r3, #8];");
	__asm("vstr s30, [r3, #4];");
	__asm("vstr s31, [r3, #0];");
	__asm("bx lr;");

	__asm("bit_is_set:");
	__asm("sub r3, r3, #32");
	__asm("str r3, [r1, #0]");	// store the currently using stack pointer to tcb->sp(sw stack frame).
	__asm("str r4,  [r3, #28];");
	__asm("str r5,  [r3, #24];");
	__asm("str r6,  [r3, #20];");
	__asm("str r7,  [r3, #16];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r3, #12];");
	__asm("str r9,  [r3, #8];");
	__asm("str r10, [r3, #4];");
	__asm("str r11, [r3, #0];");

	__asm("bx lr;");
}

__attribute__((naked))uint32_t xrt_load_context(void){
	__asm("ldr r2, =xrt_kernel_running_list");
	__asm("ldr r2, [r2, #0]"); // load the address value of head node's
	__asm("ldr r2, [r2, #0]");  // take the node's tcb address.

	__asm("add r2, #4");		// move the thread's stack pointer area.

	__asm("mov r0, r2");				//get value of sp area.
	__asm("add r0, #44");	 			// r0 points exc_val area,
	__asm("ldr r0, [r0, #0]");			// load execption return value.
	//determine the extended frame or not.
	__asm("UBFX	r1, r0, #4, #1");
	__asm("CBNZ r1, ext_frame_is_not_set");
	__asm("ldr r2, [r2, #0]");	// take the thread's stack pointer value.
	__asm("ldr r4,  [r2, #92];");
	__asm("ldr r5,  [r2, #88];");
	__asm("ldr r6,  [r2, #84];");
	__asm("ldr r7,  [r2, #80];");// store related thread's frame pointer(stack pointer).
	__asm("ldr r8,  [r2, #76];");
	__asm("ldr r9,  [r2, #72];");
	__asm("ldr r10, [r2, #68];");
	__asm("ldr r11, [r2, #64];");
	__asm("vldr s16, [r2, #60];");
	__asm("vldr s17, [r2, #56];");
	__asm("vldr s18, [r2, #52];");
	__asm("vldr s19, [r2, #48];");
	__asm("vldr s20, [r2, #44];");
	__asm("vldr s21, [r2, #40];");
	__asm("vldr s22, [r2, #36];");
	__asm("vldr s23, [r2, #32];");
	__asm("vldr s24, [r2, #28];");
	__asm("vldr s25, [r2, #24];");
	__asm("vldr s26, [r2, #20];");
	__asm("vldr s27, [r2, #16];");
	__asm("vldr s28, [r2, #12];");
	__asm("vldr s29, [r2, #8];");
	__asm("vldr s30, [r2, #4];");
	__asm("vldr s31, [r2, #0];");
	__asm("add r2, r2, #96");
	__asm("msr PSP, r2");	//set psp to thread's hardware stack pointer.
	__asm("bx lr");


	__asm("ext_frame_is_not_set:");
	__asm("ldr r2, [r2, #0]");	// take the thread's stack pointer value.
	__asm("ldr r4, [r2, #28]");
	__asm("ldr r5, [r2, #24]");
	__asm("ldr r6, [r2, #20]");
	__asm("ldr r7, [r2, #16]");
	__asm("ldr r8, [r2, #12]");
	__asm("ldr r9, [r2, #8]");
	__asm("ldr r10,[r2, #4]");
	__asm("ldr r11, [r2, #0];");
	__asm("add r2, r2, #32");
	__asm("msr PSP, r2");	//set psp to thread's hardware stack pointer.
	__asm("bx lr");
}

/*
 *@note It must be naked since the inside handler.
 *		GCC push the r7, it assigns sp to r7, and sp shows the handler's stack frame.
 * 		then you try to assign the r7 to related thread's stack, the program won't work
 * 		properly.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm("push {r1, lr}");

    __asm volatile("mov r1, %[basepri_val]\n"
                   "msr basepri, r1"
                   :
                   : [basepri_val] "I" (KERNEL_BASEPRI));


    __asm("LDR     r3, =is_os_first_cs_occurs");
    __asm("LDRB    r2, [r3]");
    __asm("CMP     r2, #0");
    __asm("BEQ     else_branch");

    /* ──────── if(flag == true) ──────── */
    __asm("MOV     r2, #0");
    __asm("STRB    r2, [r3]");
    __asm("bl xrt_load_context");

    __asm("pop {r1, lr}");
    __asm("mov r1, #0");
	__asm("msr basepri, r1");
	__asm("mov lr, r0");
    __asm("bx lr");

    /* ──────── else branch ──────── */
    __asm("else_branch:");
    __asm("mov r0, lr");
    __asm("bl xrt_thread_store_context");
    __asm("bl xrt_kernel_schedule");
    __asm("bl xrt_load_context");
    __asm("mov r2, r0");

    __asm("mov r1, #0");
	__asm("msr basepri, r1");

    __asm("pop {r1, lr}");

    __asm("mov lr, r2");
	__asm("bx lr");
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
	uint32_t* thread_sp = (uint32_t*)__get_PSP();
	uint32_t svc_arg    = *thread_sp;

	thread_sp += STACKED_PC_OFFSET;

	uint8_t* svc_instruction = (uint8_t*)*thread_sp;
	svc_instruction -= THUMB_INSTRUCTION_SIZE_IN_BYTE;

	uint8_t svc_number = *svc_instruction;

	xrt_system_calls_dispatch(svc_number, svc_arg);
}

void xrt_port_set_os_priority_order(void){
    NVIC_SetPriority(SVCall_IRQn, XRT_PORT_SVCALL_PRIORITY);
    NVIC_SetPriority(PendSV_IRQn, XRT_PORT_PENDSV_PRIORITY);
    NVIC_SetPriority(SysTick_IRQn, XRT_PORT_SYSTICK_PRIORITY);
}
