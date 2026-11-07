/**
 * @file    xrt_kernel.h
 * @brief   Kernel lifecycle and the critical section the application needs.
 *
 * Bring-up order is fixed: xrt_kernel_init first, then one xrt_thread_create
 * per thread, then xrt_kernel_start. The application must also call
 * xrt_kernel_check_sleeping_threads from whatever it uses as a tick source,
 * or no delay ever expires.
 */

#ifndef INC_XRT_KERNEL_H_
#define INC_XRT_KERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#include "xrt_thread.h"
#include "xrt_port.h"

/**
 * @brief Prepares the kernel: clears the lists, sets the exception
 *        priorities and creates the idle thread.
 *
 * @return false when a list fails to initialise or the idle thread cannot be
 *         created.
 *
 */
bool xrt_kernel_init(void);

/**
 * @brief Hands the CPU to the highest priority ready thread and starts
 *        scheduling.
 *
 * @return false when the ready list is empty or the kernel is already
 *         running.
 *
 */
bool xrt_kernel_start(void);

/**
 * @brief Scans the sleeping threads and returns the ones whose delay has
 *      expired to the ready list
 *
 * Decrements the remaining delay of every sleeping thread and moves the ones
 * that reach zero back to the ready list, then asks for a context switch so
 * threads of equal priority take turns.
 *
 * @note The application owns the tick source. Call this from its handler —
 *       SysTick, a hardware timer, whatever the integration uses. The rate
 *       chosen here is the unit xrt_thread_delay counts in.
 */
void xrt_kernel_check_sleeping_threads(void);

/**
 * @brief Masks every interrupt at or below XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY.
 *
 * @return The previous mask, to be handed back to xrt_kernel_exit_critical.
 *
 */
static inline uint32_t xrt_kernel_enter_critical(void){
	return xrt_port_enter_critical();
}

/**
 * @brief Restores the mask a critical section replaced.
 *
 * @param old Value returned by the matching xrt_kernel_enter_critical.
 */
static inline void xrt_kernel_exit_critical(uint32_t old){
	xrt_port_exit_critical(old);
}

#endif /* INC_XRT_KERNEL_H_ */