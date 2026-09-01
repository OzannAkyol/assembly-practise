/*
 * thread.h
 *
 *  Created on: Apr 4, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_THREAD_H_
#define INC_XRT_THREAD_H_

#include <stdint.h>
#include "xrt_list.h"

typedef cdll_list TCB_List_t;

typedef enum{
    LOW_PRIORITY,
    MEDIUM_PRIORITY,
    HIGH_PRIORITY,
    MAX_NUM_OF_PRIORITY
}Priority_t;

typedef enum{
    THREAD_ID_1,
    THREAD_ID_2,
    THREAD_ID_3,
    THREAD_ID_4,
    THREAD_ID_5,
    THREAD_ID_RESERVED,
    MAX_NUM_OF_THREAD
}ThreadId_t;

typedef enum{
    THREAD_READY_STATE,
    THREAD_RUNNING_STATE,
    THREAD_BLOCKED_STATE,
}ThreadState_t;

typedef enum{
    XRT_BLOCK_NONE = 0,
    XRT_THREAD_OS_DELAY,
    XRT_THREAD_BLOCK_MUTEX,
    XRT_THREAD_BLOCK_SEMAPHORE,
}ThreadBlockedReason_t;

typedef struct{
    ThreadId_t thread_id;
    uint32_t* thread_sp;
    uint32_t* thread_base_ptr;
    uint32_t ThreadStackSize;
    void (*fptr)(void);     // fp to thread execution function.
    Priority_t base_priority;
    Priority_t currentPriority;
    ThreadState_t state;
    ThreadBlockedReason_t blocked_reason;
    uint32_t wake_tick;
    TCB_List_t* currently_located_list;
    cdll_node* thread_node;	//embed the node information to tcb Since decrease the code complexity.
}TCB_t;

bool xrt_thread_list_init(TCB_List_t* list);
bool xrt_thread_init(TCB_List_t* list ,TCB_t* node);
void xrt_thread_start();
void xrt_set_os_priority_order(void);
void xrt_thread_yield(void);
void xrt_thread_delay(uint32_t ms);

uint32_t xrt_enter_critical_section(void);
void xrt_exit_critical_section(uint32_t old);


#endif /* INC_XRT_THREAD_H_ */
