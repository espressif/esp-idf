idf_component_register(SRCS "cmd_wifi.c"
                            "iperf_example_main.c"
                    INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
