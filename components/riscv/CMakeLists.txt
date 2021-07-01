idf_build_get_property(target IDF_TARGET)

# should test arch here not target: IDF-1754
if(NOT "${target}" STREQUAL "esp32c3" AND NOT "${target}" STREQUAL "esp32h2")
    return()
endif()

if(BOOTLOADER_BUILD)
    set(priv_requires soc)
else()
    set(priv_requires soc freertos)
    set(srcs
        "instruction_decode.c"
        "interrupt.c"
        "vectors.S")
endif()

idf_component_register(SRCS "${srcs}"
                       LDFRAGMENTS linker.lf
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES ${priv_requires})
