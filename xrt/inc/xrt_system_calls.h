/**
 * @file    xrt_system_calls.h
 * @brief   Syscall numbering and the dispatch entry point.
 *
 *
 */

#ifndef INC_XRT_SYSTEM_CALLS_H_
#define INC_XRT_SYSTEM_CALLS_H_

#include <stdint.h>

/**
 * @brief Syscall numbers, encoded into the instruction that raises them.
 *
 */
typedef enum{
	SYS_CALL_MUTEX_LOCK = 0,
	SYS_CALL_MUTEX_UNLOCK,
	SYS_CALL_SEMAPHORE_RELEASE,
	SYS_CALL_SEMAPHORE_TAKE,
	SYS_CALL_THREAD_YIELD,
	SYS_CALL_OS_DELAY,
}SystemCalls_t;

/**
 * @brief Runs the kernel side of a syscall.
 *
 * @param svc_number Syscall number the port decoded.
 * @param svc_arg    Raw argument word the caller left behind. Its meaning
 *                   depends on the number: a handle for the mutex and
 *                   semaphore calls, a tick count for the delay, unused for
 *                   yield.
 *
 */
void xrt_system_calls_dispatch(uint8_t svc_number, uint32_t svc_arg);

#endif /* INC_XRT_SYSTEM_CALLS_H_ */