idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

# Target-specific include directory if present
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

list(APPEND srcs "${target}/clk_tree_hal.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES soc hal)
