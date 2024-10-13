idf_build_get_property(target IDF_TARGET)

set(srcs)
set(public_include "include")
if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "src/mcpwm_cap.c"
                     "src/mcpwm_cmpr.c"
                     "src/mcpwm_com.c"
                     "src/mcpwm_fault.c"
                     "src/mcpwm_gen.c"
                     "src/mcpwm_oper.c"
                     "src/mcpwm_sync.c"
                     "src/mcpwm_timer.c")
    if(CONFIG_SOC_MCPWM_SUPPORT_ETM)
        list(APPEND srcs "src/mcpwm_etm.c")
    endif()
endif()

if(${target} STREQUAL "linux")
    set(priv_requires "")
else()
    set(priv_requires esp_pm esp_driver_gpio)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS "linker.lf"
                      )
