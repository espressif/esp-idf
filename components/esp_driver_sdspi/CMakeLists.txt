idf_build_get_property(target IDF_TARGET)

set(srcs)

set(public_include "include")

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "src/sdspi_crc.c"
                     "src/sdspi_host.c"
                     "src/sdspi_transaction.c")
endif()

if(${target} STREQUAL "linux")
    set(requires "")
    set(priv_requires esp_timer)
else()
    set(requires sdmmc esp_driver_spi esp_driver_gpio)
    set(priv_requires esp_timer)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES "${requires}"
                       PRIV_REQUIRES "${priv_requires}"
                      )
