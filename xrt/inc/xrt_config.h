/*
 * xrt_config.h
 *
 * Compile-time configuration for the xrt kernel and its port.
 * This is the only file an integrator is expected to edit.
 */

#ifndef INC_XRT_CONFIG_H_
#define INC_XRT_CONFIG_H_

/* ---- Port: interrupt priorities --------------------------------------- */

/*
 * Number of priority bits the device implements. Cortex-M allows 3 to 8;
 * STM32F4 implements 4.
 */
#define XRT_PORT_PRIO_BITS                      (4u)

/*
 * Kernel API calls are only legal from interrupts at this priority or lower
 * (numerically greater or equal). A higher priority ISR is NOT masked by the
 * critical section, so calling xrt_* from one corrupts the kernel lists.
 */
#define XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY      (5u)

/* Priorities of the exceptions the kernel owns. */
#define XRT_PORT_SVCALL_PRIORITY                (15u)
#define XRT_PORT_PENDSV_PRIORITY                (15u)
#define XRT_PORT_SYSTICK_PRIORITY               (14u)

/* ---- Kernel ----------------------------------------------------------- */

#define XRT_IDLE_THREAD_STACK_SIZE_IN_WORD      (64u)

/* ---- Invariants ------------------------------------------------------- */

/*
 * A critical section masks everything at or below the syscall threshold, so
 * the kernel's own exceptions must sit below it; otherwise they would preempt
 * a critical section and reach the lists while they are inconsistent.
 */
_Static_assert(XRT_PORT_SVCALL_PRIORITY  > XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY, "SVCall must be lower priority than the XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY threshold");
_Static_assert(XRT_PORT_PENDSV_PRIORITY  > XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY, "PendSV must be lower priority than the XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY threshold");
_Static_assert(XRT_PORT_SYSTICK_PRIORITY > XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY, "SysTick must be lower priority than the XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY threshold");

_Static_assert(XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY < (1 << XRT_PORT_PRIO_BITS), "XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY is out of range");
/*
 * PendSV must never preempt SysTick, or a tick could be interrupted halfway
 * through the stopped-list scan.
 */
_Static_assert(XRT_PORT_PENDSV_PRIORITY >= XRT_PORT_SYSTICK_PRIORITY,
               "PendSV must not outrank SysTick");

_Static_assert(XRT_PORT_SVCALL_PRIORITY < (1u << XRT_PORT_PRIO_BITS), "XRT_PORT_SVCALL_PRIORITY is out of range: valid values are "
               	   	   	   	   	   	   	   	   	   	   	   	   	   	  "0 to (1 << XRT_PORT_PRIO_BITS) - 1");

_Static_assert(XRT_PORT_PENDSV_PRIORITY < (1u << XRT_PORT_PRIO_BITS), "XRT_PORT_PENDSV_PRIORITY is out of range: valid values are "
               	   	   	   	   	   	   	   	   	   	   	   	   	   	  "0 to (1 << XRT_PORT_PRIO_BITS) - 1");

_Static_assert(XRT_PORT_SYSTICK_PRIORITY < (1u << XRT_PORT_PRIO_BITS), "XRT_PORT_SYSTICK_PRIORITY is out of range: valid values are "
               	   	   	   	   	   	   	   	   	   	   	   	   	   	   "0 to (1 << XRT_PORT_PRIO_BITS) - 1");

#endif /* INC_XRT_CONFIG_H_ */
