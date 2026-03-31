extern TCB_t tcb2;
extern TCB_t tcb1;
__attribute__((naked))void xrt_store_context2(void);
__attribute__((naked))void xrt_store_context2(void){
	__asm("mrs r0,  PSP;"); //we already know we have tcb1 stack pointer.
	__asm("sub r0, r0, #32;");

	__asm("ldr r1, =tcb2");
	__asm("add r1, r1, #4");

	__asm("str r0, [r1, #0]");

	__asm("str r4,  [r0, #28];");
	__asm("str r5,  [r0, #24];");
	__asm("str r6,  [r0, #20];");
	//TODO: I guess that this line is problematic.
	__asm("str r7,  [r0, #16];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r0, #12];");
	__asm("str r9,  [r0, #8];");
	__asm("str r10, [r0, #4];");
	__asm("str r11, [r0, #0];");

	__asm("bx lr;");

}

__attribute__((naked))void xrt_store_context1(void);
__attribute__((naked))void xrt_store_context1(void){
	__asm("mrs r0,  PSP;"); //we already know we have tcb1 stack pointer.
	__asm("sub r0, r0, #32;");

	__asm("ldr r1, =tcb1");
	__asm("add r1, r1, #4");

	__asm("str r0, [r1, #0]");

	__asm("str r4,  [r0, #28];");
	__asm("str r5,  [r0, #24];");
	__asm("str r6,  [r0, #20];");
	//TODO: I guess that this line is problematic. // I fix this using naked pendSV handler
	__asm("str r7,  [r0, #16];");// store related thread's frame pointer(stack pointer).
	__asm("str r8,  [r0, #12];");
	__asm("str r9,  [r0, #8];");
	__asm("str r10, [r0, #4];");
	__asm("str r11, [r0, #0];");

	__asm("bx lr;");

}

bool flag = true;

/**
  * @brief This function handles Pendable request for system service.
  */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm("LDR     r3, =flag");
    __asm("LDRB    r2, [r3]");
    __asm("CMP     r2, #0");
    __asm("BEQ     else_branch");

    /* ──────── if(flag == true) ──────── */
    __asm("MOV     r2, #0");
    __asm("STRB    r2, [r3]");

    __asm("PUSH    {lr}");
    __asm("BL      xrt_store_context1");
    __asm("POP     {lr}");

    __asm("LDR     r0, =tcb2");
    __asm("ADD     r0, r0, #4");
    __asm("LDR     r1, [r0, #0]");

    __asm("LDR     r4,  [r1, #28]");
    __asm("LDR     r5,  [r1, #24]");
    __asm("LDR     r6,  [r1, #20]");
    __asm("LDR     r7,  [r1, #16]");
    __asm("LDR     r8,  [r1, #12]");
    __asm("LDR     r9,  [r1, #8]");
    __asm("LDR     r10, [r1, #4]");
    __asm("LDR     r11, [r1, #0]");

    __asm("ADD     r1, r1, #32");
    __asm("MSR     PSP, r1");

    __asm("B       pendsv_exit");

    /* ── else (flag == false) ── */
    __asm("else_branch:");
    __asm("MOV     r2, #1");
    __asm("STRB    r2, [r3]");

    __asm("PUSH    {lr}");
    __asm("BL      xrt_store_context2");
    __asm("POP     {lr}");

    __asm("LDR     r0, =tcb1");
    __asm("ADD     r0, r0, #4");
    __asm("LDR     r1, [r0, #0]");

    __asm("LDR     r4,  [r1, #28]");
    __asm("LDR     r5,  [r1, #24]");
    __asm("LDR     r6,  [r1, #20]");
    __asm("LDR     r7,  [r1, #16]");
    __asm("LDR     r8,  [r1, #12]");
    __asm("LDR     r9,  [r1, #8]");
    __asm("LDR     r10, [r1, #4]");
    __asm("LDR     r11, [r1, #0]");

    __asm("ADD     r1, r1, #32");
    __asm("MSR     PSP, r1");

    /* ── exit ── */
    __asm("pendsv_exit:");
    __asm("BX      lr");
}