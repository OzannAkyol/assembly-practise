/**
 * @file    xrt_kernel_internal.h
 * @brief   Kernel state and the state transitions the primitives drive.
 *
 * Not part of the public API. Every function here must be called inside a
 * critical section, and all of them touch the kernel lists directly.
 *
 * The three lists are the kernel's only global state. A thread is in exactly
 * one of them, or in a primitive's wait list, and its TCB names which through
 * currently_located_list.
 */

#ifndef INC_XRT_KERNEL_INTERNAL_H_
#define INC_XRT_KERNEL_INTERNAL_H_

#include "xrt_kernel.h"

/**
 * @brief Set by xrt_kernel_start, cleared by the first context switch.
 *
 * Read from the port's PendSV handler, which skips saving a context the first
 * time round because there is no thread to save.
 
 */
extern volatile bool is_os_first_cs_occurs;

extern cdll_list xrt_kernel_ready_list;    /**< Runnable threads, highest priority first. */
extern cdll_list xrt_kernel_running_list;  /**< Holds exactly one thread once the kernel runs. */
extern cdll_list xrt_kernel_stopped_list;  /**< Threads waiting for a delay to expire. */

/**
 * @brief Moves a blocked thread back to the ready list and asks for a switch If it is needed.
 *
 * @param thread Thread to wake.
 *
 * @note The caller removes the thread from its own wait list first; this
 *       function only handles the kernel side of the transition.
 */
void xrt_kernel_wake_thread(TCB_t* thread);

/**
 * @brief Marks the running thread blocked and names the list it will wait in.
 *
 * @param thread    Thread to block
 * @param wait_list List the thread will be moved into.
 * @param reason    What it is waiting for.
 *
 */
void xrt_kernel_block_thread(TCB_t* thread, cdll_list* wait_list, ThreadBlockedReason_t reason);

/**
 * @brief Re-sorts a thread within the list it already belongs to.
 *
 * @param thread Thread whose priority changed.
 *
 */
void xrt_kernel_reorder_thread(TCB_t* thread);

/**
 * @brief Inserts a thread into a list, ordered by current priority.
 *
 * @param list   List to insert into.
 * @param thread Thread to insert.
 * @return false when either argument is NULL.
 *
 */
bool xrt_kernel_push_thread_by_priority(cdll_list* list, TCB_t* thread);

/**
 * @brief Picks the next thread to run and relocates the outgoing one.
 *
 */
void xrt_kernel_schedule(void);

#endif /* INC_XRT_KERNEL_INTERNAL_H_ */