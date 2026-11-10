cmake_minimum_required(VERSION 3.21)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/cortex-m4.cmake)
endif()

project(xrt_stm32f407 LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)

# Build strategy flags: apply to every target in this directory and below (incl. xrt)
add_compile_options(-ffunction-sections -fdata-sections)

file(GLOB HAL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Src/*.c)

add_executable(stm32f407
    Core/Src/main.c
    Core/Src/stm32f4xx_it.c
    Core/Src/stm32f4xx_hal_msp.c
    Core/Src/system_stm32f4xx.c
    Core/Src/syscalls.c
    Core/Src/sysmem.c
    Core/Src/gpio.c
    Core/Src/xrt_app.c
    Core/Startup/startup_stm32f407vgtx.s
    ${HAL_SOURCES}
)

# Single source of truth for device info, shared by the application and xrt
add_library(xrt_device INTERFACE)
target_include_directories(xrt_device INTERFACE
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
)
target_compile_definitions(xrt_device INTERFACE
    STM32F407xx
    XRT_PORT_DEVICE_HEADER="stm32f4xx.h"
)

set(XRT_PORT "cm4" CACHE STRING "")
add_subdirectory(../../xrt xrt)

target_link_libraries(stm32f407 PRIVATE xrt_device xrt)

target_compile_definitions(stm32f407 PRIVATE USE_HAL_DRIVER USE_PWR_DIRECT_SMPS_SUPPLY)

target_include_directories(stm32f407 PRIVATE
    Core/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc
)

target_compile_options(stm32f407 PRIVATE -Wall)

target_link_options(stm32f407 PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F407VGTX_FLASH.ld
    --specs=nano.specs
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/stm32f407.map
)

set_target_properties(stm32f407 PROPERTIES SUFFIX ".elf")

add_custom_command(TARGET stm32f407 POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:stm32f407> ${CMAKE_CURRENT_BINARY_DIR}/stm32f407.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:stm32f407> ${CMAKE_CURRENT_BINARY_DIR}/stm32f407.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:stm32f407>
    COMMENT "Generating hex/bin"
)