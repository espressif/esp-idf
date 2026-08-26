/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "esp_err.h"
#include "hal/i2c_types.h"
#include "hal/i2c_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief hal i2c config structure
 */
typedef struct {
    int scl_pin;           /*!< SCL PIN, -1 means not change the current pin*/
    int sda_pin;           /*!< SDA PIN, -1 means not change the current pin*/
    uint32_t freq;         /*!< SCL frequency*/
    i2c_port_t i2c_port;   /*!< i2c port */
} hal_i2c_config;

/**
 * @brief Initialise hal i2c driver.
 *
 * @param cfg Configuration structure.
 * @return
 *       - ESP_OK: Initialize hal i2c driver successfully.
 *       - ESP_ERR_INVALID_ARG: Invalid argument of this function.
 */
esp_err_t hal_i2c_init(hal_i2c_config *cfg);

/**
 * @brief I2C write data to slave
 *
 * @param[in] port_num I2C port
 * @param[in] addr Address of slave
 * @param[in] txdata The pointer data to be sent
 * @param[in] txlength The length of data to be sent
 * @param[in] timeout_ms The timeout(ms) value per command in this function
 * @return
 *       - ESP_OK: I2C write data successfully
 *       - ESP_ERR_INVALID_STATE: The port you are using is not initialized
 *       - ESP_ERR_TIMEOUT: I2C write to slave timeout.
 */
esp_err_t hal_i2c_write(i2c_port_t port_num, uint16_t addr, const uint8_t *txdata, uint32_t txlength, uint32_t timeout_ms);

/**
 * @brief I2C read data from slave
 *
 * @param[in] port_num I2C port
 * @param[in] addr Address of slave
 * @param[out] rxdata The pointer of the memory to receive data
 * @param[in] rxlength The length of data to be received
 * @param[in] timeout_ms The timeout(ms) value per command in this function
 * @return
 *       - ESP_OK: I2C write data successfully
 *       - ESP_ERR_INVALID_STATE: The port you are using is not initialized
 *       - ESP_ERR_TIMEOUT: I2C write to slave timeout.
 */
esp_err_t hal_i2c_read(i2c_port_t port_num, uint16_t addr, uint8_t *rxdata, uint32_t rxlength, uint32_t timeout_ms);

/**
 * @brief I2C write data to slave and then read from slave
 *
 * @param[in] port_num I2C port
 * @param[in] addr Address of slave
 * @param[in] txdata The pointer data to be sent
 * @param[in] txlength The length of data to be sent
 * @param[out] rxdata The pointer of the memory to receive data
 * @param[in] rxlength The length of data to be received
 * @param[in] timeout_ms The timeout(ms) value per command in this function
 * @return
 *       - ESP_OK: I2C write data successfully
 *       - ESP_ERR_INVALID_STATE: The port you are using is not initialized
 *       - ESP_ERR_TIMEOUT: I2C write to slave timeout.
 */
esp_err_t hal_i2c_write_read(i2c_port_t port_num, uint16_t addr, const uint8_t *txdata, uint32_t txlength, uint8_t *rxdata, uint32_t rxlength, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
