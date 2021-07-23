idf_component_register(SRCS "lldesc.c"
                            "soc_include_legacy_warn.c"
                       INCLUDE_DIRS include
                       LDFRAGMENTS "linker.lf")

idf_build_get_property(target IDF_TARGET)
add_subdirectory(${target})

target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.peripherals.ld")
