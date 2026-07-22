idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")

if(CONFIG_SOC_RISCV_TRACE_SUPPORTED)
    list(APPEND srcs "riscv_trace_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal
                       PRIV_REQUIRES esp_rom)
