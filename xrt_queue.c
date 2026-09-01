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

#include <main.h>
#include "xrt_queue.h"
#include "xrt_thread.h"

extern TCB_List_t xrtKernelReadyList;

#define XRT_QUEUE_NOT_AVAILABLE_AT_STARTUP			(0u)

void xrt_queue_init(xrtQueue_t* queue){
	if(queue != NULL){
		queue -> write_ptr = queue -> queue_data;
		queue -> read_ptr = queue -> queue_data;

		xrt_semaphore_init(queue -> write_access_sem, queue -> queue_capacity / queue-> block_size, queue -> queue_capacity / queue-> block_size);
		xrt_semaphore_init(queue -> read_access_sem, XRT_QUEUE_NOT_AVAILABLE_AT_STARTUP, queue -> queue_capacity / queue-> block_size);
	}
}

//Fixed size
bool xrt_queue_send(xrtQueue_t* queue, void* data){
	if(queue == NULL || data == NULL){
		return false;
	}

	xrt_semaphore_take(queue -> write_access_sem);
	uint32_t old_prio = xrt_enter_critical_section();

	memcpy(queue -> write_ptr, data, queue-> block_size);
	queue -> write_ptr+= queue-> block_size;

	if(queue -> write_ptr == queue-> queue_data + queue-> queue_capacity){
		queue -> write_ptr = queue-> queue_data;
	}

	xrt_exit_critical_section(old_prio);

	xrt_semaphore_release(queue -> read_access_sem);

	return true;
}

//Fixed size
bool xrt_queue_receive(xrtQueue_t* queue, void* data){
	if(queue == NULL || data == NULL){
		return false;
	}

	xrt_semaphore_take(queue -> read_access_sem);

	uint32_t old_prio = xrt_enter_critical_section();

	memcpy(data, queue->read_ptr, queue-> block_size);
	queue-> read_ptr += queue->block_size;

	if(queue -> read_ptr == queue -> queue_data + queue -> queue_capacity){
		queue -> read_ptr  = queue -> queue_data;
	}

	xrt_exit_critical_section(old_prio);
	xrt_semaphore_release(queue -> write_access_sem);

	return true;
}

bool xrt_queue_send_from_ISR(xrtQueue_t* queue, void* data){
    if(queue == NULL || data == NULL){
		return false;
    }

	uint32_t old_priority = xrt_enter_critical_section();

	if(queue -> write_access_sem -> semaphore_value < 1){
		xrt_exit_critical_section(old_priority);
		return false;
	}

	memcpy(queue -> write_ptr, data, queue -> block_size);
	queue -> write_access_sem -> semaphore_value--;

	queue -> write_ptr += queue -> block_size;

	if(queue -> write_ptr == queue -> queue_data + queue -> queue_capacity){
		queue -> write_ptr = queue -> queue_data;
	}

	xrt_semaphore_signal_locked(queue-> read_access_sem);

	xrt_exit_critical_section(old_priority);
	return true;
}
