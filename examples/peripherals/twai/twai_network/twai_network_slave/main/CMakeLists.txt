idf_component_register(SRCS "twai_network_example_slave_main.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
