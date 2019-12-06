set(priv_requires bootloader_support soc)
if(BOOTLOADER_BUILD)
    if (CONFIG_IDF_TARGET_ESP32)
        # ESP32 Bootloader needs SPIUnlock from this file, but doesn't
        # need other parts of this component
        set(srcs "esp32/spi_flash_rom_patch.c")
    elseif (CONFIG_IDF_TARGET_ESP32S2BETA)
        set(srcs "esp32s2beta/spi_flash_rom_patch.c")
    else()
        # but on other platforms no source files are needed for bootloader
        set(srcs)
    endif()
    set(cache_srcs "")
else()
    set(cache_srcs
        "cache_utils.c"
        "flash_mmap.c"
        "flash_ops.c"
    )
    set(srcs
        "partition.c")

    if (CONFIG_IDF_TARGET_ESP32)
        list(APPEND srcs
            "esp32/spi_flash_rom_patch.c")
    elseif (CONFIG_IDF_TARGET_ESP32S2BETA)
        list(APPEND srcs 
            "esp32s2beta/spi_flash_rom_patch.c")
    endif()

    # New implementation after IDF v4.0
    list(APPEND srcs
        "spi_flash_chip_drivers.c"
        "spi_flash_chip_generic.c"
        "spi_flash_chip_issi.c"
        "spi_flash_chip_gd.c")

    if (NOT CONFIG_IDF_TARGET_ESP32S2BETA)
        # TODO: workaround until ESP32-S2 supports new API, can be always included
        list(APPEND srcs
            "memspi_host_driver.c")
        list(APPEND cache_srcs
        "esp_flash_api.c"
        "esp_flash_spi_init.c"
        "spi_flash_os_func_app.c"
        "spi_flash_os_func_noos.c")
    endif()
    list(APPEND srcs ${cache_srcs})
    set(priv_requires bootloader_support app_update soc)
endif()

idf_component_register(SRCS "${srcs}"
                    PRIV_REQUIRES "${priv_requires}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    LDFRAGMENTS linker.lf)

# Avoid cache miss by unexpected inlineing when built by -Os
set_source_files_properties(${cache_srcs} PROPERTIES COMPILE_FLAGS
    "-fno-inline-functions -fno-inline-small-functions -fno-inline-functions-called-once")
