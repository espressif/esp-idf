idf_component_register(SRCS "touch_button_example_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES touch_element)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
