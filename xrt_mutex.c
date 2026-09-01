/*
 * xrt_mutex.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include <stdbool.h>
#include <stddef.h>

#include "xrt_list.h"
#include "xrt_mutex.h"

bool xrt_mutex_init(xrtMutex_t* mutex_obj){
	mutex_obj -> mutexOwner = NULL;
	return cdll_init_list(&mutex_obj -> waitingList);
}

void xrt_mutex_lock(xrtMutex_t* mutex_obj){
	register uint32_t* r0 __asm("r0") = (uint32_t*)mutex_obj;
	__asm volatile("SVC #4" : : "r"(r0));
}

void xrt_mutex_unlock(xrtMutex_t* mutex_obj){
	register uint32_t* r0 __asm("r0") = (uint32_t*)mutex_obj;
	__asm volatile("SVC #5" : : "r"(r0));
}
