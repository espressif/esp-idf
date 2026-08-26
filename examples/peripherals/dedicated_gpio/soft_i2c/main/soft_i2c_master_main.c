/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soft_i2c_master.h"

/* I2C device address to communicate with */
#define I2C_DEVICE_ADDRESS 0x20

const char* EXAMPLE_TAG = "soft_i2c_master";

void app_main(void)
{
    esp_err_t ret = ESP_OK;
    soft_i2c_master_bus_t bus = NULL;
    soft_i2c_master_config_t config = {
        .scl_pin = CONFIG_EXAMPLE_TARGET_SCL,
        .sda_pin = CONFIG_EXAMPLE_TARGET_SDA,
        .freq = SOFT_I2C_100KHZ
    };

    /* Initialize and configure the software I2C bus */
    ret = soft_i2c_master_new(&config, &bus);
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error configuring software I2C");

    /* Write the register 0 of our device, set it to 0x42 */
    uint8_t write_buffer[] = { 0x0, 0x42 };
    ret = soft_i2c_master_write(bus, I2C_DEVICE_ADDRESS, write_buffer, sizeof(write_buffer));
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error writing to the I2C device");

    /* Read from the device. The register that will be read cannot be pre-determined, it will depend on the device's
     * internal counter. Let's simply make sure the function returns correctly. */
    uint8_t read_buffer[] = { 0x0 };
    ret = soft_i2c_master_read(bus, I2C_DEVICE_ADDRESS, read_buffer, sizeof(read_buffer));
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error reading from the I2C device");

    /* Finally, read the back the first register (0) that we wrote earlier. */
    write_buffer[0] = 0;
    ret = soft_i2c_master_write_read(bus, I2C_DEVICE_ADDRESS, write_buffer, 1, read_buffer, 1);
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error reading a register from the I2C device");

    ESP_LOGI(EXAMPLE_TAG, "I2C transfers succeeded, received byte 0x%02x", read_buffer[0]);

error:
    if (bus != NULL) {
        soft_i2c_master_del(bus);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(EXAMPLE_TAG, "An error occurred while communicating with the I2C device");
    }
}
