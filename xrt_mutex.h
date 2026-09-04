/*
 * xrt_mutex.h
 *
 *  Created on: Jun 30, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_MUTEX_H_
#define INC_XRT_MUTEX_H_

#include "xrt_thread.h"

typedef cdll_list	mutex_list;
typedef cdll_node	mutex_node;

typedef struct{
	TCB_t* mutexOwner; // it should points to thread TCB,
	mutex_list waitingList;
}xrtMutex_t;

bool xrt_mutex_init(xrtMutex_t* mutex_obj);
void xrt_mutex_lock(xrtMutex_t* mutex_obj);
void xrt_mutex_unlock(xrtMutex_t* mutex_obj);
void xrt_mutex_lock_from_svc(xrtMutex_t* mutex_obj, TCB_t* running_thread);
void xrt_mutex_unlock_from_svc(xrtMutex_t* mutex_ptr, TCB_t* running_thread);

#endif /* INC_XRT_MUTEX_H_ */
