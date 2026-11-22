# Cortex-M7 toolchain: generic ARM bare-metal GCC + core-specific flags
include(${CMAKE_CURRENT_LIST_DIR}/arm-none-eabi.cmake)

set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfloat-abi=hard")

set(CMAKE_C_FLAGS_INIT   "${MCU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")