set(srcs "src/bdc_motor.c")

if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "src/bdc_motor_mcpwm_impl.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include" "interface"
                       PRIV_REQUIRES "driver")
