/*
 * xrt_kernel.h
 *
 *  Created on: Sep 1, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_KERNEL_H_
#define INC_XRT_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#include "xrt_port.h"

bool xrt_kernel_init(void);
bool xrt_kernel_start(void);

__STATIC_FORCEINLINE uint32_t xrt_kernel_enter_critical(void){
	return xrt_port_enter_critical();
}

__STATIC_FORCEINLINE void xrt_kernel_exit_critical(uint32_t old){
	xrt_port_exit_critical(old);
}
#endif /* INC_XRT_KERNEL_H_ */
