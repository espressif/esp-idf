idf_component_register(SRCS "compare_set.c"
                            "cpu_util.c"
                       INCLUDE_DIRS include
                       REQUIRES soc
                       LDFRAGMENTS linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})
