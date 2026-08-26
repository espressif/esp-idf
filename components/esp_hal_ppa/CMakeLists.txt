idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

# Add target-specific include directory if it exists
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

if(CONFIG_SOC_PPA_SUPPORTED)
    list(APPEND srcs "ppa_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
