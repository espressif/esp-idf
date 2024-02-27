idf_component_register(SRCS "bootloader_start.c"
                    REQUIRES bootloader bootloader_support)

if(CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION)
    set(target_folder "esp32c5/beta3")
elseif(CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
    set(target_folder "esp32c5/mp")
else()
    set(target_folder "${target}")
endif()

idf_build_get_property(target IDF_TARGET)
set(scripts "ld/${target_folder}/bootloader.ld")

list(APPEND scripts "ld/${target_folder}/bootloader.rom.ld")
target_linker_script(${COMPONENT_LIB} INTERFACE "${scripts}")

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u bootloader_hooks_include")
