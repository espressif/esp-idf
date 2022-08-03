idf_component_register(SRCS "mcpwm_bldc_hall_control_example_main.c"
                       INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
