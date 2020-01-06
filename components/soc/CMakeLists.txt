idf_build_get_property(soc_name IDF_TARGET)

if(EXISTS "${COMPONENT_DIR}/${soc_name}")
    include(${COMPONENT_DIR}/${soc_name}/sources.cmake)

    spaces2list(SOC_SRCS)
    add_prefix(srcs "${soc_name}/" ${SOC_SRCS})
    set(include_dirs ${soc_name}/include)
endif()

list(APPEND include_dirs include)
list(APPEND srcs
    "src/memory_layout_utils.c"
    "src/lldesc.c"
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
    )

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcs "src/hal/mcpwm_hal.c"
                     "src/hal/sdio_slave_hal.c"
    )
endif()

if(IDF_TARGET STREQUAL "esp32s2beta")
    list(APPEND srcs "src/hal/spi_flash_hal_gpspi.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES ${soc_name}
                       LDFRAGMENTS linker.lf)
