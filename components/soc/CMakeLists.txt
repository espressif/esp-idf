idf_build_get_property(target IDF_TARGET)

set(target_folder "${target}")

# On Linux the soc component is a simple wrapper, without much functionality
if(NOT ${target} STREQUAL "linux")
    set(srcs "lldesc.c"
             "dport_access_common.c"
             "${target_folder}/interrupts.c"
             "${target_folder}/gpio_periph.c"
             "${target_folder}/uart_periph.c")
endif()

set(includes "include" "${target_folder}")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${target_folder}/include")
    list(APPEND includes "${target_folder}/include")
endif()

if(target STREQUAL "esp32")
    list(APPEND srcs "${target_folder}/dport_access.c")
endif()

if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs "${target_folder}/adc_periph.c")
endif()

if(CONFIG_SOC_ANA_CMPR_SUPPORTED)
    list(APPEND srcs "${target_folder}/ana_cmpr_periph.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "${target_folder}/dedic_gpio_periph.c")
endif()

if(CONFIG_SOC_DEBUG_PROBE_SUPPORTED)
    list(APPEND srcs "${target_folder}/debug_probe_periph.c")
endif()

if(CONFIG_SOC_EMAC_SUPPORTED)
    list(APPEND srcs "${target_folder}/emac_periph.c")
endif()

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "${target_folder}/gdma_periph.c")
endif()

if(CONFIG_SOC_DMA2D_SUPPORTED)
    list(APPEND srcs "${target_folder}/dma2d_periph.c")
endif()

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "${target_folder}/spi_periph.c")
endif()

if(CONFIG_SOC_LEDC_SUPPORTED)
    list(APPEND srcs "${target_folder}/ledc_periph.c")
endif()

if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "${target_folder}/pcnt_periph.c")
endif()

if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "${target_folder}/rmt_periph.c")
endif()

if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "${target_folder}/sdm_periph.c")
endif()

if(CONFIG_SOC_ISP_SUPPORTED)
    list(APPEND srcs "${target}/isp_periph.c")
endif()

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "${target_folder}/i2s_periph.c")
endif()

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "${target_folder}/i2c_periph.c")
endif()

if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "${target_folder}/temperature_sensor_periph.c")
endif()

if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "${target_folder}/timer_periph.c")
endif()

if(CONFIG_SOC_LCDCAM_SUPPORTED OR CONFIG_SOC_I2S_SUPPORTS_LCD_CAMERA)
    list(APPEND srcs "${target_folder}/lcd_periph.c")
endif()

if(CONFIG_SOC_MIPI_DSI_SUPPORTED)
    list(APPEND srcs "${target_folder}/mipi_dsi_periph.c")
endif()

if(CONFIG_SOC_MIPI_CSI_SUPPORTED)
    list(APPEND srcs "${target_folder}/mipi_csi_periph.c")
endif()

if(CONFIG_SOC_PARLIO_SUPPORTED)
    list(APPEND srcs "${target_folder}/parlio_periph.c")
endif()

if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "${target_folder}/mcpwm_periph.c")
endif()

if(CONFIG_SOC_MPI_SUPPORTED)
    list(APPEND srcs "${target_folder}/mpi_periph.c")
endif()

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "${target_folder}/sdmmc_periph.c")
endif()

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    list(APPEND srcs "${target_folder}/touch_sensor_periph.c")
endif()

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "${target_folder}/twai_periph.c")
endif()

if(CONFIG_SOC_WDT_SUPPORTED)
    list(APPEND srcs "${target_folder}/wdt_periph.c")
endif()

if(CONFIG_SOC_IEEE802154_SUPPORTED)
    if(NOT target STREQUAL "esp32h4")
        list(APPEND srcs "${target_folder}/ieee802154_periph.c")
    endif()
endif()

if(CONFIG_SOC_USB_OTG_SUPPORTED)
    if(NOT ${target} STREQUAL "esp32p4")
        list(APPEND srcs "${target_folder}/usb_periph.c"
                         "${target_folder}/usb_dwc_periph.c")
    endif()
endif()

if(CONFIG_SOC_DAC_SUPPORTED)
    list(APPEND srcs "${target_folder}/dac_periph.c")
endif()

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0)
    list(APPEND srcs "${target_folder}/rtc_io_periph.c")
endif()

if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "${target_folder}/sdio_slave_periph.c")
endif()

if(CONFIG_SOC_PAU_SUPPORTED)
    list(APPEND srcs "${target_folder}/system_retention_periph.c")
endif()

if(CONFIG_SOC_LCDCAM_CAM_SUPPORTED)
    list(APPEND srcs "${target_folder}/cam_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       LDFRAGMENTS "linker.lf")

# For an embedded system, the MMU page size should always be defined statically
# For IDF, we define it according to the Flash size that user selects
# Replace this value in an adaptive way, if Kconfig isn't available on your platform
target_compile_definitions(${COMPONENT_LIB} INTERFACE SOC_MMU_PAGE_SIZE=CONFIG_MMU_PAGE_SIZE)

target_compile_definitions(${COMPONENT_LIB} INTERFACE SOC_XTAL_FREQ_MHZ=CONFIG_XTAL_FREQ)

if(target STREQUAL "esp32")
    # esp_dport_access_reg_read is added as an undefined symbol because otherwise
    # the linker can ignore dport_access.c as it would no other files depending on any symbols in it.
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-u esp_dport_access_reg_read")
endif()

if(NOT CONFIG_IDF_TARGET_LINUX)
     target_linker_script(${COMPONENT_LIB} INTERFACE "${target_folder}/ld/${target}.peripherals.ld")
endif()
