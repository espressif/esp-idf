if(CONFIG_IDF_TARGET_ARCH_RISCV AND NOT DEFINED CONFIG_ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY)
    message(WARNING
        "You probably added a new chip support. Please do the next steps:\n"
        "  1) Check if ROM functions implementation is optimized on misaligned memory operations.\n"
        "  2) Define ESP_ROM_HAS_SUBOPTIMAL_NEWLIB_ON_MISALIGNED_MEMORY to esp_rom_caps.h. If it is zero:\n"
        "    2.1) Move some functions out from *.rom.libc.ld file (see *.rom.libc-suboptimal_for_misaligned_mem.ld).\n"
        "Find a related test in the newlib component to use as a reference.")
endif()

set(srcs "test_app_main.c"
         "test_libgcc.c"
         "test_printf.c")

if(CONFIG_IDF_ENV_FPGA)
    list(APPEND srcs "test_miniz.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity
                       WHOLE_ARCHIVE)
