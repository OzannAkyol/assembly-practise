/*
 * xrt_syscalls_internal.h
 *
 *  Created on: Nov 6, 2026
 *      Author: ozi
 */

/**
 * @file    xrt_syscalls_internal.h
 * @brief   Implementations behind the syscall numbers, called by the dispatcher.
 *
 * Every function here runs in handler context, inside the critical section the
 * dispatcher takes, with the running thread passed in.
 *
 * The declarations sit here rather than in the primitives' own headers so the
 * application never sees them. The definitions stay with their primitive,
 * because what they do is that primitive's semantics.
 */

#ifndef INC_XRT_SYSCALLS_INTERNAL_H_
#define INC_XRT_SYSCALLS_INTERNAL_H_

#include <stdint.h>

#include "xrt_thread.h"
#include "xrt_semaphore.h"
#include "xrt_mutex.h"

/**
 * @brief Blocks the running thread until the given number of ticks elapses.
 *
 * @param arg            Tick count, taken from the syscall argument.
 * @param running_thread Thread that raised the syscall.
 */
void xrt_thread_delay_from_svc(uint32_t arg, TCB_t* running_thread);

/**
 * @brief Takes the semaphore, blocking the running thread when it is not
 *        available.
 *
 * @param semaphore      Semaphore to take.
 * @param running_thread Thread that raised the syscall.
 */
void xrt_semaphore_take_from_svc(xrt_semaphore_t* semaphore, TCB_t* running_thread);

/**
 * @brief Releases the semaphore, waking the highest priority waiter if there
 *        is one.
 *
 * @param semaphore Semaphore to release.
 *
 * @note Takes no running thread: the priority comparison that decides whether
 *       to switch is made by the kernel when it wakes the waiter.
 */
void xrt_semaphore_release_from_svc(xrt_semaphore_t* semaphore);

/**
 * @brief Takes the mutex, or blocks the running thread and lifts the owner's
 *        priority when it is already held.
 *
 * @param mutex_obj      Mutex to lock.
 * @param running_thread Thread that raised the syscall.
 *
 * @note Re-locking a mutex the running thread already owns is a no-op; the
 *       mutex is not recursive and does not count.
 */
void xrt_mutex_lock_from_svc(xrt_mutex_t* mutex_obj, TCB_t* running_thread);

/**
 * @brief Releases the mutex, handing it to the highest priority waiter and
 *        restoring the owner's base priority.
 *
 * @param mutex_obj      Mutex to unlock.
 * @param running_thread Thread that raised the syscall. A thread that is not
 *                       the owner is ignored.
 */
void xrt_mutex_unlock_from_svc(xrt_mutex_t* mutex_obj, TCB_t* running_thread);

#endif /* INC_XRT_SYSCALLS_INTERNAL_H_ */