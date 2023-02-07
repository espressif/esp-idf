idf_build_get_property(target IDF_TARGET)

set(includes "include")

# Note: requires spi_flash for cache_utils, will be refactored
set(priv_requires heap spi_flash)

set(srcs)

if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    set(srcs "esp_mmu_map.c"
             "port/${target}/ext_mem_layout.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires})

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_SPIRAM)
        # Use esp_psram for `esp_psram_extram_writeback_cache()` on ESP32
        idf_component_optional_requires(PRIVATE esp_psram)
    endif()
endif()
