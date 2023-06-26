# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
#       Furthermore, this component can only mock the interfaces of
#       spi_master.h and gpio.h.
message(STATUS "building DRIVER MOCKS (only SPI, I2C and GPIO driver)")

idf_component_get_property(original_driver_dir driver COMPONENT_OVERRIDEN_DIR)

set(include_dirs
    "${original_driver_dir}/i2c/include/driver"
    "${original_driver_dir}/spi/include/driver"
    "${original_driver_dir}/gpio/include/driver"
    "${original_driver_dir}/rmt/include/driver"
    "${original_driver_dir}/i2c/include"
    "${original_driver_dir}/spi/include"
    "${original_driver_dir}/gpio/include"
    "${original_driver_dir}/rmt/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../hal/include")

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES freertos
    MOCK_HEADER_FILES
    ${original_driver_dir}/spi/include/driver/spi_master.h
    ${original_driver_dir}/spi/include/driver/spi_common.h
    ${original_driver_dir}/i2c/include/driver/i2c.h
    ${original_driver_dir}/gpio/include/driver/gpio.h
    ${original_driver_dir}/rmt/include/driver/rmt_rx.h
    ${original_driver_dir}/rmt/include/driver/rmt_tx.h
    ${original_driver_dir}/rmt/include/driver/rmt_common.h
    ${original_driver_dir}/rmt/include/driver/rmt_encoder.h)

target_compile_definitions(${COMPONENT_LIB} PUBLIC SOC_I2C_NUM=2)
