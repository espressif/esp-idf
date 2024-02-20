set(srcs)

set(public_include "include")

# SDIO Slave related source files
if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "src/sdio_slave.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES esp_driver_gpio esp_ringbuf
                      )
