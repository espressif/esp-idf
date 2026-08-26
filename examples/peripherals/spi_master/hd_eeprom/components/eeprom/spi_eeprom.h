/*
    This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D
    EEPROM (8-bit mode).

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/// Configurations of the spi_eeprom
typedef struct {
    spi_host_device_t host; ///< The SPI host used, set before calling `spi_eeprom_init()`
    gpio_num_t cs_io;       ///< CS gpio number, set before calling `spi_eeprom_init()`
    gpio_num_t miso_io;     ///< MISO gpio number, set before calling `spi_eeprom_init()`
    bool intr_used;         ///< Whether to use polling or interrupt when waiting for write to be done. Set before calling `spi_eeprom_init()`.
} eeprom_config_t;

typedef struct eeprom_context_t* eeprom_handle_t;

/**
 * @brief Initialize the hardware.
 *
 * @param config Configuration of the EEPROM
 * @param out_handle Output context of EEPROM communication.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_INVALID_ARG: If the configuration in the context is incorrect.
 *  - ESP_ERR_NO_MEM: if semaphore create failed.
 *  - or other return value from `spi_bus_add_device()` or `gpio_isr_handler_add()`.
 */
esp_err_t spi_eeprom_init(const eeprom_config_t *config, eeprom_handle_t* out_handle);

/**
 * @brief Release the resources used by the EEPROM.
 *
 * @param handle Context of EEPROM communication.
 * @return Always ESP_OK
 */
esp_err_t spi_eeprom_deinit(eeprom_handle_t handle);

/**
 * @brief Read a byte from the EEPROM.
 *
 * @param handle Context of EEPROM communication.
 * @param addr      Address to read.
 * @param out_data  Buffer to output the read data.
 * @return return value from `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_read(eeprom_handle_t handle, uint8_t addr, uint8_t* out_data);

/**
 * @brief Erase a byte in the EEPROM.
 *
 * @param handle Context of EEPROM communication.
 * @param addr  Address to erase.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_TIMEOUT: if the EEPROM is not able to be ready before the time in the spec. This may mean that the connection is not correct.
 *  - or return value from `spi_device_acquire_bus()` `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_erase(eeprom_handle_t handle, uint8_t addr);

/**
 * @brief Write a byte into the EEPROM
 *
 * @param handle Context of EEPROM communication.
 * @param addr  Address to write.
 * @param data  The byte to write.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_TIMEOUT: if the EEPROM is not able to be ready before the time in the spec. This may mean that the connection is not correct.
 *  - or return value from `spi_device_acquire_bus()` `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_write(eeprom_handle_t handle, uint8_t addr, uint8_t data);

/**
 * @brief Enable following write/erase to the EEPROM.
 *
 * @param handle Context of EEPROM communication.
 * @return return value from `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_write_enable(eeprom_handle_t handle);

/**
 * @brief Disable following write/erase to the EEPROM.
 *
 * @param handle Context of EEPROM communication.
 * @return return value from `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_write_disable(eeprom_handle_t handle);

#if CONFIG_EXAMPLE_5V_COMMANDS
/**
 * @brief Erase all the memory in the EEPROM.
 *
 * @note This is only supported when EEPROM VCC is 5V.
 * @param handle Context of EEPROM communication.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_TIMEOUT: if the EEPROM is not able to be ready before the time in the spec. This may mean that the connection is not correct.
 *  - or return value from `spi_device_acquire_bus()` `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_erase_all(eeprom_handle_t handle);

/**
 * @brief write all the memory in the EEPROM to the value given.
 *
 * @note This is only supported when EEPROM VCC is 5V.
 * @param handle Context of EEPROM communication.
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_TIMEOUT: if the EEPROM is not able to be ready before the time in the spec. This may mean that the connection is not correct.
 *  - or return value from `spi_device_acquire_bus()` `spi_device_polling_transmit()`.
 */
esp_err_t spi_eeprom_write_all(eeprom_handle_t handle, uint8_t data);
#endif //CONFIG_EXAMPLE_5V_COMMANDS
