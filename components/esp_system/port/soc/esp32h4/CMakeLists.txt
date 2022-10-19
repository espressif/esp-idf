set(srcs "clk.c"
         "reset_reason.c"
         "system_internal.c"
         "cache_err_int.c"
         "apb_backup_dma.c"
         "../../arch/riscv/expression_with_stack.c"
         "../../arch/riscv/expression_with_stack_asm.S"
         "../../arch/riscv/panic_arch.c"
         "../../arch/riscv/debug_stubs.c")

add_prefix(srcs "${CMAKE_CURRENT_LIST_DIR}/" ${srcs})

target_sources(${COMPONENT_LIB} PRIVATE ${srcs})
