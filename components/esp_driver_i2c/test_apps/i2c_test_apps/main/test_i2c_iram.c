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
#include "unity_test_utils_cache.h"

static QueueHandle_t s_send_queue;
static QueueHandle_t s_receive_queue;

static IRAM_ATTR bool test_master_tx_done_callback(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_data_t *evt_data, void *arg)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t s_send_queue = (QueueHandle_t)arg;
    xQueueSendFromISR(s_send_queue, evt_data, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static IRAM_ATTR bool test_i2c_rx_done_callback(i2c_slave_dev_handle_t channel, const i2c_slave_rx_done_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_wakeup = pdFALSE;
    QueueHandle_t receive_queue = (QueueHandle_t)user_data;
    xQueueSendFromISR(receive_queue, edata, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(10000);
}

static void i2c_master_write_test_iram_safe(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
        .trans_queue_depth = 200,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    s_send_queue = xQueueCreate(1, sizeof(i2c_master_event_data_t));
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    i2c_master_event_callbacks_t cbs = {
        .on_trans_done = test_master_tx_done_callback,
    };
    i2c_master_event_data_t evt_data;
    i2c_master_register_event_callbacks(dev_handle, &cbs, s_send_queue);
    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    disp_buf(data_wr, i);
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    xQueueReceive(s_send_queue, &evt_data, pdMS_TO_TICKS(10000));
    unity_wait_for_signal("ready to delete");
    vQueueDelete(s_send_queue);
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_read_test_iram_safe(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};

    i2c_slave_config_t i2c_slv_config = {
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .send_buf_depth = 256,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
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
    unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
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

TEST_CASE_MULTIPLE_DEVICES("I2C master write slave test iram safe", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_test_iram_safe, i2c_slave_read_test_iram_safe);
