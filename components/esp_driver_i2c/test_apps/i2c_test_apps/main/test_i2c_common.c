/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"
#include "hal/uart_ll.h"
#include "esp_private/periph_ctrl.h"
#include "driver/i2c_master.h"
#include "driver/i2c_slave.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_board.h"
#include "driver/uart.h"

static const char TAG[] = "test-i2c";

// Make it as a local test, because don't know where there happened to be any pull-up on CI.
TEST_CASE("I2C master initialize without pins pull-up ", "[i2c][ignore]")
{
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = false,  // no pull-up
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, i2c_master_probe(bus_handle, 0x22, 20));
    vTaskDelay(1000);
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

TEST_CASE("I2C bus install-uninstall test", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_mst_handle1;

#if SOC_HP_I2C_NUM > 1
    i2c_master_bus_config_t i2c_mst_config_2 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 1,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_mst_handle2;
#endif
    // Install master bus 0
    ESP_LOGI(TAG, "Initialize bus0");
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &i2c_mst_handle1));
#if SOC_HP_I2C_NUM > 1
    // Install master bus 1
    ESP_LOGI(TAG, "Initialize bus1");
    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_2, &i2c_mst_handle2));
#endif
    // Install master bus 0 again
    ESP_LOGI(TAG, "Initialize bus0 again");
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_new_master_bus(&i2c_mst_config_1, &i2c_mst_handle1));
    ESP_LOGI(TAG, "Delete bus0");
    TEST_ESP_OK(i2c_del_master_bus(i2c_mst_handle1));
#if SOC_HP_I2C_NUM > 1
    ESP_LOGI(TAG, "Delete bus1");
    TEST_ESP_OK(i2c_del_master_bus(i2c_mst_handle2));
#endif
}

TEST_CASE("I2C driver memory leaking check", "[i2c]")
{
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
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
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));

    i2c_device_config_t dev_cfg_1 = {
        .scl_speed_hz = 100 * 1000,
        .device_address = 0x10,
    };
    i2c_master_dev_handle_t dev_1;
    i2c_master_bus_add_device(bus_handle, &dev_cfg_1, &dev_1);

    i2c_device_config_t dev_cfg_2 = {
        .scl_speed_hz = 100 * 1000,
        .device_address = 0x20,
    };
    i2c_master_dev_handle_t dev_2;
    i2c_master_bus_add_device(bus_handle, &dev_cfg_2, &dev_2);

    i2c_device_config_t dev_cfg_3 = {
        .scl_speed_hz = 100 * 1000,
        .device_address = 0x30,
    };
    i2c_master_dev_handle_t dev_3;
    i2c_master_bus_add_device(bus_handle, &dev_cfg_3, &dev_3);

    i2c_master_bus_rm_device(dev_1);
    i2c_master_bus_rm_device(dev_2);

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_del_master_bus(bus_handle));
    i2c_master_bus_rm_device(dev_3);
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

TEST_CASE("I2C peripheral allocate all", "[i2c]")
{
    i2c_master_bus_handle_t bus_handle[SOC_HP_I2C_NUM];
    for (int i = 0; i < SOC_HP_I2C_NUM; i++) {
        i2c_master_bus_config_t i2c_mst_config_1 = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = -1,
            .scl_io_num = I2C_MASTER_SCL_IO,
            .sda_io_num = I2C_MASTER_SDA_IO,
            .flags.enable_internal_pullup = true,
        };

        TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle[i]));
    }
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle_2;

    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_new_master_bus(&i2c_mst_config_1, &bus_handle_2));

    for (int i = 0; i < SOC_HP_I2C_NUM; i++) {
        TEST_ESP_OK(i2c_del_master_bus(bus_handle[i]));
    }

    // Get another one

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle_2));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle_2));
}

