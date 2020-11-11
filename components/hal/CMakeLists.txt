idf_build_get_property(target IDF_TARGET)

set(srcs    "wdt_hal_iram.c"
            "mpu_hal.c")
set(includes "${target}/include" "include")

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs
                "cpu_hal.c"
                "rmt_hal.c"
                "rtc_io_hal.c"
                "dac_hal.c"
                "adc_hal.c"
                "spi_hal.c"
                "spi_hal_iram.c"
                "spi_slave_hal.c"
                "spi_slave_hal_iram.c"
                "touch_sensor_hal.c"
                "pcnt_hal.c"
                "i2s_hal.c"
                "sigmadelta_hal.c"
                "timer_hal.c"
                "ledc_hal.c"
                "ledc_hal_iram.c"
                "i2c_hal.c"
                "i2c_hal_iram.c"
                "gpio_hal.c"
                "uart_hal.c"
                "uart_hal_iram.c"
                "spi_flash_hal.c"
                "spi_flash_hal_iram.c"
                "soc_hal.c"
                "twai_hal.c"
                "twai_hal_iram.c"
                "interrupt_controller_hal.c"
                "sha_hal.c"
                "${target}/interrupt_descriptor_table.c")

    if(${target} STREQUAL "esp32")
        list(APPEND srcs
                    "mcpwm_hal.c"
                    "sdio_slave_hal.c"
                    "esp32/adc_hal.c"
                    "esp32/brownout_hal.c"
                    "esp32/touch_sensor_hal.c")
        if(NOT BOOTLOADER_BUILD AND CONFIG_ETH_USE_ESP32_EMAC)
            list(APPEND srcs "esp32/emac_hal.c")
        endif()
    endif()

    if(${target} STREQUAL "esp32s2")
        list(APPEND srcs
                    "spi_flash_hal_gpspi.c"
                    "spi_slave_hd_hal.c"
                    "esp32s2/adc_hal.c"
                    "esp32s2/brownout_hal.c"
                    "esp32s2/cp_dma_hal.c"
                    "esp32s2/systimer_hal.c"
                    "esp32s2/touch_sensor_hal.c"
                    "esp32s2/dac_hal.c"
                    "esp32s2/usb_hal.c")
    endif()

    if(${target} STREQUAL "esp32s3")
        list(APPEND srcs
            "spi_flash_hal_gpspi.c"
            "spi_slave_hd_hal.c"
            "esp32s3/brownout_hal.c"
            "esp32s3/systimer_hal.c"
            "esp32s3/touch_sensor_hal.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       REQUIRES soc
                       LDFRAGMENTS linker.lf)
