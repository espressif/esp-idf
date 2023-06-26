# Xtensa Component Architecture
#
# The ESP-IDF Xtesna component contains two major features:
# - The Xtensa HAL
# - The Xtensa RTOS porting layer
#
# The Xtensa HAL provides various macros/functions regarding the Xtensa processor's configuration and extensions (see
# "sys_sw_rm.pdf 3.1"). The Xtensa HAL...
#   - is packaged as a library ("libxt_hal.a") in the ESP-IDF Xtensa component
#   - expects `#include <xtensa/...h>` as the include path to the Xtensa HAL's headers
#
# The Xtensa RTOS Porting Layer is a set of helper functions and interrupt vectors that act as a basis of an RTOS port.
# The porting layer sources files are OS agnostic, thus are common across multiple Xtensa RTOS ports (e.g., FreeRTOS,
# ThreadX). The Xtensa RTOS Porting Layer...
#   - interfaces with an RTOS port via the "xtensa_rtos.h" header provided by the RTOS port
#   - expected `#include <...h>` as the incldue path to the porting layer's headers

idf_build_get_property(target IDF_TARGET)
idf_build_get_property(arch IDF_TARGET_ARCH)

if(NOT "${arch}" STREQUAL "xtensa")
    return()
endif()

set(include_dirs
    "${target}/include"         # - Add include path for target specific Xtensa HAL headers (`#include <xtensa/...h>`)
    "include"                   # - Add include path for...
                                #       - common Xtensa HAL headers (`#include <xtensa/...h>`)
                                #       - Xtensa RTOS porting layer headers (`#include <...h>`)
    "deprecated_include")       # - For deprecated include paths (see IDF-7230)

set(srcs
    "eri.c"
    "xt_trax.c")

# Minor optimization. The following sources are excluded from the bootloader as they are not required by the bootloader.
#
# - ROM provides a copy of basic exception vectors (e.g., _UserExceptionVector and _WindowOverflow8)
# - The bootloader doesn't register any interrupts, thus...
#   - the "xtensa_api.h" isn't used
#   - the "xtensa_context.h" functions aren't used as there are no interrupts to trigger a context switch
if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs
        "xtensa_context.S"
        "xtensa_intr_asm.S"
        "xtensa_intr.c"
        "xtensa_vectors.S")

    if(CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY)
        list(APPEND srcs "xtensa_loadstore_handler.S")
    endif()

    if(NOT CONFIG_FREERTOS_PORT)
        # No RTOS provided. Use default bare metal stubs (to pass G0 build test)
        list(APPEND srcs
             "baremetal/xtensa_rtos_bm.S")
        list(APPEND include_dirs
             "baremetal")               # For "...h"
    endif()
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${include_dirs}
                    LDFRAGMENTS linker.lf)

target_link_libraries(${COMPONENT_LIB} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${target}/libxt_hal.a")
