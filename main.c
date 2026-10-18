#include "main.h"
#include "gpio.h"

#include "xrt_kernel.h"
#include "xrt_app.h"
#include "xrt_semaphore.h"

void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

int main(void)
{

  FPU->FPCCR &= ~(FPU_FPCCR_LSPEN_Msk | FPU_FPCCR_ASPEN_Msk);

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();

	if(!xrt_kernel_init()){
		Error_Handler();
	}

  if(!xrt_app_init()){
	  Error_Handler();
	}
//  /* Never returns from app start*/
  xrt_app_start();

  while(1)
  {
	  ;;
  }
}
