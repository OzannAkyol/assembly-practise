/**
 * @file    xrt_port.h
 * @brief   Selects the port implementation for the target being built.
 *
 */

#ifndef INC_XRT_PORT_H_
#define INC_XRT_PORT_H_

#if defined(XRT_PORT_CM4)
#include "xrt_port_cm4.h"
#elif defined(XRT_PORT_HOST)
#include "xrt_port_host.h"
#elif defined(XRT_PORT_CM7)
#include "xrt_port_cm7.h"
#else
#error "No port selected: define XRT_PORT_CM4, XRT_PORT_HOST or XRT_PORT_CM7"
#endif

#endif /* INC_XRT_PORT_H_ */