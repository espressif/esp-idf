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
    "src/hal/spi_hal.c"
    "src/hal/spi_hal_iram.c"
    "src/hal/spi_slave_hal.c"
    "src/hal/spi_slave_hal_iram.c"
    "src/soc_include_legacy_warn.c"
    )

# TODO: SPI Flash HAL for ESP32S2Beta also
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcs "src/hal/spi_flash_hal.c"
                     "src/hal/spi_flash_hal_iram.c"
                     )
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "${include_dirs}"
                       PRIV_REQUIRES ${soc_name}
                       LDFRAGMENTS linker.lf)
