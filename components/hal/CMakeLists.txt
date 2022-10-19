idf_build_get_property(target IDF_TARGET)

set(srcs "mpu_hal.c"
         "efuse_hal.c"
         "${target}/efuse_hal.c"
         "mmu_hal.c")

set(includes "${target}/include" "include" "platform_port/include")

if(NOT CONFIG_HAL_WDT_USE_ROM_IMPL)
    list(APPEND srcs "wdt_hal_iram.c")
endif()

if(NOT ${target} STREQUAL "esp32")
    list(APPEND srcs "cache_hal.c")
endif()

if(${target} STREQUAL "esp32h4")
    if(CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_1)
        list(APPEND includes "${target}/include/rev1")
    elseif(CONFIG_IDF_TARGET_ESP32H4_BETA_VERSION_2)
        list(APPEND includes "${target}/include/rev2")
    endif()
endif()

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs
        "rtc_io_hal.c"
        "spi_hal.c"
        "spi_hal_iram.c"
        "spi_slave_hal.c"
        "spi_slave_hal_iram.c"
        "timer_hal.c"
        "timer_hal_iram.c"
        "gpio_hal.c"
        "uart_hal.c"
        "uart_hal_iram.c"
        "spi_flash_hal.c"
        "spi_flash_hal_iram.c"
        "spi_flash_encrypt_hal_iram.c"
        "adc_hal_common.c"
        "adc_oneshot_hal.c")

    if(CONFIG_SOC_SYSTIMER_SUPPORTED AND NOT CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
        list(APPEND srcs "systimer_hal.c")
    endif()

    if(CONFIG_SOC_LEDC_SUPPORTED)
        list(APPEND srcs "ledc_hal.c" "ledc_hal_iram.c")
    endif()

    if(CONFIG_SOC_I2C_SUPPORTED)
        list(APPEND srcs "i2c_hal.c" "i2c_hal_iram.c")
    endif()

    if(CONFIG_SOC_RMT_SUPPORTED)
        list(APPEND srcs "rmt_hal.c")
    endif()

    if(CONFIG_SOC_PCNT_SUPPORTED)
        list(APPEND srcs "pcnt_hal.c")
    endif()

    if(CONFIG_SOC_MCPWM_SUPPORTED)
        list(APPEND srcs "mcpwm_hal.c")
    endif()

    if(CONFIG_SOC_TWAI_SUPPORTED)
        list(APPEND srcs "twai_hal.c" "twai_hal_iram.c")
    endif()

    if(CONFIG_SOC_GDMA_SUPPORTED)
        list(APPEND srcs "gdma_hal.c")
    endif()

    if(CONFIG_SOC_I2S_SUPPORTED)
        list(APPEND srcs "i2s_hal.c")
    endif()

    if(CONFIG_SOC_SDM_SUPPORTED)
        list(APPEND srcs "sdm_hal.c")
    endif()

    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "emac_hal.c")
    endif()

    if(CONFIG_SOC_ADC_DMA_SUPPORTED)
        list(APPEND srcs "adc_hal.c")
    endif()

    if(CONFIG_SOC_LCDCAM_SUPPORTED)
        list(APPEND srcs "lcd_hal.c")
    endif()

    if(CONFIG_SOC_ECC_SUPPORTED)
        list(APPEND srcs "ecc_hal.c")
    endif()

    if(CONFIG_SOC_SHA_SUPPORTED)
        list(APPEND srcs "sha_hal.c")
    endif()

    if(${target} STREQUAL "esp32")
        list(APPEND srcs
            "sdio_slave_hal.c"
            "touch_sensor_hal.c"
            "aes_hal.c"
            "esp32/brownout_hal.c"
            "esp32/touch_sensor_hal.c"
            "esp32/gpio_hal_workaround.c")
    endif()

    if(${target} STREQUAL "esp32s2")
        list(APPEND srcs
                    "spi_flash_hal_gpspi.c"
                    "spi_slave_hd_hal.c"
                    "touch_sensor_hal.c"
                    "usb_hal.c"
                    "usb_phy_hal.c"
                    "xt_wdt_hal.c"
                    "aes_hal.c"
                    "esp32s2/brownout_hal.c"
                    "esp32s2/cp_dma_hal.c"
                    "esp32s2/touch_sensor_hal.c"
                    "usb_dwc_hal.c")
    endif()

    if(${target} STREQUAL "esp32s3")
        list(APPEND srcs
            "ds_hal.c"
            "spi_flash_hal_gpspi.c"
            "spi_slave_hd_hal.c"
            "touch_sensor_hal.c"
            "usb_hal.c"
            "usb_phy_hal.c"
            "xt_wdt_hal.c"
            "aes_hal.c"
            "esp32s3/brownout_hal.c"
            "esp32s3/hmac_hal.c"
            "esp32s3/touch_sensor_hal.c"
            "esp32s3/rtc_cntl_hal.c"
            "usb_dwc_hal.c")
    endif()

    if(${target} STREQUAL "esp32c3")
        list(APPEND srcs
              "ds_hal.c"
              "spi_flash_hal_gpspi.c"
              "spi_slave_hd_hal.c"
              "xt_wdt_hal.c"
              "aes_hal.c"
              "esp32c3/brownout_hal.c"
              "esp32c3/hmac_hal.c"
              "esp32c3/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32h4")
        list(APPEND srcs
            "ds_hal.c"
            "spi_flash_hal_gpspi.c"
            "spi_slave_hd_hal.c"
            "aes_hal.c"
            "esp32h4/brownout_hal.c"
            "esp32h4/hmac_hal.c"
            "esp32h4/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32c2")
        list(APPEND srcs
              "spi_flash_hal_gpspi.c"
              "spi_slave_hd_hal.c"
              "esp32c2/brownout_hal.c"
              "esp32c2/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32c6")
        list(APPEND srcs
              "spi_flash_hal_gpspi.c"
              "spi_slave_hd_hal.c"
              "esp32c6/brownout_hal.c"
              "esp32c6/rtc_cntl_hal.c")

        # TODO: IDF-5310
        list(REMOVE_ITEM srcs
                "adc_oneshot_hal.c"
                "adc_hal_common.c"
                )
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       REQUIRES soc esp_rom
                       LDFRAGMENTS linker.lf)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

if(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 1)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u abort")
elseif(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 2)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __assert_func")
endif()