TEST_CASE("I2C master clock frequency test", "[i2c]")
{
    uint8_t data_wr[500] = { 0 };

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
        .trans_queue_depth = 30,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
        .flags.disable_ack_check = 1,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    TEST_ESP_OK(i2c_master_transmit(dev_handle, data_wr, 500, 0));

    uart_bitrate_detect_config_t conf = {
        .rx_io_num = I2C_MASTER_SCL_IO,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_bitrate_res_t res = {};
    uart_detect_bitrate_start(UART_NUM_1, &conf);
    vTaskDelay(pdMS_TO_TICKS(50));
    uart_detect_bitrate_stop(UART_NUM_1, true, &res);

    int freq_hz = res.clk_freq_hz / res.pos_period;
    printf("The tested I2C SCL frequency is %d\n", freq_hz);
    TEST_ASSERT_INT_WITHIN(500, 100000, freq_hz);

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

TEST_CASE("I2C master probe device test", "[i2c]")
{
    // 0x22,33,44,55 does not exist on the I2C bus, so it's expected to return `not found` error
    // TODO: Add exist slave for testing probe success after i2c slave is merged.
    i2c_master_bus_config_t i2c_mst_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config_1, &bus_handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_master_probe(bus_handle, 0x22, -1));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_master_probe(bus_handle, 0x33, -1));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_master_probe(bus_handle, 0x44, -1));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_master_probe(bus_handle, 0x55, -1));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

TEST_CASE("probe test after general call (0x00 0x06)", "[i2c]")
{
    uint8_t data_wr[1] = { 0x06 };

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg1 = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x00,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle1;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg1, &dev_handle1));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_transmit(dev_handle1, data_wr, 1, 200));

    for (int i = 1; i < 0x7f; i++) {
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_master_probe(bus_handle, i, 800));
    }

    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle1));

    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

#define LENGTH 48

static IRAM_ATTR bool test_master_tx_done_callback(i2c_master_dev_handle_t i2c_dev, const i2c_master_event_data_t *evt_data, void *arg)
{
    return true;
}

/*******************************************************************************
 *
 * This test aim to test I2C non-blocking transaction function. Several things have been
 * done in this test for testing its memory/concurrency issues.
 *
 * 1. See the depth of queue is 37, but the number of transaction is 42, that means some
 *    queue must be reused.
 * 2. There are some delay randomly set there, for testing the concurrency or any I2C state
 *    might be met.
 *******************************************************************************
*/
TEST_CASE("I2C master transaction non-blocking mode with large amount of transaction", "[i2c]")
{
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .trans_queue_depth = 37,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;

    TEST_ESP_OK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 400000,
        .flags.disable_ack_check = true,
    };

    i2c_master_dev_handle_t dev_handle;
    TEST_ESP_OK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    i2c_master_event_callbacks_t cbs = {
        .on_trans_done = test_master_tx_done_callback,
    };
    i2c_master_register_event_callbacks(dev_handle, &cbs, NULL);

    uint32_t cnt = 7;
    uint8_t *buf[6];
    for (int i = 0; i < 6; i++) {
        buf[i] = (uint8_t*)heap_caps_calloc(1, LENGTH, MALLOC_CAP_8BIT);
        for (int j = 0; j < LENGTH; j++) {
            buf[i][j] = i + j;
        }
    }
    while (cnt--) {
        i2c_master_transmit(dev_handle, buf[0], LENGTH, -1);
        esp_rom_delay_us(1000);
        i2c_master_transmit(dev_handle, buf[1], LENGTH, -1);
        esp_rom_delay_us(500);
        i2c_master_transmit(dev_handle, buf[2], LENGTH, -1);
        esp_rom_delay_us(200);
        i2c_master_transmit(dev_handle, buf[3], LENGTH, -1);
        esp_rom_delay_us(400);
        i2c_master_transmit(dev_handle, buf[4], LENGTH, -1);
        esp_rom_delay_us(700);
        i2c_master_transmit(dev_handle, buf[5], LENGTH, -1);
        esp_rom_delay_us(200);
    }

    i2c_master_bus_wait_all_done(bus_handle, -1);
    for (int i = 0; i < 6; i++) {
        if (buf[i]) {
            free(buf[i]);
        }
    }
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

