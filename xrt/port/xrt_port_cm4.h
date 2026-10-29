/*
 * xrt_port_cm4.h
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_PORT_CM4_H_
#define INC_XRT_PORT_CM4_H_
#include <stdint.h>

#ifndef XRT_PORT_DEVICE_HEADER
#error "XRT_PORT_DEVICE_HEADER must be defined (e.g. XRT_PORT_DEVICE_HEADER="stm32f407xx.h")"
#endif

#include XRT_PORT_DEVICE_HEADER
#include "xrt_config.h"

#define KERNEL_BASEPRI  (XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY << (8U - XRT_PORT_PRIO_BITS))

_Static_assert(XRT_PORT_PRIO_BITS == __NVIC_PRIO_BITS,
               "XRT_PORT_PRIO_BITS does not match the device's __NVIC_PRIO_BITS");

__attribute__((naked))void xrt_thread_store_context(void);
__attribute__((naked))void xrt_load_context(void);
void xrt_port_set_os_priority_order(void);

__STATIC_FORCEINLINE uint32_t xrt_port_enter_critical(void){
    uint32_t old = __get_BASEPRI();
    __set_BASEPRI(KERNEL_BASEPRI);
    return old;
}

__STATIC_FORCEINLINE void xrt_port_exit_critical(uint32_t old){
    __set_BASEPRI(old);
}

__STATIC_FORCEINLINE void xrt_port_request_context_switch(void){
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

#endif /* INC_XRT_PORT_CM4_H_ */
