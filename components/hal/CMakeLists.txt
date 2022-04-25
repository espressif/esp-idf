idf_build_get_property(target IDF_TARGET)

set(srcs "wdt_hal_iram.c"
         "mpu_hal.c"
         "cpu_hal.c")

set(includes "${target}/include" "include" "platform_port/include")

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs
        "rmt_hal.c"
        "rtc_io_hal.c"
        "spi_hal.c"
        "spi_hal_iram.c"
        "spi_slave_hal.c"
        "spi_slave_hal_iram.c"
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
        "spi_flash_encrypt_hal_iram.c"
        "soc_hal.c"
        "interrupt_controller_hal.c"
        "sha_hal.c"
        "aes_hal.c"
        "twai_hal.c"
        "twai_hal_iram.c"
        "adc_hal.c")

    if(${target} STREQUAL "esp32")
        list(APPEND srcs
            "dac_hal.c"
            "mcpwm_hal.c"
            "pcnt_hal.c"
            "sdio_slave_hal.c"
            "touch_sensor_hal.c"
            "esp32/adc_hal.c"
            "esp32/brownout_hal.c"
            "esp32/interrupt_descriptor_table.c"
            "esp32/touch_sensor_hal.c"
            "esp32/gpio_hal_workaround.c")
        if(NOT BOOTLOADER_BUILD AND CONFIG_ETH_USE_ESP32_EMAC)
            list(APPEND srcs "emac_hal.c")
        endif()
    endif()

    if(${target} STREQUAL "esp32s2")
        list(APPEND srcs
                    "dac_hal.c"
                    "pcnt_hal.c"
                    "spi_flash_hal_gpspi.c"
                    "spi_slave_hd_hal.c"
                    "systimer_hal.c"
                    "touch_sensor_hal.c"
                    "usb_hal.c"
                    "usb_phy_hal.c"
                    "xt_wdt_hal.c"
                    "esp32s2/brownout_hal.c"
                    "esp32s2/cp_dma_hal.c"
                    "esp32s2/touch_sensor_hal.c"
                    "esp32s2/dac_hal.c"
                    "esp32s2/interrupt_descriptor_table.c"
                    "usbh_hal.c")
    endif()

    if(${target} STREQUAL "esp32s3")
        list(APPEND srcs
            "ds_hal.c"
            "gdma_hal.c"
            "lcd_hal.c"
            "mcpwm_hal.c"
            "pcnt_hal.c"
            "spi_flash_hal_gpspi.c"
            "spi_slave_hd_hal.c"
            "systimer_hal.c"
            "touch_sensor_hal.c"
            "usb_hal.c"
            "usb_phy_hal.c"
            "xt_wdt_hal.c"
            "esp32s3/brownout_hal.c"
            "esp32s3/hmac_hal.c"
            "esp32s3/interrupt_descriptor_table.c"
            "esp32s3/touch_sensor_hal.c"
            "esp32s3/rtc_cntl_hal.c"
            "usbh_hal.c")
    endif()

    if(${target} STREQUAL "esp32c3")
        list(APPEND srcs
              "ds_hal.c"
              "gdma_hal.c"
              "spi_flash_hal_gpspi.c"
              "spi_slave_hd_hal.c"
              "systimer_hal.c"
              "xt_wdt_hal.c"
              "esp32c3/adc_hal.c"
              "esp32c3/brownout_hal.c"
              "esp32c3/hmac_hal.c"
              "esp32c3/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32h2")
        list(APPEND srcs
            "ds_hal.c"
            "gdma_hal.c"
            "spi_flash_hal_gpspi.c"
            "spi_slave_hd_hal.c"
            "systimer_hal.c"
            "esp32h2/brownout_hal.c"
            "esp32h2/hmac_hal.c"
            "esp32h2/rtc_cntl_hal.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       REQUIRES soc
                       PRIV_REQUIRES ${target}
                       LDFRAGMENTS linker.lf)

if(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 1)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u abort")
elseif(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 2)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __assert_func")
endif()
