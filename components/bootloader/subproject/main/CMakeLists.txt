set(COMPONENT_SRCS "bootloader_start.c")
set(COMPONENT_ADD_INCLUDEDIRS "")
set(COMPONENT_REQUIRES bootloader bootloader_support)
register_component()

idf_build_get_property(target IDF_TARGET)
set(scripts "${target}.bootloader.ld"
            "${target}.bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} "${scripts}")