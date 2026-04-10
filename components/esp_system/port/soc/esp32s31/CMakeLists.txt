set(srcs "clk.c"
         "reset_reason.c"
         "system_internal.c"
         "cache_err_int.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})
