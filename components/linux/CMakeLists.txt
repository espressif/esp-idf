idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "linux")
    return()
endif()

idf_component_register(INCLUDE_DIRS include
                       REQUIRED_IDF_TARGETS linux)
