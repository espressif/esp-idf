idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "src/gdbstub.c"
         "src/gdbstub_transport.c"
         "src/packet.c")

set(includes "include")
set(priv_includes "private_include")

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    list(APPEND srcs "src/port/xtensa/gdbstub_xtensa.c"
                     "src/port/xtensa/gdbstub-entry.S"
                     "src/port/xtensa/xt_debugexception.S")
    list(APPEND priv_includes "src/port/xtensa/include")
elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND srcs "src/port/riscv/gdbstub_riscv.c"
                     "src/port/riscv/rv_decode.c")
    list(APPEND priv_includes "src/port/riscv/include")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       LDFRAGMENTS "linker.lf"
                       REQUIRES "freertos"
                       PRIV_REQUIRES "soc" "esp_rom" "esp_system")
