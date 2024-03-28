idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")

set(public_include "include")

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs
        "src/gpspi/spi_common.c"
        "src/gpspi/spi_master.c"
        "src/gpspi/spi_slave.c"
        "src/gpspi/spi_dma.c"
    )

    if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2)
        list(APPEND srcs "src/gpspi/spi_slave_hd.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES esp_pm
                       PRIV_REQUIRES esp_timer esp_mm esp_driver_gpio esp_ringbuf
                       LDFRAGMENTS "linker.lf"
                      )
