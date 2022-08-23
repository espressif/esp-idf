idf_component_register(SRCS "lldesc.c"
                            "dport_access_common.c"
                       INCLUDE_DIRS include
                       LDFRAGMENTS "linker.lf")

idf_build_get_property(target IDF_TARGET)
add_subdirectory(${target})

target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.peripherals.ld")
