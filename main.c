#define NUM_OF_THREAD_IN_SYSTEM (4)

#define THREAD1_STACK_SIZE		64
uint32_t thread1_stack[THREAD1_STACK_SIZE];

#define THREAD2_STACK_SIZE		64
uint32_t thread2_stack[THREAD2_STACK_SIZE];

#define THREAD3_STACK_SIZE		64
uint32_t thread3_stack[THREAD3_STACK_SIZE];

#define THREAD4_STACK_SIZE		64
uint32_t thread4_stack[THREAD4_STACK_SIZE];

void t1_dummy_function(void);
void t2_dummy_function(void);

void t1_dummy_function(void){
	int a = 5;
	int b = 10;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LED_TEST_GPIO_Port, LED_TEST_Pin, GPIO_PIN_SET);
	}
}

void t2_dummy_function(void){
	int a = 5;
	int b = 20;

	while(1){
		HAL_GPIO_WritePin(LED_TEST_GPIO_Port, LED_TEST_Pin, GPIO_PIN_RESET);
	}
}

TCB_t tcb1 = {
        .fptr = t1_dummy_function,
        .thread_sp = thread1_stack + THREAD1_STACK_SIZE - 1,
        .ThreadStackSize = THREAD1_STACK_SIZE,
        .priority = LOW_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = "T1",
        .thread_base_ptr = thread1_stack,
};


TCB_t tcb2 = {
        .fptr = t2_dummy_function,
        .ThreadStackSize = THREAD2_STACK_SIZE,
        .priority = HIGH_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = "T2",
        .thread_base_ptr = thread2_stack,
        .thread_sp = thread2_stack + THREAD2_STACK_SIZE - 1,
};

__STATIC_FORCEINLINE void xrt_thread_start();
__STATIC_FORCEINLINE void xrt_thread_start(){
	uint32_t value = (uint32_t)tcb1.thread_sp;
	__set_PSP(value);
	__set_CONTROL(__get_CONTROL() | 0x2);

	tcb1.fptr();

}

// it is written only
__attribute__((naked))void xrt_thread_init(void);
__attribute__((naked))void xrt_thread_init(void){
	__asm("push {r7, lr}");

	__asm("ldr r7, =thread2_stack");
	__asm("add.w r7, 252"); // (64 - 1)* 4, 64 length of array

	__asm("add r0, r7, #0");

	__asm("ldr r1, =0x01000000");	//@note: xPSR value I saw this kind of usage for thread_1;
	__asm("str r1, [r0, #0]");

	__asm("ldr r1, =t2_dummy_function");
	__asm("sub r0, #4");		  // now, I need to assign t2_dummy_function address to stack area.
	__asm("str  r1, [r0, #0]");   // it means that when exception returned, this value will be PC.

	__asm("ldr r1, =0xFFFFFFFD"); // may be LR value, I don't determine the exact value.
	__asm("sub r0, #4"); 			// I'm not sure which LR value will be written
	__asm("str  r1, [r0, #0]");

	__asm("mov r1, 0");	//r12
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");

	__asm("mov r1, 0");//r3
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");

	__asm("mov r1, 0");//r2
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");

	__asm("mov r1, 0");//r1
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r0

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r4

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r5

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r6

//	__asm("mov r1, 0");
	__asm("ldr r1, [r0, #0]");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r7

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r8

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r9

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r10

	__asm("mov r1, 0");
	__asm("sub r0, #4");
	__asm("str  r1, [r0, #0]");//r11

	__asm("ldr r7, =tcb2;"
		  "add r7, r7, #4");

	__asm("str r0, [r7, #0]"); /*TODO: fix this  dynamic assignment, but we update the stack pointer*/

	__asm("pop {r7, lr}");

	__asm("bx lr");

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	  FPU->FPCCR &= ~(FPU_FPCCR_LSPEN_Msk | FPU_FPCCR_ASPEN_Msk);
	__enable_irq();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
//  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
//  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
//  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
	MX_GPIO_Init();


  for(int i = 0; i < THREAD2_STACK_SIZE; ++i){
	  thread1_stack[i] = 11;
	  thread2_stack[i] = 11;
  }

  xrt_thread_init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  xrt_thread_start();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}