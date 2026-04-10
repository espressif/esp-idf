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
#include "test_i3c_board.h"
#include "driver/uart.h"
#include "driver/gpio.h"

static const char TAG[] = "test-i3c";

TEST_CASE("I3C bus install-uninstall test", "[i3c]")
{
    i3c_master_bus_config_t i3c_bus_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 30,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 0,
        .i3c_scl_freq_hz_pp = 0,
        .i3c_scl_pp_duty_cycle = 0.5,
        .i3c_scl_od_duty_cycle = 0.5,
        .i3c_sda_od_hold_time_ns = 25,
        .i3c_sda_pp_hold_time_ns = 0,
        .entdaa_device_num = 5,
        .internal_pullup_resistor_val = I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K,
        .flags = {
            .enable_async_trans = 0,
            .ibi_rstart_trans_en = 0,
            .ibi_silent_sir_rejected = 0,
            .ibi_no_auto_disable = 0,
        }
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
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 30,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 0,
        .i3c_scl_freq_hz_pp = 0,
        .i3c_scl_pp_duty_cycle = 0.5,
        .i3c_scl_od_duty_cycle = 0.5,
        .i3c_sda_od_hold_time_ns = 25,  // 1 clock cycle at 40MHz XTAL
        .i3c_sda_pp_hold_time_ns = 0,
        .entdaa_device_num = 5,
        .internal_pullup_resistor_val = I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K,
        .flags = {
            .enable_async_trans = 0,
            .ibi_rstart_trans_en = 0,
            .ibi_silent_sir_rejected = 0,
            .ibi_no_auto_disable = 0,
        }
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
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 30,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 0,
        .i3c_scl_freq_hz_pp = 0,
        .i3c_scl_pp_duty_cycle = 0.5,
        .i3c_scl_od_duty_cycle = 0.5,
        .i3c_sda_od_hold_time_ns = 25,  // 1 clock cycle at 40MHz XTAL
        .i3c_sda_pp_hold_time_ns = 0,
        .entdaa_device_num = 5,
        .internal_pullup_resistor_val = I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K,
        .flags = {
            .enable_async_trans = 1,
            .ibi_rstart_trans_en = 0,
            .ibi_silent_sir_rejected = 0,
            .ibi_no_auto_disable = 0,
        }
    };
    i3c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i3c_new_master_bus(&i2c_mst_config_1, &bus_handle));

    i3c_device_i2c_config_t dev_cfg_1 = {
        .device_address = 0x10,
        .scl_freq_hz = 100 * 1000
    };
    i3c_master_i2c_device_handle_t dev_1;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_1, &dev_1));

    i3c_device_i2c_config_t dev_cfg_2 = {
        .device_address = 0x20,
        .scl_freq_hz = 100 * 1000
    };
    i3c_master_i2c_device_handle_t dev_2;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_2, &dev_2));

    i3c_device_i2c_config_t dev_cfg_3 = {
        .device_address = 0x30,
        .scl_freq_hz = 100 * 1000
    };
    i3c_master_i2c_device_handle_t dev_3;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_3, &dev_3));

    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_1));
    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_2));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i3c_del_master_bus(bus_handle));
    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_3));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE("I3C master clock frequency test", "[i3c]")
{
    uint8_t data_wr[500] = { 0 };

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 0,
        .i3c_scl_freq_hz_pp = 0,
        .i3c_scl_pp_duty_cycle = 0.5,
        .i3c_scl_od_duty_cycle = 0.5,
        .i3c_sda_od_hold_time_ns = 25,  // 1 clock cycle at 40MHz XTAL
        .i3c_sda_pp_hold_time_ns = 0,
        .entdaa_device_num = 5,
        .internal_pullup_resistor_val = I3C_MASTER_INTERNAL_PULLUP_RESISTOR_0_3K,
        .flags = {
            .enable_async_trans = 0,
            .ibi_rstart_trans_en = 0,
            .ibi_silent_sir_rejected = 0,
            .ibi_no_auto_disable = 0,
        }
    };
    i3c_master_bus_handle_t bus_handle;
    gpio_pullup_en(I3C_MASTER_SCL_IO);
    gpio_pullup_en(I3C_MASTER_SDA_IO);

    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    i3c_device_i2c_config_t dev_cfg_1 = {
        .device_address = TEST_STATIC_ADDRESS,
        .scl_freq_hz = 400 * 1000
    };
    i3c_master_i2c_device_handle_t dev_1;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg_1, &dev_1));

    uart_bitrate_detect_config_t conf = {};
    conf.rx_io_num = I3C_MASTER_SCL_IO;
    conf.source_clk = UART_SCLK_DEFAULT;
    uart_bitrate_res_t res = {};
    uart_detect_bitrate_start(UART_NUM_1, &conf);

    i3c_master_i2c_device_transmit(dev_1, data_wr, 50, -1);

    vTaskDelay(pdMS_TO_TICKS(50));
    uart_detect_bitrate_stop(UART_NUM_1, true, &res);

    int freq_hz = res.clk_freq_hz / res.pos_period;
    printf("The tested I3C SCL frequency(fm) is %d\n", freq_hz);
    TEST_ASSERT_INT_WITHIN(500, 400000, freq_hz);

    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev_1));

    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}
