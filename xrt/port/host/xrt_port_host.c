/*
 * xrt_port_host.c
 *
 *  Created on: Sep 2, 2026
 *      Author: ozi
 */
#include "xrt_port_host.h"

uint32_t port_set_os_priority_order = 0;
uint32_t port_request_context_switch = 0;


void xrt_port_set_os_priority_order(void){
    port_set_os_priority_order++;
}

uint32_t xrt_port_enter_critical(void){
    uint32_t old = 0;
    return old;
}

void xrt_port_exit_critical(uint32_t old){
    return;
}

void xrt_port_request_context_switch(void){
    port_request_context_switch++;
}
