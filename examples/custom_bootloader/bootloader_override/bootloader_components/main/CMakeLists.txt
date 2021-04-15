idf_component_register(SRCS "bootloader_start.c"
                    REQUIRES bootloader bootloader_support)

idf_build_get_property(target IDF_TARGET)
# Use the linker script files from the actual bootloader
set(scripts "${IDF_PATH}/components/bootloader/subproject/main/ld/${target}/bootloader.ld"
            "${IDF_PATH}/components/bootloader/subproject/main/ld/${target}/bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
