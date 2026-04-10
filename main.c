#include "xrt_thread.h"


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
void t3_dummy_function(void);

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
        .ThreadStackSize = THREAD1_STACK_SIZE * 4,
        .priority = LOW_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = "T1",
        .thread_base_ptr = thread1_stack,
};


TCB_t tcb2 = {
        .fptr = t2_dummy_function,
        .ThreadStackSize = THREAD2_STACK_SIZE * 4,
        .priority = HIGH_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = "T2",
        .thread_base_ptr = thread2_stack,
        .thread_sp = thread2_stack + THREAD2_STACK_SIZE - 1,
};

TCB_t tcb3 = {
        .fptr = t3_dummy_function,
        .ThreadStackSize = THREAD3_STACK_SIZE,
        .priority = HIGH_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = "T3",
        .thread_base_ptr = thread3_stack,
        .thread_sp = thread3_stack + THREAD3_STACK_SIZE - 1,
};

cdll_node thread1_node = {
		.data = &tcb1,
		.next = NULL,
		.prev = NULL,
};

cdll_node thread2_node = {
		.data = &tcb2,
		.next = NULL,
		.prev = NULL,
};

cdll_node thread3_node = {
		.data = &tcb3,
		.next = NULL,
		.prev = NULL,
};

TCB_List_t xrtKernelList = {
							.head = NULL,
							.size = 0,
};

__STATIC_FORCEINLINE void xrt_thread_start();
__STATIC_FORCEINLINE void xrt_thread_start(){
	uint32_t value = (uint32_t)tcb1.thread_sp;
	__set_PSP(value);
	__set_CONTROL(__get_CONTROL() | 0x2);

	tcb1.fptr();

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  	xrt_thread_list_init(&xrtKernelList);

	xrt_thread_init(&xrtKernelList, &tcb1, &thread1_node);
	xrt_thread_init(&xrtKernelList, &tcb2, &thread2_node);

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