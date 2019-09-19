set(priv_requires bootloader_support soc)
if(BOOTLOADER_BUILD)
    # on ESP32, we will include spi_flash_rom_patch.c
    # but on other platforms no source files are needed for bootloader
    set(srcs)
else()
    set(srcs
        "cache_utils.c"
        "flash_mmap.c"
        "flash_ops.c"
        "partition.c"
        "spi_flash_chip_drivers.c"
        "spi_flash_chip_generic.c"
        "spi_flash_chip_issi.c"
        "spi_flash_rom_patch.c"
        )
    if (NOT CONFIG_IDF_TARGET_ESP32S2BETA)
        # TODO: workaround until ESP32-S2 supports new API, can be always included
        list(APPEND srcs "esp_flash_spi_init.c"
                         "memspi_host_driver.c"
                         "spi_flash_os_func_app.c"
                         "spi_flash_os_func_noos.c"
                         "memspi_host_driver.c"
                         "esp_flash_api.c"
                         )
    endif()

    set(priv_requires bootloader_support app_update soc)
endif()

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcs "spi_flash_rom_patch.c")
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES "${priv_requires}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    LDFRAGMENTS linker.lf)

