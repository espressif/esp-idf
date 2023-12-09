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

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "esp_pm" "esp_driver_gpio"
                       LDFRAGMENTS "linker.lf"
                      )
