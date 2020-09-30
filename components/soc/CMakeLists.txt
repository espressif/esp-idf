idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "src/lldesc.c"
                            "src/soc_include_legacy_warn.c"
                            "src/memory_layout_utils.c"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES hal # [refactor-todo] soc dependency on hal for rtc sources
                       LDFRAGMENTS linker.lf)

add_subdirectory(soc)

add_subdirectory(src/${target})
target_link_libraries(${COMPONENT_LIB} PUBLIC "soc_${target}")
