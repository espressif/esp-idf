set(srcs "highint_hdl.S"
         "clk.c"
         "reset_reason.c"
         "system_internal.c"
         "cache_err_int.c"
         "../../arch/xtensa/panic_arch.c"
         "../../arch/xtensa/panic_handler_asm.S"
         "../../arch/xtensa/expression_with_stack.c"
         "../../arch/xtensa/expression_with_stack_asm.S"
         "../../arch/xtensa/debug_helpers.c"
         "../../arch/xtensa/debug_helpers_asm.S"
         "../../arch/xtensa/debug_stubs.c"
         "../../arch/xtensa/trax.c"
         )
add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})

#ld_include_highint_hdl is added as an undefined symbol because otherwise the
#linker will ignore panic_highint_hdl.S as it has no other files depending on any
#symbols in it.
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u ld_include_highint_hdl")
