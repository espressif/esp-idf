idf_build_get_property(target IDF_TARGET)

idf_component_register(SRCS "src/cpu_util.c"
                            "src/memory_layout_utils.c"
                            "src/lldesc.c"
                            "src/soc_include_legacy_warn.c"
                            "src/compare_set.c"
                       REQUIRES hal #cpu.h directly includes HAL header
                       PRIV_REQUIRES ${target}
                       LDFRAGMENTS linker.lf)


# Since there can be chip-specific HAL headers which can include the common
# HAL header via include_next, process the build scripts here first so that
# include directories appear first in the compile command.
add_subdirectory(src/${target})
target_include_directories(${COMPONENT_LIB} PUBLIC include)
target_link_libraries(${COMPONENT_LIB} PRIVATE idf::hal)

add_subdirectory(soc)
target_link_libraries(${COMPONENT_LIB} PUBLIC "soc_${target}")
