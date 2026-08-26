idf_component_register(SRCS test_main.c
                       REQUIRES esp_partition)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
