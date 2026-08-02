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
#include "xrt_mutex.h"

typedef cdll_list xrt_queue_list;

typedef struct{
	uint32_t* queue_data;
	uint32_t* write_ptr;
	uint32_t* read_ptr;
	uint32_t queue_capacity;
}xrtQueue_t;

void xrt_queue_init(xrtQueue_t* queue);
bool xrt_queue_send(xrtQueue_t* queue, uint32_t* data);
bool xrt_queue_receive(xrtQueue_t* queue, uint32_t* data);

#endif /* INC_XRT_QUEUE_H_ */
