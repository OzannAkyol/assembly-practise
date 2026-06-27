#include "main.h"
#include "gpio.h"

#include <stdint.h>
#include "cmsis_gcc.h"
#include <stdbool.h>
#include "xrt_thread.h"
#include "xrt_list.h"


void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

#define NUM_OF_THREAD_IN_SYSTEM (4)

#define THREAD1_STACK_SIZE		64
uint32_t thread1_stack[THREAD1_STACK_SIZE];

#define THREAD2_STACK_SIZE		64
uint32_t thread2_stack[THREAD2_STACK_SIZE];

#define THREAD3_STACK_SIZE		64
uint32_t thread3_stack[THREAD3_STACK_SIZE];

#define THREAD4_STACK_SIZE		64
uint32_t thread4_stack[THREAD4_STACK_SIZE];

#define THREAD5_STACK_SIZE		64
uint32_t thread5_stack[THREAD5_STACK_SIZE];

void t1_dummy_function(void);
void t2_dummy_function(void);
void t3_dummy_function(void);
void t4_dummy_function(void);
void t5_dummy_function(void);

void t1_dummy_function(void){
	int a = 5;
	int b = 10;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
		HAL_Delay(750);
		xrt_thread_delay(10000);
	}
}

void t2_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);

	}
}

void t3_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
	}
}

void t4_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
		HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	}
}

void t5_dummy_function(void){
	int a = 5;
	int b = 20;

	(void)a;
	(void)b;

	while(1){
//		HAL_GPIO_WritePin(LED_TEST_GPIO_Port, LED_TEST_Pin, GPIO_PIN_RESET);
	}
}

TCB_t tcb1 = {
        .fptr = t1_dummy_function,
        .thread_sp = thread1_stack + THREAD1_STACK_SIZE - 1,
        .ThreadStackSize = THREAD1_STACK_SIZE,
        .priority = HIGH_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_1,
        .thread_base_ptr = thread1_stack,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
};

TCB_t tcb2 = {
        .fptr = t2_dummy_function,
        .ThreadStackSize = THREAD2_STACK_SIZE,
        .priority = MEDIUM_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_2,
        .thread_base_ptr = thread2_stack,
        .thread_sp = thread2_stack + THREAD2_STACK_SIZE - 1,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
};

TCB_t tcb3 = {
        .fptr = t3_dummy_function,
        .ThreadStackSize = THREAD3_STACK_SIZE,
        .priority = MEDIUM_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_3,
        .thread_base_ptr = thread3_stack,
        .thread_sp = thread3_stack + THREAD3_STACK_SIZE - 1,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
};

TCB_t tcb4 = {
        .fptr = t4_dummy_function,
        .ThreadStackSize = THREAD4_STACK_SIZE,
        .priority = MEDIUM_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_4,
        .thread_base_ptr = thread4_stack,
        .thread_sp = thread4_stack + THREAD4_STACK_SIZE - 1,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
};

TCB_t tcb5 = {
        .fptr = t5_dummy_function,
        .ThreadStackSize = THREAD5_STACK_SIZE,
        .priority = LOW_PRIORITY,
        .state = THREAD_READY_STATE,
        .thread_id = THREAD_ID_5,
        .thread_base_ptr = thread5_stack,
        .thread_sp = thread5_stack + THREAD5_STACK_SIZE - 1,
		.wake_tick = 0,
		.blocked_reason = XRT_BLOCK_NONE,
};


cdll_list list;
cdll_node thread1_node ={
	.data = &tcb1,
	.prev = NULL,
	.next = NULL,
};

cdll_node thread2_node ={
	.data = &tcb2,
	.prev = NULL,
	.next = NULL,
};

cdll_node thread3_node ={
	.data = &tcb3,
	.prev = NULL,
	.next = NULL,
};

cdll_node thread4_node ={
	.data = &tcb4,
	.prev = NULL,
	.next = NULL,
};

cdll_node thread5_node ={
	.data = &tcb5,
	.prev = NULL,
	.next = NULL,
};

TCB_List_t xrtKernelReadyList={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrtKernelRunningList ={
		.head = NULL,
		.size = 0,
};

TCB_List_t xrtKernelStoppedList ={
		.head = NULL,
		.size = 0,
};

int main(void)
{

//  __disable_irq();
  FPU->FPCCR &= ~(FPU_FPCCR_LSPEN_Msk | FPU_FPCCR_ASPEN_Msk);

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();

  for(int i = 0; i < THREAD2_STACK_SIZE; ++i){
	  thread1_stack[i] = 11;
	  thread2_stack[i] = 11;
	  thread3_stack[i] = 11;
	  thread4_stack[i] = 11;
	  thread5_stack[i] = 11;
  }

  xrt_thread_list_init(&xrtKernelRunningList);
//  xrt_thread_list_init(&xrtKernelStoppedList);

  xrt_thread_list_init(&xrtKernelReadyList);
  xrt_thread_init(&xrtKernelReadyList, &tcb1, &thread1_node);
  xrt_thread_init(&xrtKernelReadyList, &tcb2, &thread2_node);
  xrt_thread_init(&xrtKernelReadyList, &tcb3, &thread3_node);
  xrt_thread_init(&xrtKernelReadyList, &tcb4, &thread4_node);
  xrt_thread_init(&xrtKernelReadyList, &tcb5, &thread5_node);

  xrt_thread_start();

  while(1)
  {

  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
