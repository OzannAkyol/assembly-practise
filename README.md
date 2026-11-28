# xrt

A small preemptive kernel for ARM Cortex-M. Static allocation throughout; the
kernel never calls `malloc`.

The kernel knows nothing about the architecture it runs on. Nothing under
`xrt/src` touches a CMSIS header, an SCB register, `BASEPRI` or inline
assembly; all of that sits in one port directory. Two boards are in the tree so
this is checkable rather than claimed: an STM32F407 and an STM32H7B3 build from
the same kernel sources and run the same application file. A host port compiles
the kernel for a desktop with no hardware involved.

What is in it:

- Fixed-priority preemptive scheduling, with a kernel-owned idle thread
- `SVC` syscall dispatch, `PendSV` context switch, `SysTick` tick
- `BASEPRI` critical sections with a configurable syscall priority threshold
- Mutexes with transitive priority inheritance and stepped priority restore
- Counting semaphores, with an ISR-safe release
- A queue built on a pair of semaphores
- Tick-based thread delay
- FPU support, including lazy context stacking

## Targets

| Port | Cores | Run on |
|---|---|---|
| `armv7m` | Cortex-M4F, Cortex-M7 | STM32F407VG, STM32H7B3II |
| `host` | — | x86-64 Linux |

## Building

`arm-none-eabi-gcc` has to be on `PATH` for the MCU targets. Each example picks
its own toolchain file.

```sh
cmake -S xrt -B build/host -DXRT_PORT=host      # kernel only
cmake -S example/stm32f407 -B build/f407        # STM32F407
cmake -S example/stm32h7b3 -B build/h7b3        # STM32H7B3

cmake --build build/f407
```

Each MCU build leaves an `.elf`, a `.hex` and a `.bin` and prints the section
sizes. The example runs a priority inheritance scenario over two mutexes.

```
xrt/inc           public headers, and xrt_config.h
xrt/src           scheduler, threads, mutex, semaphore, queue, lists
xrt/port          port selector
xrt/port/armv7m   exception handlers, context switch, BASEPRI
xrt/port/host     null port
example/          one directory per board
cmake/            toolchain files
```

The kernel reaches the device header without naming it. The application defines
an `xrt_device` interface target, sets `XRT_PORT_DEVICE_HEADER` on it, and the
port includes whatever that points at.

## Configuration

`xrt/inc/xrt_config.h` holds the device's priority bit count, the interrupt
priority above which kernel API calls are illegal, the priorities of the three
exceptions the kernel owns, the idle stack size and the thread limit. The
values are checked against each other with `_Static_assert` in the same file,
so a bad combination fails to compile.

`CP10` and `CP11` have to be enabled before `xrt_kernel_start`. CubeMX does
this in `SystemInit` when the project is built with the hard float ABI, which
both examples are.

## Limitations

Mutexes are not recursive. No deadlock detection, no stack overflow detection,
no software timer service, and no thread deletion, so a thread entry function
must never return. The port needs a CMSIS device header rather than standing on
CMSIS-Core alone. `xrt_config.h` is shared by every target in a build tree, so
devices with different priority bit counts need a copy each. The host port can
compile and link-check the kernel but cannot run threads. No test suite yet.