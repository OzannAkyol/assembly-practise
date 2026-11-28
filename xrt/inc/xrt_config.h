/**
 * @file    xrt_config.h
 * @brief   Compile-time configuration for the xrt kernel and its port.
 *
 * This is the only file an integrator is expected to edit. The values below
 * are checked against each other at the bottom of the file, so a combination
 * that would leave the kernel open to preemption inside a critical section
 * fails to compile rather than failing on the target.
 *
 * The file is shared by every target in a build tree. A project that mixes
 * devices implementing different numbers of priority bits needs one copy of
 * this file per target.
 */

#ifndef INC_XRT_CONFIG_H_
#define INC_XRT_CONFIG_H_

/* ---- Port: interrupt priorities --------------------------------------- */

/**
 * @brief Number of priority bits the device implements.
 *
 * Cortex-M allows 3 to 8. Both STM32F4 and STM32H7 implement 4. The port
 * asserts this against the device's own __NVIC_PRIO_BITS, so a wrong value
 * is caught at compile time.
 */
#define XRT_PORT_PRIO_BITS                      (4u)

/**
 * @brief Lowest interrupt priority from which kernel API calls are legal.
 *
 * Calls are only legal from interrupts at this priority or lower, meaning
 * numerically greater or equal. A higher priority ISR is not masked by the
 * critical section, so calling xrt_* from one corrupts the kernel lists.
 */
#define XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY   (5u)

/** @brief Priority of the SVCall exception, which dispatches syscalls. */
#define XRT_PORT_SVCALL_PRIORITY                (15u)

/** @brief Priority of the PendSV exception, which performs context switches. */
#define XRT_PORT_PENDSV_PRIORITY                (15u)

/** @brief Priority of the SysTick exception, which drives the kernel tick. */
#define XRT_PORT_SYSTICK_PRIORITY               (14u)

/* ---- Kernel ----------------------------------------------------------- */

/** @brief Stack size of the kernel's idle thread, in 32-bit words. */
#define XRT_IDLE_THREAD_STACK_SIZE_IN_WORD      (64u)

/** @brief Upper bound on the number of threads, the idle thread included. */
#define XRT_MAX_NUM_OF_THREAD                   (8u)

/* ---- Invariants ------------------------------------------------------- */

/*
 * A critical section masks everything at or below the syscall threshold, so
 * the kernel's own exceptions must sit below it; otherwise they would preempt
 * a critical section and reach the lists while they are inconsistent.
 */
_Static_assert(XRT_PORT_SVCALL_PRIORITY  > XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY,
               "SVCall must be lower priority than the XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY threshold");
_Static_assert(XRT_PORT_PENDSV_PRIORITY  > XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY,
               "PendSV must be lower priority than the XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY threshold");
_Static_assert(XRT_PORT_SYSTICK_PRIORITY > XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY,
               "SysTick must be lower priority than the XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY threshold");

_Static_assert(XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY < (1u << XRT_PORT_PRIO_BITS),
               "XRT_MAX_KERNEL_API_INTERRUPT_PRIORITY is out of range");

/*
 * PendSV must never preempt SysTick, or a tick could be interrupted halfway
 * through the stopped-list scan.
 */
_Static_assert(XRT_PORT_PENDSV_PRIORITY >= XRT_PORT_SYSTICK_PRIORITY,
               "PendSV must not outrank SysTick");

_Static_assert(XRT_PORT_SVCALL_PRIORITY < (1u << XRT_PORT_PRIO_BITS),
               "XRT_PORT_SVCALL_PRIORITY is out of range: valid values are "
               "0 to (1 << XRT_PORT_PRIO_BITS) - 1");

_Static_assert(XRT_PORT_PENDSV_PRIORITY < (1u << XRT_PORT_PRIO_BITS),
               "XRT_PORT_PENDSV_PRIORITY is out of range: valid values are "
               "0 to (1 << XRT_PORT_PRIO_BITS) - 1");

_Static_assert(XRT_PORT_SYSTICK_PRIORITY < (1u << XRT_PORT_PRIO_BITS),
               "XRT_PORT_SYSTICK_PRIORITY is out of range: valid values are "
               "0 to (1 << XRT_PORT_PRIO_BITS) - 1");

#endif /* INC_XRT_CONFIG_H_ */