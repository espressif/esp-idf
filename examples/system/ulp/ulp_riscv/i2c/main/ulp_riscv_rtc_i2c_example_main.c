/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP RISC-V RTC I2C example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "esp_sleep.h"
#include "ulp_riscv.h"
#include "ulp_riscv_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ulp_main.h"
#include "bmp180_defs.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

/************************************************
 * ULP utility APIs
 ************************************************/
static void init_ulp_program(void);

/************************************************
 * RTC I2C utility APIs
 ************************************************/
static void init_i2c(void);

void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(1000));

    uint8_t data_rd = 0;
    int16_t ut_data = 0;
    int32_t up_data = 0;
    int32_t temperature = 0;
    int32_t pressure = 0;
    oss_mode_t oss_mode;

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    /* Not a wakeup from ULP
     * Initialize RTC I2C
     * Setup BMP180 sensor
     * Store current temperature and pressure values
     * Load the ULP firmware
     * Go to deep sleep
     */
    if (cause != ESP_SLEEP_WAKEUP_ULP) {
        printf("Not a ULP-RISC V wakeup (cause = %d)\n", cause);

        /* Initialize RTC I2C */
        init_i2c();

        /* Configure I2C slave address */
        ulp_riscv_i2c_master_set_slave_addr(BMP180_SENSOR_I2C_ADDR);

        /* Reset the BMP180 sensor*/
        ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_SOFT_RESET);
        uint8_t data_wr = BMP180_SENSOR_CMD_SOFT_RESET;
        ulp_riscv_i2c_master_write_to_device(&data_wr, 1);

        /* Confirm that the sensor is alive
         * The BMP180 returns the chip id 0x55 on quering reg addr 0xD0
         */
        ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_WHO_AM_I);
        ulp_riscv_i2c_master_read_from_device(&data_rd, 1);
        if (data_rd != BMP180_SENSOR_CHIP_ID) {
            printf("ERROR: Cannot communicate with I2C sensor\n");
            abort();
        }

        /* Read the calibration data */
        printf("Reading calibration data from BMP180 ...\n");
        bmp180_read_cal_data();
        printf("\n");

        /* Read uncompensated temperature and pressure */
        printf("Reading initial uncompensated temperature and pressure data ...\n");
        ut_data = 0;
        up_data = 0;
        oss_mode = EXAMPLE_OSS_MODE;
        bmp180_read_ut_data(&ut_data);
        bmp180_read_up_data(&up_data, oss_mode);
        printf("Uncompensated Temperature = %d\n", ut_data);
        printf("Uncompensated Pressure = %"PRIu32"\n", up_data);
        printf("\n");

        /* Calculate real temperature value */
        temperature = bmp180_calculate_real_temp((int32_t)ut_data);
        printf("Real Temperature = %f deg celcius\n", (float)(temperature/10.0));

        /* Calculate real pressure value */
        pressure = bmp180_calculate_real_pressure(up_data, (int32_t)ut_data, oss_mode);
        printf("Real Pressure = %f hPa\n", pressure / 100.0);
        printf("\n");

        /* Load ULP firmware
         *
         * The ULP is responsible of monitoring the temperature and pressure values
         * periodically. It will wakeup the main CPU if the temperature and pressure
         * values are above a certain threshold.
         */
        init_ulp_program();
    }

    /* ULP RISC-V read and detected a temperature or pressure above the limit */
    if (cause == ESP_SLEEP_WAKEUP_ULP) {
        printf("ULP RISC-V woke up the main CPU\n");

        /* Pause ULP while we are using the RTC I2C from the main CPU */
        ulp_timer_stop();
        ulp_riscv_halt();

        printf("Uncompensated Temperature = %"PRIu32"\n", ulp_ut_data);
        printf("Uncompensated Pressure = %"PRIu32"\n", ulp_up_data);

        /* Read the calibration data again */
        printf("Reading calibration data from BMP180 ...\n");
        bmp180_read_cal_data();
        printf("\n");

        /* Calculate real temperature and pressure again */
        temperature = 0;
        temperature = bmp180_calculate_real_temp((int32_t)ulp_ut_data);
        printf("New Real Temperature = %f deg celcius\n", (float)(temperature/10.0));

        /* Calculate real pressure value */
        pressure = 0;
        pressure = bmp180_calculate_real_pressure(ulp_up_data, (int32_t)ulp_ut_data, oss_mode);
        printf("New Real Pressure = %f hPa\n", pressure / 100.0);

        /* Resume ULP and go to deep sleep again */
        ulp_timer_resume();
    }


    /* Add a delay for everything to the printed before heading in to deep sleep */
    vTaskDelay(100);

    /* Go back to sleep, only the ULP RISC-V will run */
    printf("Entering deep sleep\n\n");

    /* RTC peripheral power domain needs to be kept on to keep RTC I2C related configs during sleep */
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());

    esp_deep_sleep_start();
}

