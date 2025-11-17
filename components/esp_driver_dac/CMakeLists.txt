idf_build_get_property(target IDF_TARGET)

set(srcs)
set(priv_req esp_pm esp_driver_gpio)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
elseif(${target} STREQUAL "esp32")
    list(APPEND priv_req esp_driver_i2s)
elseif(${target} STREQUAL "esp32s2")
    list(APPEND priv_req esp_driver_spi)
endif()

if(CONFIG_SOC_DAC_SUPPORTED)
    list(APPEND srcs "dac_oneshot.c"
                     "dac_cosine.c"
                     "dac_continuous.c"
                     "dac_common.c")

    if(${target} STREQUAL "esp32")
        list(APPEND srcs "esp32/dac_dma.c")
    elseif(${target} STREQUAL "esp32s2")
        list(APPEND srcs "esp32s2/dac_dma.c")
    endif()

endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "./include"
                    PRIV_REQUIRES ${priv_req}
                    LDFRAGMENTS "linker.lf"
                    )
