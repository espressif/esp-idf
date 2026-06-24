idf_build_get_property(target IDF_TARGET)

set(srcs)
set(includes "include")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs
        "${target}/spi_periph.c"
        "spi_hal.c"
        "spi_hal_iram.c"
        "spi_slave_hal.c"
        "spi_slave_hal_iram.c"
    )

    if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2)
        list(APPEND srcs "spi_slave_hd_hal.c")
    endif()
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES soc hal esp_hal_dma
    PRIV_REQUIRES esp_hal_gpio
)
