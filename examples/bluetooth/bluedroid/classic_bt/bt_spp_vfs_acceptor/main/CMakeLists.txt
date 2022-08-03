idf_component_register(SRCS "main.c"
                            "spp_task.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
