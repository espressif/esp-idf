idf_build_get_property(soc_name IDF_TARGET)
get_filename_component(soc_test "${CMAKE_CURRENT_SOURCE_DIR}/../${soc_name}/test" ABSOLUTE)
if(EXISTS "${soc_test}")
    set(COMPONENT_SRCS "${soc_test}")
    set(COMPONENT_ADD_INCLUDEDIRS "${soc_test}")
endif()

set(COMPONENT_REQUIRES unity test_utils)

register_component()
