/**
 * @file    xrt_port.h
 * @brief   Selects the port implementation for the target being built.
 *
 */

#ifndef INC_XRT_PORT_H_
#define INC_XRT_PORT_H_

#if defined(XRT_PORT_ARMV7M)
#include "xrt_port_armv7m.h"
#elif defined(XRT_PORT_HOST)
#include "xrt_port_host.h"
#else
#error "No port selected: define XRT_PORT_ARMV7M or XRT_PORT_HOST"
#endif

#endif /* INC_XRT_PORT_H_ */