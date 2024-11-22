/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "soc/gpio_periph.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "hal/gpio_hal.h"
#include "hal/uart_ll.h"
#include "esp_private/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_rom_gpio.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_board.h"

#define DATA_LENGTH 100

static QueueHandle_t s_receive_queue;

TEST_CASE("LP I2C initialize on i2c slave", "[i2c]")
{
    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = LP_I2C_SCLK_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .send_buf_depth = 256,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, i2c_new_slave_device(&i2c_slv_config, &slave_handle));
}

#if CONFIG_IDF_TARGET_ESP32C6

TEST_CASE("LP I2C initialize with wrong IO", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config = {
        .lp_source_clk = LP_I2C_SCLK_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .scl_io_num = 5,
        .sda_io_num = 6,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

#endif

TEST_CASE("LP I2C initialize with wrong clock source", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config = {
        .lp_source_clk = I2C_CLK_SRC_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}

static IRAM_ATTR bool test_i2c_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void lp_i2c_master_write_test(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {
        .lp_source_clk = LP_I2C_SCLK_DEFAULT,
        .i2c_port = LP_I2C_NUM_0,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    disp_buf(data_wr, i);
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    unity_wait_for_signal("ready to delete");
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void hp_i2c_slave_read_test(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = LP_I2C_SCL_IO,
        .sda_io_num = LP_I2C_SDA_IO,
        .slave_addr = 0x58,
    };

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    s_receive_queue = xQueueCreate(1, sizeof(i2c_slave_rx_done_event_data_t));
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = test_i2c_rx_done_callback,
    };
    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, s_receive_queue));

    i2c_slave_rx_done_event_data_t rx_data;
    TEST_ESP_OK(i2c_slave_receive(slave_handle, data_rd, DATA_LENGTH));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    xQueueReceive(s_receive_queue, &rx_data, pdMS_TO_TICKS(10000));
    disp_buf(data_rd, DATA_LENGTH);
    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(data_rd[i] == i);
    }
    vQueueDelete(s_receive_queue);
    unity_send_signal("ready to delete");
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("LP_I2C master write slave test", "[i2c][test_env=generic_multi_device][timeout=150]", lp_i2c_master_write_test, hp_i2c_slave_read_test);
