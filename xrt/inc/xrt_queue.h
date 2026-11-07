/**
 * @file    xrt_queue.h
 * @brief   Fixed-size block queue between threads, or from an interrupt to a
 *          thread.
 *
 * One semaphores counts free slots and blocks a sender when the queue is full,
 * the other counts filled slots and blocks a receiver when it is empty. The
 * queue itself only copies blocks and advances the pointers.
 *
 */

#ifndef INC_XRT_QUEUE_H_
#define INC_XRT_QUEUE_H_

#include "xrt_semaphore.h"

/** @brief Queue object. Partly filled by the application, completed by init. */
typedef struct{
    uint8_t*         queue_data;       /**< Backing buffer, supplied by the caller. */
    uint32_t         block_size;       /**< Size of one block, in bytes. */
    uint32_t         queue_capacity;   /**< Buffer size in bytes. Must be a whole
                                            number of blocks. */
    uint8_t*         write_ptr;        /**< Next slot to write; set by init. */
    uint8_t*         read_ptr;         /**< Next slot to read; set by init. */
    xrt_semaphore_t* write_access_sem; /**< Counts free slots. */
    xrt_semaphore_t* read_access_sem;  /**< Counts filled slots. */
}xrt_queue_t;

/**
 * @brief Validates the queue configuration and initialises both semaphores.
 *
 * @param queue Caller-owned object with the buffer.
 *
 * @return false when a pointer is NULL, a size is zero, @p queue_capacity is
 *         not a whole number of blocks, the block count exceeds what the
 *         semaphore counters hold, or either semaphore fails to initialise.
 *
 */
bool xrt_queue_init(xrt_queue_t* queue);

/**
 * @brief Copies one block out of the queue, blocking while it is empty.
 *
 * @param queue Queue to read from.
 * @param data  Destination, at least block_size bytes.
 * @return false when either argument is NULL.
 *
 */
bool xrt_queue_receive(xrt_queue_t* queue, void* data);

/**
 * @brief Copies one block into the queue, blocking while it is full.
 *
 * @param queue Queue to write to.
 * @param data  Source, at least block_size bytes.
 * @return false when either argument is NULL.
 *
 */
bool xrt_queue_send(xrt_queue_t* queue, void* data);

/**
 * @brief Copies one block into the queue from interrupt context, failing
 *        rather than blocking when it is full.
 *
 * @param queue Queue to write to.
 * @param data  Source, at least block_size bytes.
 * @return false when either argument is NULL or the queue is full.
 *
 */
bool xrt_queue_send_from_ISR(xrt_queue_t* queue, void* data);

#endif /* INC_XRT_QUEUE_H_ */