idf_component_register(SRCS "matrix_keyboard_example_main.c"
                       INCLUDE_DIRS "")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
