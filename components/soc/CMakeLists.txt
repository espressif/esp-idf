idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "lldesc.c"
                            "soc_include_legacy_warn.c"
                            "memory_layout_utils.c"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES hal # [refactor-todo] soc dependency on hal for rtc sources
                       LDFRAGMENTS linker.lf)

add_subdirectory(${target})