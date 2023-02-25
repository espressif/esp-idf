idf_component_register(SRCS "lldesc.c"
                            "dport_access_common.c"
                       INCLUDE_DIRS include
                       LDFRAGMENTS "linker.lf")

idf_build_get_property(target IDF_TARGET)
add_subdirectory(${target})

# For an embedded system, the MMU page size should always be defined statically
# For IDF, we define it according to the Flash size that user selects
# Replace this value in an adaptive way, if Kconfig isn't available on your platform
target_compile_definitions(${COMPONENT_LIB} INTERFACE SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE)

target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.peripherals.ld")
