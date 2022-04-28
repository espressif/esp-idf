set(INCLUDE_FILES "include" . include PUBLIC soc)

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND INCLUDE_FILES "include/riscv")
endif()

target_include_directories(${COMPONENT_LIB} PRIVATE ${INCLUDE_FILES} include/private)

set(srcs "cpu_start.c" "panic_handler.c" "brownout.c")
add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})

idf_build_get_property(target IDF_TARGET)
add_subdirectory(soc/${target})
