idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "src/gptimer.c"
                     "src/gptimer_priv.c")
endif()

if(CONFIG_SOC_TIMER_SUPPORT_ETM)
    list(APPEND srcs "src/gptimer_etm.c")
endif()

if(${target} STREQUAL "linux")
    set(requires "")
else()
    set(requires esp_pm)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       LDFRAGMENTS "linker.lf"
                      )
