idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "src/cpu_util.c"
                            "src/memory_layout_utils.c"
                            "src/lldesc.c"
                            "src/soc_include_legacy_warn.c"
                            "src/compare_set.c"
                       REQUIRES hal #cpu.h directly includes HAL header
                       INCLUDE_DIRS include
                       PRIV_REQUIRES ${target}
                       LDFRAGMENTS linker.lf)

add_subdirectory(soc)

add_subdirectory(src/${target})
target_link_libraries(${COMPONENT_LIB} PUBLIC "soc_${target}")
