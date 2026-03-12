
__attribute__((naked))int bar(int a, int b);
__attribute__((naked)) int baz(int x, int y);

/*
 * @note  we're responsible to write function prologue/epilogue.
 *
 */
__attribute__((naked))int bar(int a, int b){
	__asm__("push {r7 , lr};"
			"sub sp, #12;"		//allocate stack area for function stack
			"add r7, sp, #0;");

	__asm__("add r0, r0, r1;"
			"sub r0, r0, #4;");

	__asm__("str r0, [r7, #8]"); //save r0 context to stack. No need to save r1

	__asm__("bl baz");

	__asm__("str r0, [r7, #4]"); //save baz function result to caller stack.

	__asm__("ldr r3, [r7, #8];"
			"add r0, r0, r3;"); // load old computed r0 register value to another register.

	__asm__("adds sp, #12;" //clarify the prologue sequences. I write this line since to take back stack pointer to old location.
			"pop {lr, r7};");

	__asm__("bx lr"); // return
}

__attribute__((naked)) int baz(int x, int y){
	__asm__("push {r7};" //No need to push frame pointer. Since it is a leaf function.
			"add r7, sp, #0;");

	__asm__("sub r0, r1");

	__asm__("pop {r7}");
	__asm__("BX LR ");
}

__attribute__((naked))int main(void)
{
// 1. push old LR value and old frame pointer.
	__asm("push {r7, lr};"
			"sub sp, #16;"
			"add r7, sp, #0;");

	__asm("mov.w r0, #30");
	__asm("str r0, [r7, #12]");

	__asm("mov.w r1, #20");
	__asm("str r1, [r7, #8]");

	__asm("BL bar");

	__asm("str r0, [r7, #4]");

	__asm("ldr r3, [r7, #4]");

	__asm("add sp, #16;"
		  "pop {lr, r7};");

  while (1)
  {
	  ;;
  }
}