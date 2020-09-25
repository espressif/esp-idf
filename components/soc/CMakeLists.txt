idf_component_register(SRCS "lldesc.c"
                            "soc_include_legacy_warn.c"
                            "memory_layout_utils.c"
                       INCLUDE_DIRS include
                       LDFRAGMENTS linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(${target})
