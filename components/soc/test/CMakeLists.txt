idf_build_get_property(soc_name IDF_TARGET)

get_filename_component(soc_test "${CMAKE_CURRENT_SOURCE_DIR}/../${soc_name}/test" ABSOLUTE)
if(EXISTS "${soc_test}")
    set(src_dirs "${soc_test}")
    set(include_dirs "${soc_test}")
endif()

idf_component_register(SRC_DIRS "${src_dirs}"
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES unity test_utils)
