idf_component_register(SRCS "bootloader_start.c"
                    REQUIRES bootloader bootloader_support)

idf_build_get_property(target IDF_TARGET)

# TODO: [ESP32C5] IDF-9197 remove this when beta3 is removed
if(CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
    set(target_folder "esp32c5/mp")
elseif(CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION)
    set(target_folder "esp32c5/beta3")
else()
    set(target_folder "${target}")
endif()

# Use the linker script files from the actual bootloader
set(scripts "${IDF_PATH}/components/bootloader/subproject/main/ld/${target_folder}/bootloader.ld"
            "${IDF_PATH}/components/bootloader/subproject/main/ld/${target_folder}/bootloader.rom.ld")

target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")
