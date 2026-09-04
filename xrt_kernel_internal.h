/*
 * xrt_kernel_internal.h
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_KERNEL_INTERNAL_H_
#define INC_XRT_KERNEL_INTERNAL_H_

#include "xrt_thread.h"

extern TCB_List_t xrtKernelReadyList;
extern TCB_List_t xrtKernelRunningList;
extern TCB_List_t xrtKernelStoppedList;
extern volatile bool is_os_kernel_started;
extern volatile bool is_os_first_cs_occurs;

void xrt_kernel_schedule(void);

#endif /* INC_XRT_KERNEL_INTERNAL_H_ */
