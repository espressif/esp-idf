/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_spi.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_print.h"
#include "bme280_defs.h"

#define LP_SPI_TRANS_WAIT_FOREVER   -1
#define WAKEUP_HP_CPU_LIMIT_CELSIUS 30

// Uncomment to print debug logs
// #define DEBUG

#ifdef DEBUG
#define DEBUG_LOG lp_core_printf
#else
#define DEBUG_LOG (void)
#endif

static bool start_up = true;

// Misc values used to compensate the measurements
static uint16_t dig_T1;
static int16_t dig_T2;
static int16_t dig_T3;
static int32_t t_fine;

static uint16_t dig_H1;
static int16_t dig_H2;
static uint16_t dig_H3;
static int16_t dig_H4;
static int16_t dig_H5;
static int16_t dig_H6;


static void bme280_write(uint8_t reg_addr, uint8_t data)
{
    lp_spi_transaction_t trans_desc = {
        .tx_buffer = &data,
        .tx_length = 1,
        .address = reg_addr & ~(1 << 7), // Clear MSB of register addr to indicate it is a write
        .address_bits = 8,
    };

    esp_err_t err = lp_core_lp_spi_master_transfer(&trans_desc, LP_SPI_TRANS_WAIT_FOREVER);
    if(err != ESP_OK) {
        DEBUG_LOG("Failed to write register: 0x%X, with data = 0x%X\n", reg_addr, data);
        abort();
    }
}

static void bme280_read(uint8_t reg_addr, uint8_t* read_data, size_t read_len)
{
    lp_spi_transaction_t trans_desc = {
        .tx_buffer = read_data,
        .tx_length = read_len,
        .rx_buffer = read_data,
        .rx_length = read_len,
        .address = reg_addr,
        .address_bits = 8,
    };


    esp_err_t err = lp_core_lp_spi_master_transfer(&trans_desc, LP_SPI_TRANS_WAIT_FOREVER);
    if(err != ESP_OK) {
        DEBUG_LOG("Failed to read register: 0x%X, with len = 0x%X\n", reg_addr, read_len);
        abort();
    }
}



void bme280_read_compensation_params()
{
    uint8_t param_buf[6];
    // Temperature compensation params are all in consecutive registers, read them all in one go
    bme280_read(BME280_TRIM_PARAM_TEMP_1_REG, param_buf, 6);
    dig_T1 = (param_buf[1] << 8) | param_buf[0];
    dig_T2 = (param_buf[3] << 8) | param_buf[2];
    dig_T3 = (param_buf[5] << 8) | param_buf[4];

    // Humidity compensation params are in two separate regions, read twice
    bme280_read(BME280_TRIM_PARAM_HUM_1_REG, param_buf, 1);
    dig_H1 = param_buf[0];

    bme280_read(BME280_TRIM_PARAM_HUM_2_REG, param_buf, 7);
    dig_H2 = (param_buf[1] << 8) | param_buf[0];
    dig_H3 = param_buf[2];
    dig_H4 = (param_buf[3] << 4) | (param_buf[4] & 0xF);
    dig_H5 = (param_buf[4] >> 4) | (param_buf[5] << 4);
    dig_H6 = param_buf[6];

}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
// Function is taken from BME280 datasheet
static int32_t convert_temp(int32_t adc_t)
{
    int32_t var1, var2, T;
    var1 = ((((adc_t >> 3) - ((int32_t)dig_T1<<1)))*((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_t>>4) - ((int32_t)dig_T1)) * ((adc_t>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
// Function is taken from BME280 datasheet
static uint32_t convert_humidity(int32_t adc_h)
{
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_h << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r>>12);
}

static void bme280_read_environment_data(int32_t *temperature, uint32_t *humidity)
{
    uint8_t ctrl_hum = BME280_HUM_OVERSAMPLING_1X << BME280_HUM_OVERSAMPLING_BIT;
    bme280_write(BME280_CTRL_HUM_REG, ctrl_hum);

    uint8_t ctrl_meas = (BME280_TEMP_OVERSAMPLING_1X << BME280_TEMP_OVERSAMPLING_BIT) | (BME280_MODE_FORCED << BME280_MODE_BIT);
    bme280_write(BME280_CTRL_MEAS_REG, ctrl_meas);

    bool measuring = true;
    while(measuring) {
        uint8_t status = 0;
        bme280_read(BME280_STATUS_REG, &status, 1);
        measuring = status & (1 << BME280_MEASURING_BIT);
    }

    uint8_t env_data_buf[8] = {};
    bme280_read(BME280_TEMPERATURE_MSB_REG, env_data_buf, 5);
    uint32_t adc_temp = (env_data_buf[0] << 12) | (env_data_buf[1] << 4) | (env_data_buf[2] >> 4);
    uint32_t adc_hum  = (env_data_buf[3] << 8) | (env_data_buf[4]);
    *temperature = convert_temp(adc_temp);
    *humidity = convert_humidity(adc_hum);
}


static void init_sensor(void)
{
    bme280_write(BME280_RESET_REG, BME280_RESET_VAL);
    // Give the sensor some time to reset
    ulp_lp_core_delay_us(2000);

    uint8_t chip_id = 0;
    bme280_read(BME280_CHIP_ID_REG, &chip_id, sizeof(chip_id));
    DEBUG_LOG("Read chip id = 0x%X, expected 0x%X\n", chip_id, BME280_CHIP_ID_VAL);
    if(chip_id != BME280_CHIP_ID_VAL) {
        abort();
    }
}

int32_t temperature;
uint32_t humidity;

int main (void)
{
    if(start_up) {
        init_sensor();
        bme280_read_compensation_params();
        start_up = false;
    }

    bme280_read_environment_data(&temperature, &humidity);
    DEBUG_LOG("Temperature: %d.%d degree Celsius, humidity: %d.%d\%%RH\n", temperature / 100, temperature % 100, humidity / 1024, humidity % 1024);

    if(temperature/100 > WAKEUP_HP_CPU_LIMIT_CELSIUS) {
        ulp_lp_core_wakeup_main_processor();
    }

    return 0;
}
