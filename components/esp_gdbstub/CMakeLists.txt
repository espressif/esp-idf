idf_build_get_property(target IDF_TARGET)

set(esp_gdbstub_srcs "src/gdbstub.c"
                     "src/packet.c"
                     "${target}/gdbstub_${target}.c"
                     "xtensa/gdbstub_xtensa.c")

idf_component_register(SRCS "${esp_gdbstub_srcs}"
                       INCLUDE_DIRS "include" "xtensa" "${target}"
                       PRIV_INCLUDE_DIRS "private_include"
                       LDFRAGMENTS "linker.lf"
                       REQUIRES "freertos"
                       PRIV_REQUIRES "soc" "xtensa" "esp_rom")
