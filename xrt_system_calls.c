/*
 * xrt_system_calls.c
 *
 *  Created on: Sep 1, 2026
 *      Author: ozi
 */

#include "xrt_system_calls.h"
#include "xrt_port.h"
#include "xrt_kernel.h"
#include "xrt_kernel_internal.h"
#include "xrt_mutex.h"
#include "xrt_semaphore.h"
#include "xrt_thread.h"

void xrt_system_calls_dispatch(uint8_t svc_number, uint32_t* svc_arg){
    uint32_t old_prio = xrt_kernel_enter_critical();

    TCB_t* running_thread = xrtKernelRunningList.head->data;

    switch((SystemCalls_t)svc_number){
        case SYS_CALL_MUTEX_LOCK:
            xrt_mutex_lock_from_svc((xrtMutex_t*)svc_arg, running_thread);
            break;

        case SYS_CALL_MUTEX_UNLOCK:
            xrt_mutex_unlock_from_svc((xrtMutex_t*)svc_arg, running_thread);
            break;

        case SYS_CALL_SEMAPHORE_RELEASE:
            xrt_semaphore_release_from_svc((xrtSemaphore_t*)svc_arg, running_thread);
            break;

        case SYS_CALL_SEMAPHORE_TAKE:
            xrt_semaphore_take_from_svc((xrtSemaphore_t*)svc_arg, running_thread);
            break;

        case SYS_CALL_THREAD_YIELD:
        	xrt_port_request_context_switch();
            break;

        case SYS_CALL_OS_DELAY:
            xrt_thread_delay_from_svc(*svc_arg, running_thread);
            break;

        default:
            break;
    }

    xrt_kernel_exit_critical(old_prio);
}

