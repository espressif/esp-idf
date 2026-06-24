/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i3c_master.h"
#include "driver/i3c_master_i2c.h"

#define ICM_42688_ADDRESS  0x69
#define ICM_42688_WHO_AM_I_REG  0x75
#define ICM_42688_WHO_AM_I_VAL  0x47

#define ICM_42688_REG_PWR_MGMT0    0x4E
#define ICM_42688_REG_ACCEL_CONFIG0 0x50
#define ICM_42688_REG_GYRO_CONFIG0  0x4F
#define ICM_42688_REG_TEMP_DATA1   0x1D
#define ICM_42688_REG_ACCEL_DATA_X1 0x1F
#define ICM_42688_REG_GYRO_DATA_X1  0x25

#define ICM_42688_ACCEL_RANGE      (16.0f)
#define ICM_42688_GYRO_RANGE       (2000.0f)
#define ADC_MAX                    (32768.0f)

#define I3C_I2C_TIMEOUT_MS         (5000)

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temp;
} icm42688_data_t;

static const char *TAG = "example";

void icm42688_init(i3c_master_i2c_device_handle_t icm42688_handle, i3c_master_bus_handle_t bus_handle)
{
    uint8_t hw_id;
    uint8_t write_hw_id = ICM_42688_WHO_AM_I_REG;
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit_receive(icm42688_handle, &write_hw_id, 1, &hw_id, 1, -1));

    if (hw_id != ICM_42688_WHO_AM_I_VAL) {
        ESP_LOGE(TAG, "Invalid Hardware ID: 0x%02X", hw_id);
        return;
    }

    uint8_t init_cmd[2] = {ICM_42688_REG_PWR_MGMT0, 0x0f}; // 0x0f stands for enable GYRO_MODE and ACCEL_MODE
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit(icm42688_handle, init_cmd, 2, I3C_I2C_TIMEOUT_MS));

    uint8_t accel_cfg[2] = {ICM_42688_REG_ACCEL_CONFIG0, 0x23}; // 0x23 stands for range +-8g with 8khz
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit(icm42688_handle, accel_cfg, 2, I3C_I2C_TIMEOUT_MS));

    uint8_t gyro_cfg[2] = {ICM_42688_REG_GYRO_CONFIG0, 0x23}; // 0x23 stands for range +-1000dps with 8khz
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit(icm42688_handle, gyro_cfg, 2, I3C_I2C_TIMEOUT_MS));
}

void icm42688_read_data(i3c_master_i2c_device_handle_t icm42688_handle, icm42688_data_t *data)
{
    uint8_t raw_data[14];
    uint8_t temp_data = ICM_42688_REG_TEMP_DATA1;
    ESP_ERROR_CHECK(i3c_master_i2c_device_transmit_receive(icm42688_handle, &temp_data, 1, raw_data, sizeof(raw_data), I3C_I2C_TIMEOUT_MS));

    int16_t temp_raw = (raw_data[0] << 8) | raw_data[1];
    // Provided by datasheet
    data->temp = (temp_raw / 132.48f) + 25.0f;

    int16_t accel_x = (raw_data[2] << 8) | raw_data[3];
    int16_t accel_y = (raw_data[4] << 8) | raw_data[5];
    int16_t accel_z = (raw_data[6] << 8) | raw_data[7];
    data->accel_x = accel_x * (ICM_42688_ACCEL_RANGE / ADC_MAX);
    data->accel_y = accel_y * (ICM_42688_ACCEL_RANGE / ADC_MAX);
    data->accel_z = accel_z * (ICM_42688_ACCEL_RANGE / ADC_MAX);

    int16_t gyro_x = (raw_data[8] << 8) | raw_data[9];
    int16_t gyro_y = (raw_data[10] << 8) | raw_data[11];
    int16_t gyro_z = (raw_data[12] << 8) | raw_data[13];
    data->gyro_x = gyro_x * (ICM_42688_GYRO_RANGE / ADC_MAX);
    data->gyro_y = gyro_y * (ICM_42688_GYRO_RANGE / ADC_MAX);
    data->gyro_z = gyro_z * (ICM_42688_GYRO_RANGE / ADC_MAX);
}

void app_main(void)
{
    i3c_master_bus_handle_t bus_handle = NULL;
    i3c_master_i2c_device_handle_t icm42688_handle = NULL;

    i3c_master_bus_config_t i3c_bus_config = {
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .scl_io_num = CONFIG_I3C_MASTER_SCL,
        .sda_io_num = CONFIG_I3C_MASTER_SDA,
    };

    ESP_ERROR_CHECK(i3c_new_master_bus(&i3c_bus_config, &bus_handle));

    i3c_device_i2c_config_t dev_cfg = {
        .device_address = ICM_42688_ADDRESS,
        .scl_freq_hz = 100000,
    };

    ESP_ERROR_CHECK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg, &icm42688_handle));

    icm42688_init(icm42688_handle, bus_handle);

    icm42688_data_t *data = heap_caps_malloc(sizeof(icm42688_data_t), MALLOC_CAP_DEFAULT);
    assert(data);

    while (1) {
        icm42688_read_data(icm42688_handle, data);
        ESP_LOGI(TAG, "temp is %f, accel (x,y,z) is (%f, %f, %f)g, gyro (x,y,z) is (%f, %f, %f)dps", data->temp, data->accel_x, data->accel_y, data->accel_z, data->gyro_x, data->gyro_y, data->gyro_z);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
