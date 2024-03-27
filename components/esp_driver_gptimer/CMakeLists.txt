set(srcs)
set(public_include "include")
if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "src/gptimer.c"
                     "src/gptimer_priv.c")
endif()

if(CONFIG_SOC_TIMER_SUPPORT_ETM)
    list(APPEND srcs "src/gptimer_etm.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "esp_pm"
                       LDFRAGMENTS "linker.lf"
                      )
