/*
 * xrt_port.h
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */

#ifndef INC_XRT_PORT_H_
#define INC_XRT_PORT_H_

#define CORTEX_M4_PORT
#ifdef CORTEX_M4_PORT
#include "xrt_port_cm4.h"
#else
#error No port definitions
#endif


#endif /* INC_XRT_PORT_H_ */
