# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
#       Furthermore, this component can only mock the interfaces of
#       spi_master.h and gpio.h.
message(STATUS "building SPI FLASH MOCKS (only esp_partition support)")

idf_component_get_property(original_spi_flash_dir spi_flash COMPONENT_OVERRIDEN_DIR)

idf_build_get_property(target IDF_TARGET)

set(include_dirs
    "${original_spi_flash_dir}/include")

if(${target} STREQUAL "linux")
    list(APPEND include_dirs
        "${original_spi_flash_dir}/sim/stubs/soc/include")
endif()

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_common
    MOCK_HEADER_FILES
    ${original_spi_flash_dir}/include/esp_flash.h
    ${original_spi_flash_dir}/include/spi_flash_mmap.h)
