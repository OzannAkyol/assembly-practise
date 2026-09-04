/*
 * xrt_system_calls.h
 *
 *  Created on: Sep 1, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_SYSTEM_CALLS_H_
#define INC_XRT_SYSTEM_CALLS_H_

#include <stdint.h>

typedef enum{
	SYS_CALL_MUTEX_LOCK = 0,
	SYS_CALL_MUTEX_UNLOCK,
	SYS_CALL_SEMAPHORE_RELEASE,
	SYS_CALL_SEMAPHORE_TAKE,
	SYS_CALL_THREAD_YIELD,
	SYS_CALL_OS_DELAY,
}SystemCalls_t;

#define XRT_SYSCALL_WITH_ARG(svc_number, arg)                   \
    do {                                                        \
        register uint32_t r0 __asm("r0") = (uint32_t)(arg);     \
        __asm volatile("svc %[svc_num]"                         \
                       :                                        \
                       : [svc_num] "I" (svc_number), "r" (r0)   \
                       : "memory");                             \
    } while(0)

#define XRT_SYSCALL_NO_ARG(svc_number)                          \
    __asm volatile("svc %[svc_num]"                             \
                   :                                            \
                   : [svc_num] "I" (svc_number)                 \
                   : "memory")

void xrt_system_calls_dispatch(uint8_t svc_number, uint32_t* svc_arg);

#endif /* INC_XRT_SYSTEM_CALLS_H_ */
