idf_build_get_property(target IDF_TARGET)

set(srcs "hal_utils.c")
set(includes "platform_port/include")

# target specific include must be added before the generic one
# becuase of the "include_next" directive used by the efuse_hal.h
if(NOT ${target} STREQUAL "linux")
    list(APPEND includes "${target}/include")
endif()
list(APPEND includes "include")

if(CONFIG_SOC_MPU_SUPPORTED)
    list(APPEND srcs "mpu_hal.c")
endif()

if(CONFIG_SOC_EFUSE_SUPPORTED)
    list(APPEND srcs "efuse_hal.c" "${target}/efuse_hal.c")
endif()

if(CONFIG_SOC_LP_TIMER_SUPPORTED)
    list(APPEND srcs "lp_timer_hal.c")
endif()

if(CONFIG_SOC_WDT_SUPPORTED AND NOT CONFIG_HAL_WDT_USE_ROM_IMPL)
    list(APPEND srcs "wdt_hal_iram.c")
endif()

if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    if(CONFIG_SOC_MMU_PERIPH_NUM)
        list(APPEND srcs "mmu_hal.c")
    endif()

    # We wrap Cache ROM APIs as Cache HAL APIs for: 1. internal ram ; 2. unified APIs
    # ESP32 cache structure / ROM APIs are different and we have a patch `cache_hal_esp32.c` for it.
    if(${target} STREQUAL "esp32")
        list(APPEND srcs "esp32/cache_hal_esp32.c")
    elseif(NOT ${target} STREQUAL "linux")
        list(APPEND srcs "cache_hal.c")
    endif()
endif()

