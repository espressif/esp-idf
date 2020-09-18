idf_build_get_property(target IDF_TARGET)

set(srcs "src/cpu_util.c"
          "src/memory_layout_utils.c"
          "src/lldesc.c"
          "src/hal/cpu_hal.c"
          "src/hal/rmt_hal.c"
          "src/hal/rtc_io_hal.c"
          "src/hal/dac_hal.c"
          "src/hal/adc_hal.c"
          "src/hal/spi_hal.c"
          "src/hal/spi_hal_iram.c"
          "src/hal/spi_slave_hal.c"
          "src/hal/spi_slave_hal_iram.c"
          "src/hal/touch_sensor_hal.c"
          "src/soc_include_legacy_warn.c"
          "src/hal/pcnt_hal.c"
          "src/hal/i2s_hal.c"
          "src/hal/sigmadelta_hal.c"
          "src/hal/timer_hal.c"
          "src/hal/ledc_hal.c"
          "src/hal/ledc_hal_iram.c"
          "src/hal/i2c_hal.c"
          "src/hal/i2c_hal_iram.c"
          "src/hal/gpio_hal.c"
          "src/hal/uart_hal.c"
          "src/hal/uart_hal_iram.c"
          "src/hal/spi_flash_hal.c"
          "src/hal/spi_flash_hal_iram.c"
          "src/hal/mpu_hal.c"
          "src/hal/soc_hal.c"
          "src/hal/wdt_hal_iram.c"
          "src/compare_set.c"
          "src/hal/twai_hal.c"
          "src/hal/twai_hal_iram.c")

if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "src/regi2c_ctrl.c")
endif()

set(priv_requires ${target})
if(${target} STREQUAL "esp32")
     # For RTC_EXT_CRYST_ADDIT_CURRENT to get chip revision in rtc_clk.c
     list(APPEND priv_requires efuse)
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

if(CONFIG_IDF_TARGET_ESP32)
    target_sources(${COMPONENT_LIB} PRIVATE "src/hal/mcpwm_hal.c"
                                            "src/hal/sdio_slave_hal.c")
elseif(CONFIG_IDF_TARGET_ESP32S2)
    target_sources(${COMPONENT_LIB} PRIVATE "src/hal/spi_flash_hal_gpspi.c")
endif()

# Since there can be chip-specific HAL headers which can include the common
# HAL header via include_next, process the build scripts here first so that
# include directories appear first in the compile command.
add_subdirectory(src/${target})
target_include_directories(${COMPONENT_LIB} PUBLIC include)

add_subdirectory(soc)
target_link_libraries(${COMPONENT_LIB} PUBLIC "soc_${target}")
