idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)

set(public_include "include")

# SDIO Slave related source files
if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "src/sdio_slave.c")
endif()

set(priv_requires esp_driver_gpio esp_ringbuf)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       REQUIRES esp_hal_sd
                       PRIV_REQUIRES "${priv_requires}"
                      )
