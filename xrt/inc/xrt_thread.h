/**
 * @file    xrt_thread.h
 * @brief   Thread control block and the thread-facing API.
 *
 * Threads are allocated statically: the application defines the TCB, its list
 * node and its stack, then hands all three to xrt_thread_create. Every field
 * of the TCB is written by the kernel; the application writes none of them.
 */

#ifndef INC_XRT_THREAD_H_
#define INC_XRT_THREAD_H_

#include <stdint.h>
#include "xrt_list.h"

/** @brief Scheduling priority. A higher enum value is a higher priority. */
typedef enum{
    IDLE_PRIORITY,          /**< Reserved for the kernel's idle thread. */
    LOW_PRIORITY,
    MEDIUM_PRIORITY,
    HIGH_PRIORITY,
    MAX_NUM_OF_PRIORITY     /**< Bound, not a usable priority. */
}Priority_t;

/** @brief Where a thread stands with respect to the CPU. */
typedef enum{
    THREAD_READY_STATE,     /**< Runnable, waiting for the CPU. */
    THREAD_RUNNING_STATE,   /**< Currently on the CPU. */
    THREAD_BLOCKED_STATE,   /**< Waiting on a delay or on a primitive. */
}ThreadState_t;

/** @brief Why a blocked thread is blocked. */
typedef enum{
    XRT_BLOCK_NONE = 0,
    XRT_THREAD_OS_DELAY,        /**< Waiting for wake_tick to reach zero. */
    XRT_THREAD_BLOCK_MUTEX,
    XRT_THREAD_BLOCK_SEMAPHORE,
}ThreadBlockedReason_t;

/**
 * @brief Thread control block.
 *
 * The application owns the storage, the kernel owns the content.
 */
typedef struct{
    uint8_t     thread_id;              /**< Assigned by the kernel at creation. */
    uint32_t*   thread_sp;              /**< Saved stack pointer. */
    uint32_t*   thread_base_ptr;        /**< Lowest address of the stack. */
    uint32_t    thread_stack_size_word; /**< Stack size in 32-bit words. */
    void        (*fptr)(void);          /**< Entry point. Must never return. */
    Priority_t  base_priority;          /**< Priority the thread was created with. */
    Priority_t  current_priority;       /**< Effective priority. Raised above
                                             base_priority while the thread holds
                                             a mutex that a higher priority
                                             thread is waiting on. */
    ThreadState_t         state;
    ThreadBlockedReason_t blocked_reason;
    uint32_t    wake_tick;              /**< Ticks left before an OS delay
                                             expires. */
    cdll_list*  currently_located_list; /**< List the thread belongs to. Between
                                             a syscall marking the thread
                                             blocked and PendSV running, this is
                                             the list it is about to move into,
                                             not the one it still sits in. */
    cdll_node*  thread_node;            /**< This thread's list node; its data
                                             field points back at this TCB. */
}TCB_t;

/**
 * @brief Registers a thread with the kernel and places it in the ready list.
 *
 * @param tcb                Caller-owned control block. Contents are
 *                           overwritten; no field needs to be pre-filled.
 * @param tcb_node           Caller-owned list node, linked to @p tcb here.
 * @param exec_function      Thread entry point.
 * @param priority           Initial priority, below MAX_NUM_OF_PRIORITY.
 * @param stack_base         Lowest address of the thread's stack.
 * @param stack_size_in_word Stack size in 32-bit words. The top of the stack
 *                           must end up 8-byte aligned.
 *
 * @return false when an argument is NULL, the priority is out of range, the
 *         stack is too small or misaligned, or XRT_MAX_NUM_OF_THREAD is
 *         reached.
 *
 */
bool xrt_thread_create(TCB_t* tcb, cdll_node* tcb_node,
                       void       (*exec_function)(void),
                       Priority_t priority,
                       uint32_t*  stack_base,
                       uint32_t   stack_size_in_word);

/**
 * @brief Gives up the CPU to another runnable thread of equal or higher
 *        priority.
 *
 */
void xrt_thread_yield(void);

/**
 * @brief Blocks the calling thread for a number of kernel ticks.
 *
 * @param tick ticks to wait. Zero yields instead of blocking.
 *
 */
void xrt_thread_delay(uint32_t tick);

#endif /* INC_XRT_THREAD_H_ */