static void init_i2c(void)
{
    /* Configure RTC I2C */
    printf("Initializing RTC I2C ...\n");
    ulp_riscv_i2c_cfg_t i2c_cfg = ULP_RISCV_I2C_DEFAULT_CONFIG();
    esp_err_t ret = ulp_riscv_i2c_master_init(&i2c_cfg);
    if (ret!= ESP_OK) {
        printf("ERROR: Failed to initialize RTC I2C. Aborting...\n");
        abort();
    }
}

static void bmp180_read16(uint16_t *data_out, uint32_t reg_msb, uint32_t reg_lsb)
{
    uint8_t data_rd = 0;
    *data_out = 0;

    ulp_riscv_i2c_master_set_slave_reg_addr(reg_msb);
    ulp_riscv_i2c_master_read_from_device(&data_rd, 1);
    *data_out |= (uint16_t)(data_rd << 8);
    ulp_riscv_i2c_master_set_slave_reg_addr(reg_lsb);
    data_rd = 0;
    ulp_riscv_i2c_master_read_from_device(&data_rd, 1);
    *data_out |= (uint16_t)(data_rd);
}

static void bmp180_read_cal_data(void)
{
    /* AC1 */
    bmp180_read16((uint16_t *)&bmp180_cal_data.ac1, BMP180_SENSOR_REG_ADDR_AC1_MSB, BMP180_SENSOR_REG_ADDR_AC1_LSB);
    printf("ac1 = %d\n", bmp180_cal_data.ac1);

    /* AC2 */
    bmp180_read16((uint16_t *)&bmp180_cal_data.ac2, BMP180_SENSOR_REG_ADDR_AC2_MSB, BMP180_SENSOR_REG_ADDR_AC2_LSB);
    printf("ac2 = %d\n", bmp180_cal_data.ac2);

    /* AC3 */
    bmp180_read16((uint16_t *)&bmp180_cal_data.ac3, BMP180_SENSOR_REG_ADDR_AC3_MSB, BMP180_SENSOR_REG_ADDR_AC3_LSB);
    printf("ac3 = %d\n", bmp180_cal_data.ac3);

    /* AC4 */
    bmp180_read16(&bmp180_cal_data.ac4, BMP180_SENSOR_REG_ADDR_AC4_MSB, BMP180_SENSOR_REG_ADDR_AC4_LSB);
    printf("ac4 = %u\n", bmp180_cal_data.ac4);

    /* AC5 */
    bmp180_read16(&bmp180_cal_data.ac5, BMP180_SENSOR_REG_ADDR_AC5_MSB, BMP180_SENSOR_REG_ADDR_AC5_LSB);
    printf("ac5 = %u\n", bmp180_cal_data.ac5);

    /* AC6 */
    bmp180_read16(&bmp180_cal_data.ac6, BMP180_SENSOR_REG_ADDR_AC6_MSB, BMP180_SENSOR_REG_ADDR_AC6_LSB);
    printf("ac6 = %u\n", bmp180_cal_data.ac6);

    /* B1 */
    bmp180_read16((uint16_t *)&bmp180_cal_data.b1, BMP180_SENSOR_REG_ADDR_B1_MSB, BMP180_SENSOR_REG_ADDR_B1_LSB);
    printf("b1 = %d\n", bmp180_cal_data.b1);

    /* B2 */
    bmp180_read16((uint16_t *)&bmp180_cal_data.b2, BMP180_SENSOR_REG_ADDR_B2_MSB, BMP180_SENSOR_REG_ADDR_B2_LSB);
    printf("b2 = %d\n", bmp180_cal_data.b2);

    /* MB */
    bmp180_read16((uint16_t *)&bmp180_cal_data.mb, BMP180_SENSOR_REG_ADDR_MB_MSB, BMP180_SENSOR_REG_ADDR_MB_LSB);
    printf("mb = %d\n", bmp180_cal_data.mb);

    /* MC */
    bmp180_read16((uint16_t *)&bmp180_cal_data.mc, BMP180_SENSOR_REG_ADDR_MC_MSB, BMP180_SENSOR_REG_ADDR_MC_LSB);
    printf("mc = %d\n", bmp180_cal_data.mc);

    /* MD */
    bmp180_read16((uint16_t *)&bmp180_cal_data.md, BMP180_SENSOR_REG_ADDR_MD_MSB, BMP180_SENSOR_REG_ADDR_MD_LSB);
    printf("md = %d\n", bmp180_cal_data.md);
}

