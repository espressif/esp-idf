idf_build_get_property(target IDF_TARGET)

set(srcs
    "test_app_main.c"
    "test_spi_master.c"
    "test_spi_sio.c"
    "test_spi_bus_lock.c"
)

# esp32p4 don't support because sct internal api part don't using dma link list driver
if(NOT CONFIG_IDF_TARGET_ESP32P4)
    # sct test using slave hd APIs, need slave hd support
    # tmp skip sct test under iram_safe, both sct and slave hd are not cleaned
    if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2 AND NOT CONFIG_COMPILER_DUMP_RTL_FILES)
        list(APPEND srcs "test_spi_master_sct.c")
    endif()
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    # esp_psram is required for CONFIG_SPIRAM, used by the release configs
    PRIV_REQUIRES esp_driver_spi spi_flash esp_timer esp_driver_gpio esp_mm esp_driver_uart esp_psram esp_driver_dma
    WHOLE_ARCHIVE
)
