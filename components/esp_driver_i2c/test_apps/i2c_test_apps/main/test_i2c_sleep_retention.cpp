/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "test_board.h"

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_log.h"
#include "test_utils.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_pm.h"

#define DATA_LENGTH 100

static QueueHandle_t event_queue;
static uint8_t *temp_data;
static size_t temp_len = 0;

static IRAM_ATTR bool i2c_slave_request_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_request_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_TX;
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static IRAM_ATTR bool i2c_slave_receive_cb(i2c_slave_dev_handle_t i2c_slave, const i2c_slave_rx_done_event_data_t *evt_data, void *arg)
{
    BaseType_t xTaskWoken;
    i2c_slave_event_t evt = I2C_SLAVE_EVT_RX;
    memcpy(temp_data, evt_data->buffer, evt_data->length);
    temp_len = evt_data->length;
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void i2c_master_write_sleep_retention_test(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i;

    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = TEST_I2C_PORT;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.flags.enable_internal_pullup = true;
    i2c_mst_config.flags.allow_pd = true;

    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = 0x58;
    dev_cfg.scl_speed_hz = 100000;

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    unity_wait_for_signal("i2c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    disp_buf(data_wr, i);
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    unity_wait_for_signal("i2c slave receive once, master to sleep");

#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(3 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up!!\n");
    unity_send_signal("master sleep-wakeup");
    unity_wait_for_signal("i2c slave receive again");

    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    disp_buf(data_wr, i);
    unity_send_signal("master write again");

    unity_wait_for_signal("ready to delete");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void i2c_slave_read_sleep_retention_test(void)
{
    event_queue = xQueueCreate(5, sizeof(i2c_slave_event_t));
    assert(event_queue);
    temp_data = static_cast<uint8_t*>(heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT));
    assert(temp_data);

    i2c_slave_config_t i2c_slv_config = {};
    i2c_slv_config.i2c_port = TEST_I2C_PORT;
    i2c_slv_config.sda_io_num = I2C_SLAVE_SDA_IO;
    i2c_slv_config.scl_io_num = I2C_SLAVE_SCL_IO;
    i2c_slv_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_slv_config.send_buf_depth = DATA_LENGTH;
    i2c_slv_config.receive_buf_depth = DATA_LENGTH;
    i2c_slv_config.slave_addr = 0x58;
    i2c_slv_config.flags.enable_internal_pullup = true;

    i2c_slave_dev_handle_t slave_handle;
    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config, &slave_handle));

    i2c_slave_event_callbacks_t cbs = {};
    cbs.on_request = i2c_slave_request_cb;
    cbs.on_receive = i2c_slave_receive_cb;

    ESP_ERROR_CHECK(i2c_slave_register_event_callbacks(slave_handle, &cbs, NULL));

    unity_send_signal("i2c slave init finish");

    unity_wait_for_signal("master write");
    i2c_slave_event_t evt;
    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_RX) {
            disp_buf(temp_data, temp_len);
            printf("length is %x\n", temp_len);
            for (int i = 0; i < temp_len; i++) {
                TEST_ASSERT(temp_data[i] == i);
            }
        }
    }

    unity_send_signal("i2c slave receive once, master to sleep");
    // Slave sleep as well..
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    unity_wait_for_signal("master sleep-wakeup");

    unity_send_signal("i2c slave receive again");

    unity_wait_for_signal("master write again");

    if (xQueueReceive(event_queue, &evt, portMAX_DELAY) == pdTRUE) {
        if (evt == I2C_SLAVE_EVT_RX) {
            disp_buf(temp_data, temp_len);
            printf("length is %x\n", temp_len);
            for (int i = 0; i < temp_len; i++) {
                TEST_ASSERT(temp_data[i] == i);
            }
        }
    }

    vQueueDelete(event_queue);
    unity_send_signal("ready to delete");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
    TEST_ESP_OK(i2c_del_slave_device(slave_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I2C sleep retention test", "[i2c][test_env=generic_multi_device][timeout=150]", i2c_master_write_sleep_retention_test, i2c_slave_read_sleep_retention_test);
