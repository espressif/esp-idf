/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "soc/gpio_periph.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "hal/uart_ll.h"
#include "esp_private/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_rom_gpio.h"
#include "esp_log.h"
#include "test_utils.h"

static const char TAG[] = "test-i2c";

#define TEST_I2C_SCL_PIN  2
#define TEST_I2C_SDA_PIN  4
#define TEST_I2C_SCL_FREQ (100 * 1000)
#define TEST_I2C_PORT     0
#define SLAVE_ADDR        0x58


TEST_CASE("I2C bus install-uninstall test", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = TEST_I2C_SCL_PIN,
        .sda_io_num = TEST_I2C_SDA_PIN,
    };
    i2c_master_bus_handle_t i2c_mst_handle1;

#if SOC_I2C_NUM > 1
    i2c_master_bus_config_t i2c_mst_config_2 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 1,
        .scl_io_num = TEST_I2C_SCL_PIN,
        .sda_io_num = TEST_I2C_SDA_PIN,
    };
    i2c_master_bus_handle_t i2c_mst_handle2;
#endif
    // Install master bus 0
    ESP_LOGI(TAG, "Initialize bus0");
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &i2c_mst_handle1));
#if SOC_I2C_NUM > 1
    // Install master bus 1
    ESP_LOGI(TAG, "Initialize bus1");
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_2, &i2c_mst_handle2));
#endif
    // Install master bus 0 again
    ESP_LOGI(TAG, "Initialize bus0 again");
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_new_master_bus(&i2c_mst_config_1, &i2c_mst_handle1));
    ESP_LOGI(TAG, "Delete bus0");
    TEST_ESP_OK(i2c_del_master_bus(i2c_mst_handle1));
#if SOC_I2C_NUM > 1
    ESP_LOGI(TAG, "Delete bus1");
    TEST_ESP_OK(i2c_del_master_bus(i2c_mst_handle2));
#endif
}

TEST_CASE("I2C driver memory leaking check", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = TEST_I2C_SCL_PIN,
        .sda_io_num = TEST_I2C_SDA_PIN,
    };
    i2c_master_bus_handle_t bus_handle;

    int size = esp_get_free_heap_size();
    for (uint32_t i = 0; i <= 5; i++) {
        TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));
        vTaskDelay(10 / portTICK_PERIOD_MS);
        TEST_ESP_OK(i2c_del_master_bus(bus_handle));
    }

    TEST_ASSERT_INT_WITHIN(300, size, esp_get_free_heap_size());
}

TEST_CASE("I2C device add & remove check", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = TEST_I2C_SCL_PIN,
        .sda_io_num = TEST_I2C_SDA_PIN,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));

    i2c_device_config_t dev_cfg_1 = {
        .scl_speed_hz = 100*1000,
        .device_address = 0x10,
    };
    i2c_master_dev_handle_t dev_1;
    i2c_master_bus_add_device(bus_handle, &dev_cfg_1, &dev_1);

    i2c_device_config_t dev_cfg_2 = {
        .scl_speed_hz = 100*1000,
        .device_address = 0x20,
    };
    i2c_master_dev_handle_t dev_2;
    i2c_master_bus_add_device(bus_handle, &dev_cfg_2, &dev_2);

    i2c_device_config_t dev_cfg_3 = {
        .scl_speed_hz = 100*1000,
        .device_address = 0x30,
    };
    i2c_master_dev_handle_t dev_3;
    i2c_master_bus_add_device(bus_handle, &dev_cfg_3, &dev_3);

    i2c_master_bus_rm_device(dev_1);
    i2c_master_bus_rm_device(dev_2);
    i2c_master_bus_rm_device(dev_3);

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}
