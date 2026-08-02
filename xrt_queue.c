/*
 * xrt_queue.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "xrt_queue.h"

// TODO: it will be a static allocation or dynamic allocation, not fixed.
static xrtSemaphore_t read_access_semaphore;
static xrtSemaphore_t write_access_semaphore;
static xrtMutex_t mutex_queue;

#define XRT_QUEUE_NOT_AVAILABLE_AT_STARTUP			(0u)

void xrt_queue_init(xrtQueue_t* queue){
	if(queue != NULL){
		queue -> write_ptr = queue -> queue_data;
		queue -> read_ptr = queue -> queue_data;

		//for now, it is a static, and single queue test
		xrt_semaphore_init(&write_access_semaphore, queue -> queue_capacity, queue -> queue_capacity);
		xrt_semaphore_init(&read_access_semaphore, XRT_QUEUE_NOT_AVAILABLE_AT_STARTUP, queue -> queue_capacity);

		xrt_mutex_init(&mutex_queue);
	}
}

bool xrt_queue_send(xrtQueue_t* queue, uint32_t* data){
	if(queue == NULL || data == NULL){
		return false;
	}

	xrt_semaphore_take(&write_access_semaphore);

	// update base priority, TODO: for now we test with using mutex, normally, we use a base priority for ISR safety.
	xrt_mutex_lock(&mutex_queue);

	*queue -> write_ptr = *data;
	 queue -> write_ptr++;

	if(queue -> write_ptr == queue-> queue_data + queue-> queue_capacity){
		queue -> write_ptr = queue-> queue_data;
	}
	xrt_mutex_unlock(&mutex_queue);
	// load original base priority
	xrt_semaphore_release(&read_access_semaphore);
	return true;
}

bool xrt_queue_receive(xrtQueue_t* queue, uint32_t* data){
	if(queue == NULL || data == NULL){
		return false;
	}

	xrt_semaphore_take(&read_access_semaphore);

	xrt_mutex_lock(&mutex_queue);

	*data = *queue->read_ptr;
	 queue-> read_ptr++;

	if(queue -> read_ptr == queue -> queue_data + queue -> queue_capacity){
		queue -> read_ptr  = queue -> queue_data;
	}

	xrt_mutex_unlock(&mutex_queue);
	xrt_semaphore_release(&write_access_semaphore);

	return true;
}
