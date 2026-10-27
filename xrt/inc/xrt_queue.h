/*
 * xrt_queue.h
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_QUEUE_H_
#define INC_XRT_QUEUE_H_

#include "xrt_list.h"
#include "xrt_semaphore.h"

typedef cdll_list xrt_queue_list;

typedef struct{
	uint8_t* queue_data;
	uint32_t block_size;		// holds the piece of fixed block size in byte.
	uint32_t queue_capacity;	// holds the total number byte.
	uint8_t* write_ptr;
	uint8_t* read_ptr;
	xrt_semaphore_t* write_access_sem;
	xrt_semaphore_t* read_access_sem;
}xrt_queue_t;

bool xrt_queue_init(xrt_queue_t* queue);
bool xrt_queue_receive(xrt_queue_t* queue, void* data);
bool xrt_queue_send(xrt_queue_t* queue, void* data);
bool xrt_queue_send_from_ISR(xrt_queue_t* queue, void* data);

#endif /* INC_XRT_QUEUE_H_ */
