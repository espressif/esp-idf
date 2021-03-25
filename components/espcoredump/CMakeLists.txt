set(srcs "src/core_dump_common.c"
         "src/core_dump_checksum.c"
         "src/core_dump_flash.c"
         "src/core_dump_uart.c"
         "src/core_dump_elf.c"
         "src/core_dump_binary.c")

set(priv_includes "include_core_dump")

idf_build_get_property(target IDF_TARGET)

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
  set(srcs ${srcs} "src/port/xtensa/core_dump_port.c")
  set(priv_includes ${priv_includes} "include_core_dump/port/xtensa")
elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
  set(srcs ${srcs} "src/port/riscv/core_dump_port.c")
  set(priv_includes ${priv_includes} "include_core_dump/port/riscv")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    PRIV_INCLUDE_DIRS ${priv_includes}
                    LDFRAGMENTS linker.lf
                    PRIV_REQUIRES spi_flash app_update mbedtls esp_rom soc)
