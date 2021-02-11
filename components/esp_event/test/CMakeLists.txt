idf_component_register(SRC_DIRS "."
                       PRIV_INCLUDE_DIRS . ../private_include
                       PRIV_REQUIRES cmock test_utils esp_event driver)

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    # Temporary workaround for a linker issue on RISC-V that should be resolved in binutils 2.35 (internal ref: GCC-101)
    target_compile_options(${COMPONENT_LIB} PRIVATE -mno-relax)
endif()
