/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    i2c_device_config_t eeprom_device;  /*!< Configuration for eeprom device */
    uint8_t addr_wordlen;               /*!< block address wordlen */
    uint8_t write_time_ms;              /*!< eeprom write time, typically 10ms*/
} i2c_eeprom_config_t;

struct i2c_eeprom_t {
    i2c_master_dev_handle_t i2c_dev;      /*!< I2C device handle */
    uint8_t addr_wordlen;                 /*!< block address wordlen */
    uint8_t *buffer;                      /*!< I2C transaction buffer */
    uint8_t write_time_ms;                /*!< I2C eeprom write time(ms)*/
};

typedef struct i2c_eeprom_t i2c_eeprom_t;

/* handle of EEPROM device */
typedef struct i2c_eeprom_t *i2c_eeprom_handle_t;

/**
 * @brief Init an EEPROM device.
 *
 * @param[in] bus_handle I2C master bus handle
 * @param[in] eeprom_config Configuration of EEPROM
 * @param[out] eeprom_handle Handle of EEPROM
 * @return ESP_OK: Init success. ESP_FAIL: Not success.
 */
esp_err_t i2c_eeprom_init(i2c_master_bus_handle_t bus_handle, const i2c_eeprom_config_t *eeprom_config, i2c_eeprom_handle_t *eeprom_handle);

/**
 * @brief Write data to EEPROM
 *
 * @param[in] eeprom_handle EEPROM handle
 * @param[in] address Block address inside EEPROM
 * @param[in] data Data to write
 * @param[in] size Data write size
 * @return ESP_OK: Write success. Otherwise failed, please check I2C function fail reason.
 */
esp_err_t i2c_eeprom_write(i2c_eeprom_handle_t eeprom_handle, uint32_t address, const uint8_t *data, uint32_t size);

/**
 * @brief Read data from EEPROM
 *
 * @param eeprom_handle EEPROM handle
 * @param address Block address inside EEPROM
 * @param data Data read from EEPROM
 * @param size Data read size
 * @return ESP_OK: Read success. Otherwise failed, please check I2C function fail reason.
 */
esp_err_t i2c_eeprom_read(i2c_eeprom_handle_t eeprom_handle, uint32_t address, uint8_t *data, uint32_t size);

/**
 * @brief Wait eeprom finish. Typically 5ms
 *
 * @param eeprom_handle EEPROM handle
 */
void i2c_eeprom_wait_idle(i2c_eeprom_handle_t eeprom_handle);

#ifdef __cplusplus
}
#endif
