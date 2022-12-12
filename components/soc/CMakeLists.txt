idf_build_get_property(target IDF_TARGET)

# On Linux the soc component is a simple wrapper, without much functionality
if(NOT ${target} STREQUAL "linux")
     set(srcs "lldesc.c"
              "dport_access_common.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       LDFRAGMENTS "linker.lf")

idf_build_get_property(target IDF_TARGET)
add_subdirectory(${target})

if(NOT CONFIG_IDF_TARGET_LINUX)
     target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.peripherals.ld")
endif()
