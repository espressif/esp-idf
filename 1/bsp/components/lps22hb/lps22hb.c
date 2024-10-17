#include "lps22hb.h"
#include <stdio.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <driver/i2c.h>

static const char *TAG = "LPS22HB";

// I2C write helper function
esp_err_t i2c_write_byte(uint8_t device_addr, uint8_t reg_addr, uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// I2C read helper function
esp_err_t i2c_read_bytes(uint8_t device_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, len, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

// Initialize LPS22HB
esp_err_t lps22hb_init()
{
    // Configure CTRL_REG1 for continuous mode at 1Hz output data rate
    return i2c_write_byte(LPS22HB_ADDRESS, CTRL_REG1, 0x50);  // Power on, 1Hz ODR
}

// Verify WHO_AM_I register
esp_err_t lps22hb_who_am_i(uint8_t device_addr)
{
    uint8_t who_am_i_reg_id;
    esp_err_t ret = i2c_read_bytes(device_addr, WHO_AM_I_REG, &who_am_i_reg_id, 1);
    
    if (ret == ESP_OK && who_am_i_reg_id == EXPECTED_WHO_AM_I)
    {
        ESP_LOGI(TAG, "WHO_AM_I matched: 0x%02X", who_am_i_reg_id);
        return ESP_OK;  // ID matched
    }
    else
    {
        ESP_LOGE(TAG, "WHO_AM_I mismatch. Expected: 0x%02X, Got: 0x%02X", EXPECTED_WHO_AM_I, who_am_i_reg_id);
        return ESP_FAIL;  // ID mismatch
    }
}

// Read pressure from LPS22HB
esp_err_t lps22hb_read_pressure(float *pressure_hPa)
{
    uint8_t raw_data[3];  // Buffer to store raw pressure data
    esp_err_t ret = i2c_read_bytes(LPS22HB_ADDRESS, PRESS_OUT_XL, raw_data, 3);
    if (ret == ESP_OK)
    {
        int32_t raw_pressure = (int32_t)((raw_data[2] << 16) | (raw_data[1] << 8) | raw_data[0]);
        *pressure_hPa = raw_pressure / 4096.0;  // Convert to hPa
    }
    return ret;
}

// Read temperature from LPS22HB
esp_err_t lps22hb_read_temperature(float *temperature_C)
{
    uint8_t raw_data[2];  // Buffer to store raw temperature data
    esp_err_t ret = i2c_read_bytes(LPS22HB_ADDRESS, TEMP_OUT_L, raw_data, 2);
    if (ret == ESP_OK)
    {
        int16_t raw_temperature = (int16_t)((raw_data[1] << 8) | raw_data[0]);
        *temperature_C = raw_temperature / 100.0;  // Convert to Celsius
    }
    return ret;
}