static void _test_i2c_new_bus_device(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = TEST_I2C_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .flags.enable_internal_pullup = true,
    };

    TEST_ESP_OK(i2c_new_master_bus(&i2c_mst_config, bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 100000,
    };

    TEST_ESP_OK(i2c_master_bus_add_device(*bus_handle, &dev_cfg, dev_handle));
}

static void _test_i2c_del_bus_device(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t dev_handle)
{
    TEST_ESP_OK(i2c_master_bus_rm_device(dev_handle));
    TEST_ESP_OK(i2c_del_master_bus(bus_handle));
}

TEST_CASE("I2C master transaction transmit check nack return value", "[i2c]")
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };

    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    _test_i2c_new_bus_device(&bus_handle, &dev_handle);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_transmit(dev_handle, data_wr, DATA_LENGTH, -1));
    _test_i2c_del_bus_device(bus_handle, dev_handle);
}

TEST_CASE("I2C master transaction transmit receive check nack return value", "[i2c]")
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    uint8_t data_rd[DATA_LENGTH] = { 0 };

    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    _test_i2c_new_bus_device(&bus_handle, &dev_handle);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_transmit_receive(dev_handle, data_wr, DATA_LENGTH, data_rd, DATA_LENGTH, -1));
    _test_i2c_del_bus_device(bus_handle, dev_handle);
}

TEST_CASE("I2C master transaction receive check nack return value", "[i2c]")
{
    uint8_t data_rd[DATA_LENGTH] = { 0 };

    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;

    _test_i2c_new_bus_device(&bus_handle, &dev_handle);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1));
    _test_i2c_del_bus_device(bus_handle, dev_handle);
}

TEST_CASE("Test get handle with known port", "[i2c]")
{
    i2c_master_bus_handle_t handle;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, i2c_master_get_bus_handle(10, &handle));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, i2c_master_get_bus_handle(0, &handle));

    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    _test_i2c_new_bus_device(&bus_handle, &dev_handle);
    TEST_ESP_OK(i2c_master_get_bus_handle(0, &handle));

    // Check the handle retrieved is as same as original handle
    TEST_ASSERT((uint32_t)bus_handle == (uint32_t)handle);
    _test_i2c_del_bus_device(bus_handle, dev_handle);
}

#if SOC_I2C_SUPPORT_SLAVE

TEST_CASE("I2C peripheral allocate slave all", "[i2c]")
{
    i2c_slave_dev_handle_t dev_handle[SOC_HP_I2C_NUM];
    for (int i = 0; i < SOC_HP_I2C_NUM; i++) {
        i2c_slave_config_t i2c_slv_config_1 = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .i2c_port = -1,
            .scl_io_num = I2C_SLAVE_SCL_IO,
            .sda_io_num = I2C_SLAVE_SDA_IO,
            .slave_addr = ESP_SLAVE_ADDR,
            .send_buf_depth = DATA_LENGTH,
            .receive_buf_depth = DATA_LENGTH,
            .flags.enable_internal_pullup = true,
        };

        TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config_1, &dev_handle[i]));
    }
    i2c_slave_config_t i2c_slv_config_1 = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .slave_addr = ESP_SLAVE_ADDR,
        .send_buf_depth = DATA_LENGTH,
        .receive_buf_depth = DATA_LENGTH,
        .flags.enable_internal_pullup = true,
    };
    i2c_slave_dev_handle_t dev_handle_2;

    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, i2c_new_slave_device(&i2c_slv_config_1, &dev_handle_2));

    for (int i = 0; i < SOC_HP_I2C_NUM; i++) {
        TEST_ESP_OK(i2c_del_slave_device(dev_handle[i]));
    }

    // Get another one

    TEST_ESP_OK(i2c_new_slave_device(&i2c_slv_config_1, &dev_handle_2));
    TEST_ESP_OK(i2c_del_slave_device(dev_handle_2));
}

#endif // SOC_I2C_SUPPORT_SLAVE
