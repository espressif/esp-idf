/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "hal_i2c.h"
#include "esp_check.h"

#define SCL_IO_PIN CONFIG_I2C_MASTER_SCL
#define SDA_IO_PIN CONFIG_I2C_MASTER_SDA
#define MASTER_FREQUENCY CONFIG_I2C_MASTER_FREQUENCY
#define BUFFER_LEN   (35)
#define TIMEOUT_MS   (20)
static const uint8_t slave_address = 0x50;

static const char *TAG = "hal_i2c_main";

void app_main(void)
{
    hal_i2c_config i2c_config = {
        .freq = MASTER_FREQUENCY,
        .i2c_port = I2C_NUM_0,
        .scl_pin = SCL_IO_PIN,
        .sda_pin = SDA_IO_PIN,
    };
    ESP_ERROR_CHECK(hal_i2c_init(&i2c_config));
    ESP_LOGI(TAG, "HAL I2C initialized successfully");

    // Use EEPROM as a device, write 0x33 and 0x44 to at address 0x0000, then, read value back
    uint8_t tx_data[4] = {0x00, 0x00, 0x33, 0x44};
    ESP_ERROR_CHECK(hal_i2c_write(I2C_NUM_0, slave_address, tx_data, 4, TIMEOUT_MS));
    uint8_t rx_data[2];
    ESP_ERROR_CHECK(hal_i2c_write_read(I2C_NUM_0, slave_address, tx_data, 2, rx_data, 2, TIMEOUT_MS));
    ESP_LOGI(TAG, "Read back value 1 is %x, value two is %x\n", rx_data[0], rx_data[1]);

    ESP_LOGI(TAG, "HAL I2C write-read successfully");
}
