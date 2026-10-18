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

typedef cdll_list xrt_semaphore_list;
typedef cdll_node xrts_semaphore_node;

typedef struct{
    uint16_t semaphore_value;
    uint16_t semaphore_max_value;        // counting icin ust sinir (binary'de 1)
    xrt_semaphore_list waiting_list;      // priority ordered
}xrt_semaphore_t;

bool xrt_semaphore_init(xrt_semaphore_t* semaphore, uint16_t initial_value, uint16_t max_value);
void xrt_semaphore_release(xrt_semaphore_t* semaphore_p);
void xrt_semaphore_take(xrt_semaphore_t* semaphore_p);
void xrt_semaphore_release_from_ISR(xrt_semaphore_t* semaphore_p);
void xrt_semaphore_signal_locked(xrt_semaphore_t* semaphore_p);
bool xrt_semaphore_try_take_locked(xrt_semaphore_t* semaphore);

void xrt_semaphore_take_from_svc(xrt_semaphore_t* semaphore, TCB_t* running_thread);
void xrt_semaphore_release_from_svc(xrt_semaphore_t* semaphore);

#endif /* INC_XRT_SEMAPHORE_H_ */
