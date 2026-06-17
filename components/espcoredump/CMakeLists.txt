idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")
set(includes "")
set(priv_includes "")
set(priv_requires
  esp_partition spi_flash bootloader_support mbedtls esp_rom soc esp_system esp_driver_gpio esp_app_format
  esp_hal_security efuse)

if(CONFIG_ESP_COREDUMP_ENABLE)
  list(APPEND srcs "src/core_dump_init.c"
          "src/core_dump_common.c"
          "src/core_dump_elf.c"
          "src/core_dump_sha.c")

  if(CONFIG_ESP_COREDUMP_ENABLE_TO_UART)
    list(APPEND srcs "src/core_dump_uart.c")
  elseif(CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH)
    list(APPEND srcs "src/core_dump_flash.c")
  endif()

  list(APPEND includes "include")
  list(APPEND priv_includes "include_core_dump")

  if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND srcs "src/port/xtensa/core_dump_port.c"
                     "src/port/xtensa/core_dump_stack_switch.S")
    list(APPEND includes "include/port/xtensa")
  elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND srcs "src/port/riscv/core_dump_port.c")
    list(APPEND includes "include/port/riscv")
  endif()

  idf_component_register(SRCS ${srcs}
                        INCLUDE_DIRS ${includes}
                        PRIV_INCLUDE_DIRS ${priv_includes}
                        LDFRAGMENTS linker.lf
                        PRIV_REQUIRES ${priv_requires}
                        )

# make sure 'core_dump_init' object file is considered by the linker
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_system_include_coredump_init")
else()
  idf_component_register(PRIV_REQUIRES ${priv_requires})
endif()
