idf_component_register(SRCS "mcpwm_capture_hc_sr04.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
