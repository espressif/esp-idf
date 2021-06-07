idf_build_get_property(target IDF_TARGET)

if(NOT "${target}" STREQUAL "esp32c2") # TODO: IDF-4135
idf_component_register(SRCS "src/gdbstub.c" "src/packet.c"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS "private_include"
                       LDFRAGMENTS "linker.lf"
                       REQUIRES "freertos"
                       PRIV_REQUIRES "soc" "esp_rom" "esp_system")
endif()

if(CONFIG_IDF_TARGET_ARCH_XTENSA)
    target_include_directories(${COMPONENT_LIB} PUBLIC "xtensa" "${target}")
    target_sources(${COMPONENT_LIB} PRIVATE "xtensa/gdbstub_xtensa.c"
                                            "xtensa/gdbstub-entry.S"
                                            "xtensa/xt_debugexception.S"
                                            "esp_common/gdbstub_common.c")

elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
    target_include_directories(${COMPONENT_LIB} PUBLIC "riscv" "${target}")
    target_sources(${COMPONENT_LIB} PRIVATE "riscv/gdbstub_riscv.c"
    "${target}/gdbstub_${target}.c")
endif()
