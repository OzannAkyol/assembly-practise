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
#include "xrt_kernel.h"

#define XRT_QUEUE_NOT_AVAILABLE_AT_STARTUP			(0u)

bool xrt_queue_init(xrt_queue_t* queue){
	if(queue == NULL || queue -> queue_data == NULL || queue -> block_size == 0 || queue -> queue_capacity == 0){
		return false;
	}

	if(queue -> queue_capacity % queue -> block_size != 0){
		return false;
	}

	uint32_t block_count = queue->queue_capacity / queue->block_size;
	if(block_count > UINT16_MAX){
	    return false;
	}

	if(queue -> write_access_sem == NULL || queue -> read_access_sem == NULL){
		return false;
	}

	queue -> write_ptr = queue -> queue_data;
	queue -> read_ptr = queue -> queue_data;

	xrt_semaphore_init(queue -> write_access_sem, block_count, block_count);
	xrt_semaphore_init(queue -> read_access_sem, XRT_QUEUE_NOT_AVAILABLE_AT_STARTUP, block_count);

	return true;
}

bool xrt_queue_send(xrt_queue_t* queue, void* data){
	if(queue == NULL || data == NULL){
		return false;
	}

	xrt_semaphore_take(queue -> write_access_sem);
	uint32_t old_prio = xrt_kernel_enter_critical();

	memcpy(queue -> write_ptr, data, queue-> block_size);
	queue -> write_ptr += queue-> block_size;

	if(queue -> write_ptr == queue-> queue_data + queue-> queue_capacity){
		queue -> write_ptr = queue-> queue_data;
	}

	xrt_kernel_exit_critical(old_prio);

	xrt_semaphore_release(queue -> read_access_sem);

	return true;
}

bool xrt_queue_receive(xrt_queue_t* queue, void* data){
	if(queue == NULL || data == NULL){
		return false;
	}

	xrt_semaphore_take(queue -> read_access_sem);

	uint32_t old_prio = xrt_kernel_enter_critical();

	memcpy(data, queue->read_ptr, queue-> block_size);
	queue-> read_ptr += queue->block_size;

	if(queue -> read_ptr == queue -> queue_data + queue -> queue_capacity){
		queue -> read_ptr  = queue -> queue_data;
	}

	xrt_kernel_exit_critical(old_prio);
	xrt_semaphore_release(queue -> write_access_sem);

	return true;
}

bool xrt_queue_send_from_ISR(xrt_queue_t* queue, void* data){
    if(queue == NULL || data == NULL){
		return false;
    }

	uint32_t old_priority = xrt_kernel_enter_critical();

	if(!xrt_semaphore_try_take_locked(queue -> write_access_sem)){
		xrt_kernel_exit_critical(old_priority);
		return false;
	}

	memcpy(queue -> write_ptr, data, queue -> block_size);

	queue -> write_ptr += queue -> block_size;

	if(queue -> write_ptr == queue -> queue_data + queue -> queue_capacity){
		queue -> write_ptr = queue -> queue_data;
	}

	xrt_semaphore_signal_locked(queue-> read_access_sem);

	xrt_kernel_exit_critical(old_priority);
	return true;
}
