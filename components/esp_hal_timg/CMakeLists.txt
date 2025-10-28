idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include" "${target}/include")

if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "timer_hal.c" "${target}/timer_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES soc hal)
