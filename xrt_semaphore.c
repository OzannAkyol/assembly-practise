/*
 * xrt_semaphore.c
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */
#include "xrt_semaphore.h"

void xrt_semaphore_init(xrtSemaphore_t* semaphore, uint16_t initial_value, uint16_t max_value){
    cdll_init_list(&semaphore -> wating_list);
    semaphore -> semaphore_value = initial_value; // available at init sequence
    semaphore -> semaphore_max_value = max_value;
}

void xrt_semaphore_release(xrtSemaphore_t* semaphore_p){
    register uint32_t* r0 __asm("r0") = (uint32_t*)semaphore_p;
    __asm volatile("SVC #2" : : "r"(r0));
}

void xrt_semaphore_take(xrtSemaphore_t* semaphore_p){
    register uint32_t* r0 __asm("r0") = (uint32_t*)semaphore_p;
    __asm volatile("SVC #3" : : "r"(r0));;
}

//TODO: need to add the release_from_isr.
