idf_component_register(SRCS "ext_flash_fatfs_example_main.c")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
