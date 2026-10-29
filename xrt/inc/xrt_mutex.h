/*
 * xrt_mutex.h
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_MUTEX_H_
#define INC_XRT_MUTEX_H_

#include "xrt_thread.h"
#include "xrt_list.h"

typedef cdll_list	mutex_list;

typedef struct{
	TCB_t* mutex_owner; // it should points to thread TCB,
	mutex_list waiting_list;
}xrt_mutex_t;

bool xrt_mutex_init(xrt_mutex_t* mutex_obj);
void xrt_mutex_lock(xrt_mutex_t* mutex_obj);
void xrt_mutex_unlock(xrt_mutex_t* mutex_obj);
void xrt_mutex_lock_from_svc(xrt_mutex_t* mutex_obj, TCB_t* running_thread);
void xrt_mutex_unlock_from_svc(xrt_mutex_t* mutex_ptr, TCB_t* running_thread);

#endif /* INC_XRT_MUTEX_H_ */
