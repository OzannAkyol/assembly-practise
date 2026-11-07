/**
 * @file    xrt_port_cm4.h
 * @brief   Cortex-M4 port: the interface the kernel expects from an
 *          architecture.
 *
 * A port supplies four things. A critical section that masks everything the
 * kernel could be interrupted by, a way to ask for a context switch, a one
 * time setup of the exception priorities, and the wrappers a thread uses to
 * raise a syscall.
 *
 */

#ifndef INC_XRT_PORT_CM4_H_
#define INC_XRT_PORT_CM4_H_
#include <stdint.h>

#ifndef XRT_PORT_DEVICE_HEADER
#error "XRT_PORT_DEVICE_HEADER must be defined, e.g. -DXRT_PORT_DEVICE_HEADER=\"stm32f407xx.h\""
#endif

#include XRT_PORT_DEVICE_HEADER
#include "xrt_config.h"

/**
 * @brief The configured threshold in the format BASEPRI expects.
 *
 * BASEPRI holds the priority in the upper XRT_PORT_PRIO_BITS of an eight bit
 * field, so the logical priority has to be shifted up into it.
 */
#define KERNEL_BASEPRI  (XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY << (8U - XRT_PORT_PRIO_BITS))

_Static_assert(XRT_PORT_PRIO_BITS == __NVIC_PRIO_BITS,
               "XRT_PORT_PRIO_BITS does not match the device's __NVIC_PRIO_BITS");

/**
 * @brief Saves the callee-saved registers of the outgoing thread.
 *
 */
__attribute__((naked))void xrt_thread_store_context(void);

/**
 * @brief Restores the callee-saved registers of the incoming thread and
 *        points PSP at its hardware frame.
 *
 */
__attribute__((naked))void xrt_load_context(void);

/**
 * @brief Sets the priorities of the exceptions the kernel owns.
 *
 */
void xrt_port_set_os_priority_order(void);

/**
 * @brief Raises BASEPRI to the kernel API threshold.
 *
 * @return The previous BASEPRI, to be restored by xrt_port_exit_critical.
 *
 */
__STATIC_FORCEINLINE uint32_t xrt_port_enter_critical(void){
    uint32_t old = __get_BASEPRI();
    __set_BASEPRI(KERNEL_BASEPRI);
    return old;
}

/**
 * @brief Restores the BASEPRI a critical section replaced.
 *
 * @param old Value returned by the matching xrt_port_enter_critical.
 *
 */
__STATIC_FORCEINLINE void xrt_port_exit_critical(uint32_t old){
    __set_BASEPRI(old);
}

/**
 * @brief Asks for a context switch by pending PendSV.
 *
 */
__STATIC_FORCEINLINE void xrt_port_request_context_switch(void){
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/**
 * @brief Raises a syscall carrying one argument.
 *
 */
#define XRT_SYSCALL_WITH_ARG(svc_number, arg)                   \
    do {                                                        \
        register uint32_t r0 __asm("r0") = (uint32_t)(arg);     \
        __asm volatile("svc %[svc_num]"                         \
                       :                                        \
                       : [svc_num] "I" (svc_number), "r" (r0)   \
                       : "memory");                             \
    } while(0)

/** @brief Raises a syscall that carries no argument. */
#define XRT_SYSCALL_NO_ARG(svc_number)                          \
    __asm volatile("svc %[svc_num]"                             \
                   :                                            \
                   : [svc_num] "I" (svc_number)                 \