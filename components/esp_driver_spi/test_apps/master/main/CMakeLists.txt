
set(srcs
    "test_app_main.c"
    "test_spi_master.c"
    "test_spi_sio.c"
    "test_spi_bus_lock.c"
)

# sct test using slave hd APIs, need slave hd support
# tmp skip sct test under iram_safe, both sct and slave hd are not cleaned
if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2 AND CONFIG_SOC_SPI_SCT_SUPPORTED AND NOT CONFIG_COMPILER_DUMP_RTL_FILES)
    list(APPEND srcs "test_spi_master_sct.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES esp_driver_spi spi_flash esp_timer
    WHOLE_ARCHIVE
)