if(NOT BOOTLOADER_BUILD)

    if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
        if(CONFIG_SOC_SPI_FLASH_SUPPORTED)
            list(APPEND srcs "spi_flash_hal.c" "spi_flash_hal_iram.c")
        endif()
        if(CONFIG_SOC_FLASH_ENC_SUPPORTED)
            list(APPEND srcs "spi_flash_encrypt_hal_iram.c")
        endif()
    endif()

    if(CONFIG_SOC_CLK_TREE_SUPPORTED)
        list(APPEND srcs "${target}/clk_tree_hal.c")
    endif()

    if(CONFIG_SOC_SYSTIMER_SUPPORTED AND NOT CONFIG_HAL_SYSTIMER_USE_ROM_IMPL)
        list(APPEND srcs "systimer_hal.c")
    endif()

    if(CONFIG_SOC_UART_SUPPORTED)
        list(APPEND srcs "uart_hal.c" "uart_hal_iram.c")
    endif()

    if(CONFIG_SOC_GPIO_PORT)
        list(APPEND srcs "gpio_hal.c")
    endif()

    if(CONFIG_SOC_RTCIO_PIN_COUNT)
        list(APPEND srcs "rtc_io_hal.c")
    endif()

    if(CONFIG_SOC_GPTIMER_SUPPORTED)
        list(APPEND srcs "timer_hal.c")
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
        list(APPEND srcs "gdma_hal_top.c")
    endif()

    if(CONFIG_SOC_GDMA_SUPPORT_CRC)
        list(APPEND srcs "gdma_hal_crc_gen.c")
    endif()

    if(CONFIG_SOC_AHB_GDMA_VERSION EQUAL 1)
        list(APPEND srcs "gdma_hal_ahb_v1.c")
    endif()

    if(CONFIG_SOC_AHB_GDMA_VERSION EQUAL 2)
        list(APPEND srcs "gdma_hal_ahb_v2.c")
    endif()

    if(CONFIG_SOC_AXI_GDMA_SUPPORTED)
        list(APPEND srcs "gdma_hal_axi.c")
    endif()

    if(CONFIG_SOC_DW_GDMA_SUPPORTED)
        list(APPEND srcs "dw_gdma_hal.c")
    endif()

    if(CONFIG_SOC_I2S_SUPPORTED)
        list(APPEND srcs "i2s_hal.c")
    endif()

    if(CONFIG_SOC_SDM_SUPPORTED)
        list(APPEND srcs "sdm_hal.c")
    endif()

    if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
        list(APPEND srcs "sdmmc_hal.c")
    endif()

    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "emac_hal.c")
    endif()

    if(CONFIG_SOC_ETM_SUPPORTED)
        list(APPEND srcs "etm_hal.c")
    endif()

    if(CONFIG_SOC_PARLIO_SUPPORTED)
        list(APPEND srcs "parlio_hal.c")
    endif()

    if(CONFIG_SOC_ADC_SUPPORTED)
        list(APPEND srcs "adc_hal_common.c" "adc_oneshot_hal.c")
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

    if(CONFIG_SOC_ECDSA_SUPPORTED)
        list(APPEND srcs "ecdsa_hal.c")
    endif()

    if(CONFIG_SOC_MPI_SUPPORTED)
        list(APPEND srcs "mpi_hal.c")
    endif()

    if(CONFIG_SOC_SHA_SUPPORTED)
        list(APPEND srcs "sha_hal.c")
    endif()

    if(CONFIG_SOC_AES_SUPPORTED)
        list(APPEND srcs "aes_hal.c")
    endif()

    if(CONFIG_SOC_MODEM_CLOCK_IS_INDEPENDENT)
        list(APPEND srcs "${target}/modem_clock_hal.c")
    endif()

    if(CONFIG_SOC_PAU_SUPPORTED)
        list(APPEND srcs "${target}/pau_hal.c")
    endif()

    if(CONFIG_SOC_BOD_SUPPORTED)
        list(APPEND srcs "brownout_hal.c")
    endif()

    if(CONFIG_SOC_GPSPI_SUPPORTED)
        list(APPEND srcs
                    "spi_hal.c"
                    "spi_hal_iram.c"
                    "spi_slave_hal.c"
                    "spi_slave_hal_iram.c")
    endif()

    if(CONFIG_SOC_SPI_SUPPORT_SLAVE_HD_VER2)
        list(APPEND srcs "spi_slave_hd_hal.c")
    endif()

    if(CONFIG_SOC_GPSPI_SUPPORTED AND NOT CONFIG_IDF_TARGET_ESP32)
        list(APPEND srcs "spi_flash_hal_gpspi.c")
    endif()

    if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
        list(APPEND srcs "sdio_slave_hal.c")
    endif()

    if(CONFIG_SOC_PMU_SUPPORTED)
        list(APPEND srcs "${target}/pmu_hal.c")
    endif()

    if(CONFIG_SOC_APM_SUPPORTED)
        list(APPEND srcs "apm_hal.c")
    endif()

    if(CONFIG_SOC_HMAC_SUPPORTED AND NOT CONFIG_IDF_TARGET_ESP32S2)
        # For ESP32-S2 we do not have HMAC HAL layer implementation yet
        list(APPEND srcs "hmac_hal.c")
    endif()

    if(CONFIG_SOC_DIG_SIGN_SUPPORTED AND NOT CONFIG_IDF_TARGET_ESP32S2)
        # For ESP32-S2 we do not have DS HAL layer implementation yet
        list(APPEND srcs "ds_hal.c")
    endif()

    if(CONFIG_SOC_KEY_MANAGER_SUPPORTED)
        list(APPEND srcs "key_mgr_hal.c")
        list(APPEND srcs "huk_hal.c")
    endif()

    if(CONFIG_SOC_USB_OTG_SUPPORTED)
        list(APPEND srcs
                    "usb_dwc_hal.c"
                    "usb_fsls_phy_hal.c")
    endif()

    if(${target} STREQUAL "esp32")
        list(APPEND srcs
            "touch_sensor_hal.c"
            "esp32/touch_sensor_hal.c"
            "esp32/gpio_hal_workaround.c")
    endif()

    if(${target} STREQUAL "esp32s2")
        list(APPEND srcs
                    "touch_sensor_hal.c"
                    "xt_wdt_hal.c"
                    "esp32s2/cp_dma_hal.c"
                    "esp32s2/touch_sensor_hal.c")
    endif()

    if(${target} STREQUAL "esp32s3")
        list(APPEND srcs
            "touch_sensor_hal.c"
            "xt_wdt_hal.c"
            "esp32s3/touch_sensor_hal.c"
            "esp32s3/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32c3")
        list(APPEND srcs
              "xt_wdt_hal.c"
              "esp32c3/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32c2")
        list(APPEND srcs
              "esp32c2/rtc_cntl_hal.c")
    endif()

    if(${target} STREQUAL "esp32h2")
        list(REMOVE_ITEM srcs
                "esp32h2/rtc_cntl_hal.c"
                )
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       REQUIRES soc esp_rom
                       LDFRAGMENTS linker.lf)

if(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 1)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u abort")
elseif(CONFIG_HAL_DEFAULT_ASSERTION_LEVEL EQUAL 2)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __assert_func")
endif()
