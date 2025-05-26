/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "esp_err.h"
#include "driver/i3c_master.h"
#include "driver/i3c_master_i2c.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "test_utils.h"

static const char TAG[] = "test-i3c";

TEST_CASE("I3C bus install-uninstall test", "[i3c]")
{
    i3c_master_bus_config_t i3c_bus_config = {
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .scl_io_num = 5,
        .sda_io_num = 6,
        .trans_queue_depth = 30,
    };
    i3c_master_bus_handle_t bus_handle;

    // Install master bus 0
    ESP_LOGI(TAG, "Initialize bus0");
    TEST_ESP_OK(i3c_new_master_bus(&i3c_bus_config, &bus_handle));

    // Install master bus 0 again
    ESP_LOGI(TAG, "Initialize bus0 again");
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i3c_new_master_bus(&i3c_bus_config, &bus_handle));
    ESP_LOGI(TAG, "Delete bus0");
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));

}

TEST_CASE("I3C driver memory leaking check", "[i3c]")
{
    i3c_master_bus_config_t i3c_bus_config = {
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .scl_io_num = 5,
        .sda_io_num = 6,
        .trans_queue_depth = 30,
    };
    i3c_master_bus_handle_t bus_handle;

    int size = esp_get_free_heap_size();
    for (uint32_t i = 0; i <= 5; i++) {
        TEST_ESP_OK(i3c_new_master_bus(&i3c_bus_config, &bus_handle));
        vTaskDelay(10 / portTICK_PERIOD_MS);
        TEST_ESP_OK(i3c_del_master_bus(bus_handle));
    }

    TEST_ASSERT_INT_WITHIN(300, size, esp_get_free_heap_size());
}

TEST_CASE("I3C device add & remove check", "[i3c]")
{
    i3c_master_bus_config_t i2c_mst_config_1 = {
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .scl_io_num = 5,
        .sda_io_num = 6,
        .flags.enable_async_trans = true,
        .trans_queue_depth = 30,
    };
    i3c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i3c_new_master_bus(&i2c_mst_config_1, &bus_handle));

    i3c_device_i2c_config_t dev_cfg_1 = {
        .scl_freq_hz = 100 * 1000,
        .device_address = 0x10,
    };
    i3c_master_i2c_device_handle_t dev_1;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_1, &dev_1));

    i3c_device_i2c_config_t dev_cfg_2 = {
        .scl_freq_hz = 100 * 1000,
        .device_address = 0x20,
    };
    i3c_master_i2c_device_handle_t dev_2;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_2, &dev_2));

    i3c_device_i2c_config_t dev_cfg_3 = {
        .scl_freq_hz = 100 * 1000,
        .device_address = 0x30,
    };
    i3c_master_i2c_device_handle_t dev_3;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_3, &dev_3));

    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_1));
    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_2));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i3c_del_master_bus(bus_handle));
    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_3));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}
