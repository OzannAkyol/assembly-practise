/**
 * @file    xrt_semaphore.h
 * @brief   Counting semaphore, also usable as a binary semaphore with a
 *          maximum of one.
 *
 * Waiters are kept in priority order,.
 */

#ifndef INC_XRT_SEMAPHORE_H_
#define INC_XRT_SEMAPHORE_H_

#include "xrt_list.h"
#include "xrt_thread.h"

typedef cdll_list xrt_semaphore_list;

/** @brief Semaphore object. Allocated by the application, filled in by init. */
typedef struct{
    uint16_t           semaphore_value;     /**< Tokens currently available. */
    uint16_t           semaphore_max_value; /**< Ceiling; a release past it is dropped. */
    xrt_semaphore_list waiting_list;        /**< Blocked threads, highest priority first. */
}xrt_semaphore_t;

/**
 * @brief Initialises a semaphore.
 *
 * @param semaphore     Caller-owned object; contents are overwritten.
 * @param initial_value Tokens available at the start. Zero is valid and is
 *                      what a signalling semaphore starts from.
 * @param max_value     Ceiling. One makes it a binary semaphore.
 * @return false when @p semaphore is NULL, @p max_value is zero, or
 *         @p initial_value exceeds it.
 */
bool xrt_semaphore_init(xrt_semaphore_t* semaphore, uint16_t initial_value, uint16_t max_value);

/**
 * @brief Returns a token, waking the highest priority waiter if there is one.
 *
 * @param semaphore_p Semaphore to release.
 *
 */
void xrt_semaphore_release(xrt_semaphore_t* semaphore_p);

/**
 * @brief Takes a token, blocking the calling thread when none is available.
 *
 * @param semaphore_p Semaphore to take.
 *
 */
void xrt_semaphore_take(xrt_semaphore_t* semaphore_p);

/**
 * @brief Releases a semaphore from interrupt context.
 *
 * @param semaphore_p Semaphore to release.
 *
 */
void xrt_semaphore_release_from_ISR(xrt_semaphore_t* semaphore_p);

/**
 * @brief Releases a token into an already locked semaphore.
 *
 * Wakes the highest priority waiter if the wait list is not empty, and
 * otherwise raises the token count up to the ceiling.
 *
 * @param semaphore_p Semaphore to signal.
 *
 */
void xrt_semaphore_signal_locked(xrt_semaphore_t* semaphore_p);

/**
 * @brief Takes a token from an already locked semaphore, without blocking.
 *
 * @param semaphore Semaphore to take from.
 * @return false when @p semaphore is NULL or holds no token; the caller
 *         decides what to do instead, whether that is blocking the running
 *         thread or reporting failure.
 *
 */
bool xrt_semaphore_try_take_locked(xrt_semaphore_t* semaphore);

#endif /* INC_XRT_SEMAPHORE_H_ */