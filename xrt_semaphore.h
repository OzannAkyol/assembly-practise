/*
 * xrt_semaphore.h
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_SEMAPHORE_H_
#define INC_XRT_SEMAPHORE_H_

#include "xrt_list.h"
#include "xrt_thread.h"

#define XRT_SEMAPHORE_AVAILABLE			(1U)
#define XRT_SEMAPHORE_NOT_AVAILABLE		(0U)

typedef cdll_list xrtSemaphore_list;
typedef cdll_node xrtSemaphore_node;

typedef struct{
    uint16_t semaphore_value;
    uint16_t semaphore_max_value;        // counting icin ust sinir (binary'de 1)
    xrtSemaphore_list waiting_list;      // priority ordered
}xrtSemaphore_t;

void xrt_semaphore_init(xrtSemaphore_t* semaphore, uint16_t initial_value, uint16_t max_value);
void xrt_semaphore_release(xrtSemaphore_t* semaphore_p);
void xrt_semaphore_take(xrtSemaphore_t* semaphore_p);
void xrt_semaphore_release_from_ISR(xrtSemaphore_t* semaphore_p);
void xrt_semaphore_signal_locked(xrtSemaphore_t* semaphore_p);

void xrt_semaphore_take_from_svc(xrtSemaphore_t* semaphore, TCB_t* running_thread);
void xrt_semaphore_release_from_svc(xrtSemaphore_t* semaphore, TCB_t* running_thread);

#endif /* INC_XRT_SEMAPHORE_H_ */
