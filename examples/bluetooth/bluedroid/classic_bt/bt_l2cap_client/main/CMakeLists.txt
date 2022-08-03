idf_component_register(SRCS "main.c"
                            "bt_app_core.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
