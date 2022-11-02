set(srcs "soft_spi.c")

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND srcs "riscv/soft_spi.S")
elseif(CONFIG_IDF_TARGET_ARCH_XTENSA)
    message(FATAL_ERROR "Xtensa targets not supported yet")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES driver
                       LDFRAGMENTS linker.lf)
