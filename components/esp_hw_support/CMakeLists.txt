idf_build_get_property(target IDF_TARGET)

set(requires soc)
if(${target} STREQUAL "esp32")
     list(APPEND requires efuse)
endif()

idf_component_register(SRCS "compare_set.c"
                            "cpu_util.c"
                       INCLUDE_DIRS include
                       REQUIRES ${requires}
                       LDFRAGMENTS linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})
