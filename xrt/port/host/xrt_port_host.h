/**
 * @file    xrt_port_host.h
 * @brief   Host port: satisfies the port interface so the portable layer can
 *          be compiled off target.
 *
 * It schedules nothing and runs no threads. Its only purpose is to give the
 * kernel and the primitives something to build against without CMSIS or a
 * device header, so that the claim they depend on the port alone is checked
 * on every build rather than asserted.
 *
 * How little this file contains is the measure of how narrow the port
 * interface is.
 */

#ifndef INC_XRT_PORT_HOST_H_
#define INC_XRT_PORT_HOST_H_
#include <stdint.h>

/*
 * The host has no syscall mechanism: there is no exception to raise and no
 * handler mode to switch into. The wrappers expand to nothing rather than
 * calling the dispatcher directly, which would be a different thing from
 * raising a synchronous software interrupt.
 */
#define XRT_SYSCALL_WITH_ARG(svc_number, arg)   ((void)(arg))
#define XRT_SYSCALL_NO_ARG(svc_number)          ((void)0)

/** @brief Counts calls; there are no exception priorities to set. */
void xrt_port_set_os_priority_order(void);

/**
 * @brief Returns zero; there are no interrupts to mask.
 *
 * @return Always zero, kept for the signature the kernel calls through.
 */
uint32_t xrt_port_enter_critical(void);

/**
 * @brief Does nothing.
 *
 * @param old Ignored.
 */
void xrt_port_exit_critical(uint32_t old);

/** @brief Counts the request; no switch is performed. */
void xrt_port_request_context_switch(void);

#endif /* INC_XRT_PORT_HOST_H_ */