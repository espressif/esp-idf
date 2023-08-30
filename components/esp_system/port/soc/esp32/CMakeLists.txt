set(srcs "highint_hdl.S"
         "clk.c"
         "reset_reason.c"
         "system_internal.c"
         "cache_err_int.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})

#ld_include_highint_hdl is added as an undefined symbol because otherwise the
#linker will ignore panic_highint_hdl.S as it has no other files depending on any
#symbols in it.
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u ld_include_highint_hdl")
