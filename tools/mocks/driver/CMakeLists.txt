# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building DRIVER MOCKS (only SPI master, I2C, RMT, USB-serial and GPIO driver)")

idf_component_get_property(original_driver_dir driver COMPONENT_OVERRIDEN_DIR)

set(include_dirs
    "${IDF_PATH}/components/esp_driver_gpio/include/driver"
    "${IDF_PATH}/components/esp_driver_gpio/include"
    "${IDF_PATH}/components/esp_driver_spi/include/driver"
    "${IDF_PATH}/components/esp_driver_spi/include"
    "${IDF_PATH}/components/esp_driver_rmt/include/driver"
    "${IDF_PATH}/components/esp_driver_rmt/include"
    "${original_driver_dir}/i2c/include/driver"
    "${IDF_PATH}/components/esp_driver_usb_serial_jtag/include/driver"
    "${original_driver_dir}/i2c/include"
    "${IDF_PATH}/components/esp_driver_usb_serial_jtag/include")

# Note: "hal" and "soc" are only required for corresponding header files and their definitions
# here, they don't provide functionality when built for running on the host.
idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES freertos hal soc
    MOCK_HEADER_FILES
    ${IDF_PATH}/components/esp_driver_gpio/include/driver/gpio.h
    ${IDF_PATH}/components/esp_driver_spi/include/driver/spi_master.h
    ${IDF_PATH}/components/esp_driver_spi/include/driver/spi_common.h
    ${IDF_PATH}/components/esp_driver_rmt/include/driver/rmt_rx.h
    ${IDF_PATH}/components/esp_driver_rmt/include/driver/rmt_tx.h
    ${IDF_PATH}/components/esp_driver_rmt/include/driver/rmt_common.h
    ${IDF_PATH}/components/esp_driver_rmt/include/driver/rmt_encoder.h
    ${original_driver_dir}/i2c/include/driver/i2c.h
    ${IDF_PATH}/components/esp_driver_usb_serial_jtag/include/driver/usb_serial_jtag.h)
