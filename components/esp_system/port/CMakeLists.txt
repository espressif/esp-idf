target_include_directories(${COMPONENT_LIB} PRIVATE include)

set(srcs "panic_handler.c" "panic_handler_asm.S")
add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})

idf_build_get_property(target IDF_TARGET)
add_subdirectory(${target})
