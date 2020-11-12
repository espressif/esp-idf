target_include_directories(${COMPONENT_LIB} PRIVATE include)

set(srcs "cpu_start.c" "panic_handler.c")
add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})

idf_build_get_property(target IDF_TARGET)
add_subdirectory(soc/${target})
