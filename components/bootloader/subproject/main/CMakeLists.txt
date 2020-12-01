idf_component_register(SRCS "bootloader_start.c"
                    REQUIRES bootloader bootloader_support)

idf_build_get_property(target IDF_TARGET)
set(scripts "ld/${target}/bootloader.ld")

if(NOT CONFIG_IDF_TARGET_ESP32C3)
    list(APPEND scripts "ld/${target}/bootloader.rom.ld")
endif()

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
