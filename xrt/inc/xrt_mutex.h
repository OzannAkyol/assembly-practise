/**
 * @file    xrt_mutex.h
 * @brief   Mutual exclusion lock with priority inheritance.
 *
 * Unlike a binary semaphore the mutex has an owner, which is what lets it
 * address priority inversion: while a lower priority thread holds a mutex a
 * higher priority thread is waiting on, the owner runs at the waiter's
 * priority until it unlocks.
 *
 * Inheritance is single level. An owner that is itself blocked on a second
 * mutex does not pass the raised priority further down the chain.
 *
 * The mutex is not recursive and keeps no lock count: an owner that locks it
 * again is left holding it, and a single unlock releases it.
 */

#ifndef INC_XRT_MUTEX_H_
#define INC_XRT_MUTEX_H_

#include "xrt_thread.h"
#include "xrt_list.h"

typedef cdll_list mutex_list;

/** @brief Mutex object. Allocated by the application, filled in by init. */
typedef struct{
    TCB_t*     mutex_owner;  /**< Thread currently holding the lock, NULL when free. */
    mutex_list waiting_list; /**< Blocked threads, highest priority first. */
}xrt_mutex_t;

/**
 * @brief Initialises a mutex in the unlocked state.
 *
 * @param mutex_obj Caller-owned object; contents are overwritten.
 * @return false when @p mutex_obj is NULL.
 */
bool xrt_mutex_init(xrt_mutex_t* mutex_obj);

/**
 * @brief Takes the lock, blocking the calling thread when another holds it.
 *
 * Raises the owner's priority to the caller's before blocking, if the caller
 * ranks higher, and re-sorts the owner within whatever list it sits in.
 *
 * @param mutex_obj Mutex to lock.
 *
 */
void xrt_mutex_lock(xrt_mutex_t* mutex_obj);

/**
 * @brief Releases the lock and hands it to the highest priority waiter.
 *
 * Restores the caller's base priority, then transfers ownership directly to
 * the first waiter rather than leaving the mutex free for whoever asks next.
 *
 * @param mutex_obj Mutex to unlock.
 *
 * @note A thread that does not own the mutex is ignored.
 */
void xrt_mutex_unlock(xrt_mutex_t* mutex_obj);

#endif /* INC_XRT_MUTEX_H_ */