static void bmp180_read_ut_data(int16_t *ut_data)
{
    /* Set slave register address to the control register */
    ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_CTRL_REG);

    /* Setup control register to read temperature */
    uint8_t cmd = BMP180_SENSOR_CMD_READ_TEMPERATURE;
    ulp_riscv_i2c_master_write_to_device(&cmd, 1);

    /* Wait at least 4.5 milliseconds for the sensor to complete the reading */
    vTaskDelay(pdMS_TO_TICKS(5));

    /* Read uncompensated temperature data */
    bmp180_read16((uint16_t *)ut_data, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_MSB, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_LSB);
}

static int32_t computeb5(int32_t ut_data)
{
    int32_t x1 = (ut_data - (int32_t)bmp180_cal_data.ac6) * ((int32_t)bmp180_cal_data.ac5) >> 15;
    int32_t x2 = ((int32_t)bmp180_cal_data.mc << 11) / (x1 + (int32_t)bmp180_cal_data.md);
    return x1 + x2;
}

static int32_t bmp180_calculate_real_temp(int32_t ut_data)
{
    int32_t b5 = computeb5(ut_data);
    int32_t t = (b5 + 8) >> 4;

    return t;
}

static void bmp180_read_up_data(int32_t *up_data, oss_mode_t oss_mode)
{
    uint16_t press_high;
    uint8_t press_low;

    /* Set slave register address to the control register */
    ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_CTRL_REG);

    /* Setup control register to read pressure */
    uint8_t cmd = 0;
    uint8_t wait = 0;
    switch(oss_mode)
    {
        case OSS_0:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_0;
            wait = 5; // Wait atleast 4.5 msec
            break;
        case OSS_1:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_1;
            wait = 8; // Wait atleast 7.5 msec
            break;
        case OSS_2:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_2;
            wait = 14; // Wait atleast 13.5 msec
            break;
        case OSS_3:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_3;
            wait = 26; // Wait atleast 25.5 msec
            break;
    }

    ulp_riscv_i2c_master_write_to_device(&cmd, 1);

    /* Wait for the required amount of time for the sensor to complete the reading */
    vTaskDelay(pdMS_TO_TICKS(wait));

    /* Read uncompensated temperature data */

    /* Read MSB + LSB */
    bmp180_read16(&press_high, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_MSB, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_LSB);

    /* Read XLSB */
    ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_SENSOR_DATA_XLSB);
    ulp_riscv_i2c_master_read_from_device(&press_low, 1);

    *up_data = (((uint32_t)press_high << 8) + (uint32_t)press_low) >> (8 - oss_mode);
}

static int32_t bmp180_calculate_real_pressure(int32_t up_data, int32_t ut_data, oss_mode_t oss_mode)
{
    int32_t p, x1, x2, x3, b3, b5, b6;
    uint32_t b4, b7;

    b5 = computeb5(ut_data);
    b6 = b5 - 4000;
    x1 = (bmp180_cal_data.b2 * ((b6 * b6) >> 12)) >> 11;
    x2 = (bmp180_cal_data.ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((int32_t) bmp180_cal_data.ac1) * 4 + x3) << oss_mode) + 2) >> 2;
    x1 = (bmp180_cal_data.ac3 * b6) >> 13;
    x2 = (bmp180_cal_data.b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (bmp180_cal_data.ac4 * (uint32_t) (x3 + 32768)) >> 15;
    b7 = ((uint32_t) (up_data - b3) * (50000 >> oss_mode));

    if (b7 < 0x80000000) {
        p = (b7 << 1) / b4;
    } else {
        p = (b7 / b4) << 1;
    }

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p = p + ((x1 + x2 + 3791) >> 4);

    return p;
}

static void init_ulp_program(void)
{
    esp_err_t err = ulp_riscv_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* The first argument is the period index, which is not used by the ULP-RISC-V timer
     * The second argument is the period in microseconds, which gives a wakeup time period of: 40ms
     */
    ulp_set_wakeup_period(0, 40000);

    /* Start the program */
    err = ulp_riscv_run();
    ESP_ERROR_CHECK(err);
}
