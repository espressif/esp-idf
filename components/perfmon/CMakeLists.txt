idf_build_get_property(arch IDF_TARGET_ARCH)

if(NOT "${arch}" STREQUAL "xtensa")
    return()
endif()

set(xtensa_perfmon_srcs "xtensa_perfmon_access.c"
                        "xtensa_perfmon_apis.c"
                        "xtensa_perfmon_masks.c")

idf_component_register(SRCS "${xtensa_perfmon_srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES "xtensa")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
