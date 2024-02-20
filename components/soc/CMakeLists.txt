idf_build_get_property(target IDF_TARGET)

# On Linux the soc component is a simple wrapper, without much functionality
if(NOT ${target} STREQUAL "linux")
    set(srcs "lldesc.c"
             "dport_access_common.c"
             "${target}/interrupts.c"
             "${target}/gpio_periph.c"
             "${target}/uart_periph.c")
endif()

set(includes "include" "${target}")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

if(target STREQUAL "esp32")
    list(APPEND srcs "${target}/dport_access.c")
endif()

if(CONFIG_SOC_ADC_SUPPORTED)
    list(APPEND srcs "${target}/adc_periph.c")
endif()

if(CONFIG_SOC_ANA_CMPR_SUPPORTED)
    list(APPEND srcs "${target}/ana_cmpr_periph.c")
endif()

if(CONFIG_SOC_DEDICATED_GPIO_SUPPORTED)
    list(APPEND srcs "${target}/dedic_gpio_periph.c")
endif()

if(CONFIG_SOC_GDMA_SUPPORTED)
    list(APPEND srcs "${target}/gdma_periph.c")
endif()

if(CONFIG_SOC_GPSPI_SUPPORTED)
    list(APPEND srcs "${target}/spi_periph.c")
endif()

if(CONFIG_SOC_LEDC_SUPPORTED)
    list(APPEND srcs "${target}/ledc_periph.c")
endif()

if(CONFIG_SOC_PCNT_SUPPORTED)
    list(APPEND srcs "${target}/pcnt_periph.c")
endif()

if(CONFIG_SOC_RMT_SUPPORTED)
    list(APPEND srcs "${target}/rmt_periph.c")
endif()

if(CONFIG_SOC_SDM_SUPPORTED)
    list(APPEND srcs "${target}/sdm_periph.c")
endif()

if(CONFIG_SOC_I2S_SUPPORTED)
    list(APPEND srcs "${target}/i2s_periph.c")
endif()

if(CONFIG_SOC_I2C_SUPPORTED)
    list(APPEND srcs "${target}/i2c_periph.c")
endif()

if(CONFIG_SOC_TEMP_SENSOR_SUPPORTED)
    list(APPEND srcs "${target}/temperature_sensor_periph.c")
endif()

if(CONFIG_SOC_GPTIMER_SUPPORTED)
    list(APPEND srcs "${target}/timer_periph.c")
endif()

if(CONFIG_SOC_LCDCAM_SUPPORTED OR CONFIG_SOC_LCD_I80_SUPPORTED)
    list(APPEND srcs "${target}/lcd_periph.c")
endif()

if(CONFIG_SOC_PARLIO_SUPPORTED)
    list(APPEND srcs "${target}/parlio_periph.c")
endif()

if(CONFIG_SOC_MCPWM_SUPPORTED)
    list(APPEND srcs "${target}/mcpwm_periph.c")
endif()

if(CONFIG_SOC_MPI_SUPPORTED)
    list(APPEND srcs "${target}/mpi_periph.c")
endif()

if(CONFIG_SOC_SDMMC_HOST_SUPPORTED)
    list(APPEND srcs "${target}/sdmmc_periph.c")
endif()

if(CONFIG_SOC_TOUCH_SENSOR_SUPPORTED)
    list(APPEND srcs "${target}/touch_sensor_periph.c")
endif()

if(CONFIG_SOC_TWAI_SUPPORTED)
    list(APPEND srcs "${target}/twai_periph.c")
endif()

if(CONFIG_SOC_IEEE802154_SUPPORTED)
    if(NOT target STREQUAL "esp32h4")
        list(APPEND srcs "${target}/ieee802154_periph.c")
    endif()
endif()

if(CONFIG_SOC_USB_OTG_SUPPORTED)
    list(APPEND srcs "${target}/usb_periph.c"
                     "${target}/usb_dwc_periph.c")
endif()

if(CONFIG_SOC_DAC_SUPPORTED)
    list(APPEND srcs "${target}/dac_periph.c")
endif()

if(CONFIG_SOC_RTCIO_PIN_COUNT GREATER 0)
    list(APPEND srcs "${target}/rtc_io_periph.c")
endif()

if(CONFIG_SOC_SDIO_SLAVE_SUPPORTED)
    list(APPEND srcs "${target}/sdio_slave_periph.c")
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
     target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/ld/${target}.peripherals.ld")
endif()
