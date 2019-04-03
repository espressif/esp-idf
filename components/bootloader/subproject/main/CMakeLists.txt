set(COMPONENT_SRCS "bootloader_start.c")
set(COMPONENT_ADD_INCLUDEDIRS "")
set(COMPONENT_REQUIRES bootloader bootloader_support)
register_component()

idf_build_get_property(target IDF_TARGET)

target_linker_script(${COMPONENT_LIB}
    "ld/${target}/bootloader.ld"
    "ld/${target}/bootloader.rom.ld"
)

set(scripts
    "${IDF_PATH}/components/esp_rom/${target}/ld/${target}.rom.ld"
    "${IDF_PATH}/components/esp_rom/${target}/ld/${target}.rom.newlib-funcs.ld"
    "${IDF_PATH}/components/${target}/ld/${target}.peripherals.ld")

target_linker_script(${COMPONENT_LIB} ${scripts})
