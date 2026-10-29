/*
 * xrt_port_cm4.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include "xrt_port_cm4.h"
#include "xrt_system_calls.h"

#define STACKED_R0_OFFSET               (0u)
#define STACKED_PC_OFFSET               (6u)
#define THUMB_INSTRUCTION_SIZE_IN_BYTE  (2u)

__attribute__((naked))void xrt_thread_store_context(void){
	__asm("mrs r0,  PSP;");
	__asm("sub r0, r0, #32");

	__asm("ldr r1, =xrt_kernel_running_list");
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
	__asm("ldr r0, =xrt_kernel_running_list");
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

/*
 *@note It must be naked since the inside handler.
 *		GCC push the r7, it assigns sp to r7, and sp shows the handler's stack frame.
 * 		then you try to assign the r7 to related thread's stack, the program won't work
 * 		properly.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm("push {r0, lr}");

    __asm volatile("mov r0, %[basepri_val]\n"
                   "msr basepri, r0"
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

    __asm("add sp, sp, #8");
    __asm("mov r0, #0");
	__asm("msr basepri, r0");
	__asm("ldr lr, =0xFFFFFFFD");
    __asm("bx lr");

    /* ──────── else branch ──────── */
    __asm("else_branch:");

    __asm("bl xrt_thread_store_context");
    __asm("bl xrt_kernel_schedule");
    __asm("bl xrt_load_context");

    __asm("PendSV_Handler_exit:");

    __asm("mov r0, #0");
	__asm("msr basepri, r0");

    __asm("pop {r0, lr}");
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
