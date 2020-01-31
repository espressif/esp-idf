set(srcs "dport_panic_highint_hdl.S" "clk.c" "reset_reason.c")
add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})
