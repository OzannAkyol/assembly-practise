/*
 * xrt_kernel_internal.h
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_KERNEL_INTERNAL_H_
#define INC_XRT_KERNEL_INTERNAL_H_

#include "xrt_thread.h"

extern volatile bool is_os_first_cs_occurs;

extern TCB_List_t xrt_kernel_ready_list;
extern TCB_List_t xrt_kernel_running_list;
extern TCB_List_t xrt_kernel_stopped_list;

void xrt_kernel_schedule(void);

#endif /* INC_XRT_KERNEL_INTERNAL_H_ */
