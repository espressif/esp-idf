idf_component_register(SRCS "mesh_light.c"
                            "mesh_main.c"
                    INCLUDE_DIRS "." "include")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
