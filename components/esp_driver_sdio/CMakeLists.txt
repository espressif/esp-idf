idf_build_get_property(target IDF_TARGET)

set(srcs)

set(public_include "include")

# SDIO Slave related source files
if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "src/sdio_slave.c")
endif()

if(${target} STREQUAL "linux")
    set(priv_requires esp_ringbuf)
else()
    set(priv_requires esp_driver_gpio esp_ringbuf)
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "${priv_requires}"
                      )
