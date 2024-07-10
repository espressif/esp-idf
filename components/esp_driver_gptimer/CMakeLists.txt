idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(public_include "include")
if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "src/gptimer.c"
                     "src/gptimer_common.c")
endif()

if(CONFIG_SOC_TIMER_SUPPORT_ETM)
    list(APPEND srcs "src/gptimer_etm.c")
endif()

set(requires esp_pm)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES ${requires}
                       LDFRAGMENTS "linker.lf"
                      )
