/*
 * xrt_port_host.h
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_PORT_HOST_H_
#define INC_XRT_PORT_HOST_H_
#include <stdint.h>

/*
 * The host port has no syscall mechanism: there is no exception to raise and
 * no handler mode to switch into. The wrappers compile and do nothing, which
 * is enough to prove the kernel and the primitives depend on the port alone.
 */
#define XRT_SYSCALL_WITH_ARG(svc_number, arg)   ((void)(arg))
#define XRT_SYSCALL_NO_ARG(svc_number)          ((void)0)

void xrt_port_set_os_priority_order(void);
uint32_t xrt_port_enter_critical(void);
void xrt_port_exit_critical(uint32_t old);
void xrt_port_request_context_switch(void);

#endif /* INC_XRT_PORT_CM4_H_ */
