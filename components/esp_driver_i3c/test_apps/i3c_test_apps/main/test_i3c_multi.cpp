/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_err.h"
#include "soc/clk_tree_defs.h"
#include "esp_private/periph_ctrl.h"
#include "driver/i3c_master.h"
#include "driver/i3c_master_i2c.h"
#include "hal/i3c_slave_ll.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_i3c_board.h"
#include "esp_private/gpio.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "soc/gpio_sig_map.h"
#include "soc/io_mux_reg.h"
#include "soc/i3c_slv_struct.h"
#include "soc/i3c_slv_reg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/i3c_master_ll.h"

static const char TAG[] = "test-i3c";

static esp_err_t i3c_slave_pins_config(void)
{
    esp_err_t ret = ESP_OK;
    // SDA pin configurations
    gpio_input_enable(I3C_SLAVE_SDA_IO);
    i3c_slave_ll_enable_internal_pullup(I3C_SLAVE_SDA_IO, true);
    i3c_slave_ll_set_internal_pullup_value(I3C_SLAVE_SDA_IO, 0);
    gpio_func_sel(I3C_SLAVE_SDA_IO, PIN_FUNC_GPIO);
    gpio_matrix_output(I3C_SLAVE_SDA_IO, I3C_SLV_SDA_PAD_OUT_IDX, 0, 0);
    gpio_matrix_input(I3C_SLAVE_SDA_IO, I3C_SLV_SDA_PAD_IN_IDX, 0);

    // SCL pin configurations
    gpio_input_enable(I3C_SLAVE_SCL_IO);
    i3c_slave_ll_enable_internal_pullup(I3C_SLAVE_SCL_IO, true);
    i3c_slave_ll_set_internal_pullup_value(I3C_SLAVE_SCL_IO, 0);
    gpio_func_sel(I3C_SLAVE_SCL_IO, PIN_FUNC_GPIO);
    gpio_matrix_output(I3C_SLAVE_SCL_IO, I3C_SLV_SCL_PAD_OUT_IDX, 0, 0);
    gpio_matrix_input(I3C_SLAVE_SCL_IO, I3C_SLV_SCL_PAD_IN_IDX, 0);
    return ret;
}

static void i3c_slave_init(void)
{
    PERIPH_RCC_ATOMIC() {
        i3c_slave_ll_enable_bus_clock(&I3C_SLV, true);
        i3c_slave_ll_reset_register(&I3C_SLV);
    }

    i3c_slave_pins_config();
    i3c_slave_ll_config_static_address(&I3C_SLV, TEST_STATIC_ADDRESS);
    i3c_slave_ll_enable_intr_mask(&I3C_SLV, I3C_SLAVE_LL_EVENT_INTR);

    i3c_slave_ll_unlock_register(&I3C_SLV, true);
    i3c_slave_ll_set_transmit_fifo_threshold(&I3C_SLV, 1);
    i3c_slave_ll_set_receive_fifo_threshold(&I3C_SLV, 1);
}

static void i3c_i2c_slave_read_test(void)
{
    unity_wait_for_signal("i3c master init first");

    uint8_t *temp_data = (uint8_t*)heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT);
    assert(temp_data);

    i3c_slave_init();

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("master write");

    i3c_slave_ll_read_data(&I3C_SLV, temp_data, DATA_LENGTH);

    ESP_LOG_BUFFER_HEX(TAG, temp_data, DATA_LENGTH);

    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(temp_data[i] == i);
    }

    unity_send_signal("ready to delete");
    free(temp_data);
}

static void i3c_i2c_master_write_test(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i = 0;

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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

    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    i3c_device_i2c_config_t dev_cfg = {
        .device_address = TEST_STATIC_ADDRESS,
        .scl_freq_hz = 100 * 1000
    };
    i3c_master_i2c_device_handle_t dev;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg, &dev));

    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_LOG_BUFFER_HEX(TAG, data_wr, i);

    i3c_master_i2c_device_transmit(dev, data_wr, DATA_LENGTH, -1);

    unity_wait_for_signal("ready to delete");

    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C-I2C master write slave test in fifo mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_i2c_master_write_test, i3c_i2c_slave_read_test);

static void i3c_i2c_master_read_slave_test(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    i3c_device_i2c_config_t dev_cfg = {
        .device_address = TEST_STATIC_ADDRESS,
        .scl_freq_hz = 100 * 1000
    };
    i3c_master_i2c_device_handle_t dev;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg, &dev));

    unity_wait_for_signal("i3c slave init finish");

    i3c_master_i2c_device_receive(dev, data_rd, DATA_LENGTH, -1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < DATA_LENGTH; i++) {
        printf("%x\n", data_rd[i]);
        TEST_ASSERT(data_rd[i] == i);
    }
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

static void i3c_i2c_slave_write_buffer_test(void)
{

    uint8_t data_wr[DATA_LENGTH];

    i3c_slave_init();

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    i3c_slave_ll_write_data(&I3C_SLV, data_wr, DATA_LENGTH);

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("ready to delete master read test");
}

TEST_CASE_MULTIPLE_DEVICES("I3C-I2C master read slave test in fifo mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_i2c_master_read_slave_test, i3c_i2c_slave_write_buffer_test);

static void i3c_i2c_master_write_via_dma_test(void)
{
    __attribute__((aligned(4))) uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i = 0;

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    i3c_device_i2c_config_t dev_cfg = {
        .device_address = TEST_STATIC_ADDRESS,
        .scl_freq_hz = 100 * 1000
    };
    i3c_master_i2c_device_handle_t dev;
    TEST_ESP_OK(i3c_master_bus_add_i2c_device(bus_handle, &dev_cfg, &dev));

    i3c_master_dma_config_t dma_config = {
        .max_transfer_size = 50,
        .dma_burst_size = 16,
    };

    i3c_master_bus_decorate_dma(bus_handle, &dma_config);

    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");

    unity_send_signal("master write");
    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_LOG_BUFFER_HEX(TAG, data_wr, i);
    i3c_master_i2c_device_transmit(dev, data_wr, DATA_LENGTH, -1);

    unity_wait_for_signal("ready to delete");

    TEST_ESP_OK(i3c_master_bus_rm_i2c_device(dev));
    TEST_ESP_OK(i3c_master_bus_decorate_dma(bus_handle, NULL));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C-I2C master write slave test in dma mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_i2c_master_write_via_dma_test, i3c_i2c_slave_read_test);

static void i3c_slave_read_test_via_setdasa(void)
{
    unity_wait_for_signal("i3c master init first");

    uint8_t *temp_data = (uint8_t*)heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT);
    assert(temp_data);

    i3c_slave_init();

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("master write");

    i3c_slave_ll_read_data(&I3C_SLV, temp_data, DATA_LENGTH);

    ESP_LOG_BUFFER_HEX(TAG, temp_data, DATA_LENGTH);

    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(temp_data[i] == i);
    }

    unity_send_signal("ready to delete");
    free(temp_data);
}

static void i3c_master_write_test_via_setdasa(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i = 0;

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_PLL_F160M,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 12 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    i3c_device_i3c_config_t dev_cfg = {
        .dynamic_addr = TEST_DYNAMIC_ADDRESS,
        .static_addr = TEST_STATIC_ADDRESS,
    };
    i3c_master_i3c_device_handle_t dev;
    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");
    TEST_ESP_OK(i3c_master_bus_add_i3c_static_device(bus_handle, &dev_cfg, &dev));

    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_LOG_BUFFER_HEX(TAG, data_wr, i);

    i3c_master_i3c_device_transmit(dev, data_wr, DATA_LENGTH, -1);

    unity_send_signal("master write");

    unity_wait_for_signal("ready to delete");

    TEST_ESP_OK(i3c_master_bus_rm_i3c_device(dev));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C master write slave test via SETDASA in fifo mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_master_write_test_via_setdasa, i3c_slave_read_test_via_setdasa);

static void i3c_master_read_slave_test_via_setdasa(void)
{
    uint8_t data_rd[DATA_LENGTH] = {0};
    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    unity_wait_for_signal("i3c slave init finish");
    i3c_device_i3c_config_t dev_cfg = {
        .dynamic_addr = TEST_DYNAMIC_ADDRESS,
        .static_addr = TEST_STATIC_ADDRESS,
    };
    i3c_master_i3c_device_handle_t dev;
    TEST_ESP_OK(i3c_master_bus_add_i3c_static_device(bus_handle, &dev_cfg, &dev));

    i3c_master_i3c_device_receive(dev, data_rd, DATA_LENGTH, -1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    for (int i = 0; i < DATA_LENGTH; i++) {
        printf("%x\n", data_rd[i]);
        TEST_ASSERT(data_rd[i] == i);
    }
    unity_send_signal("ready to delete master read test");

    TEST_ESP_OK(i3c_master_bus_rm_i3c_device(dev));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

static void i3c_slave_write_buffer_test_via_setdasa(void)
{

    uint8_t data_wr[DATA_LENGTH];

    i3c_slave_init();

    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    i3c_slave_ll_write_data(&I3C_SLV, data_wr, DATA_LENGTH);

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("ready to delete master read test");
}

TEST_CASE_MULTIPLE_DEVICES("I3C master read slave test via SETDASA in fifo mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_master_read_slave_test_via_setdasa, i3c_slave_write_buffer_test_via_setdasa);

static void i3c_master_write_via_dma_test_via_setdasa(void)
{
    __attribute__((aligned(4))) uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i = 0;

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");

    i3c_device_i3c_config_t dev_cfg = {
        .dynamic_addr = TEST_DYNAMIC_ADDRESS,
        .static_addr = TEST_STATIC_ADDRESS,
    };
    i3c_master_i3c_device_handle_t dev;
    TEST_ESP_OK(i3c_master_bus_add_i3c_static_device(bus_handle, &dev_cfg, &dev));

    i3c_master_dma_config_t dma_config = {
        .max_transfer_size = 50,
        .dma_burst_size = 16,
    };

    i3c_master_bus_decorate_dma(bus_handle, &dma_config);

    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_LOG_BUFFER_HEX(TAG, data_wr, i);
    i3c_master_i3c_device_transmit(dev, data_wr, DATA_LENGTH, -1);
    unity_send_signal("master write");

    unity_wait_for_signal("ready to delete");

    TEST_ESP_OK(i3c_master_bus_rm_i3c_device(dev));
    TEST_ESP_OK(i3c_master_bus_decorate_dma(bus_handle, NULL));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C master write slave test via SETDASA in dma mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_master_write_via_dma_test_via_setdasa, i3c_slave_read_test_via_setdasa);

static void i3c_slave_for_entdaa(void)
{
    unity_wait_for_signal("i3c master init first");

    uint8_t *temp_data = (uint8_t*)heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT);
    assert(temp_data);

    i3c_slave_init();

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("master write");

    unity_send_signal("ready to delete");
    free(temp_data);
}

static void i3c_master_entdaa_test(void)
{
    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");

    i3c_master_i3c_device_table_handle_t table_handle = NULL;
    TEST_ESP_OK(i3c_master_scan_devices_by_entdaa(bus_handle, &table_handle));
    size_t cnt = 0;
    i3c_master_get_device_count(table_handle, &cnt);
    for (int i = 0; i < cnt; i++) {
        i3c_master_i3c_device_handle_t dev_handle;
        TEST_ESP_OK(i3c_master_get_device_handle(table_handle, i, &dev_handle));
        i3c_device_information_t info;
        i3c_master_i3c_device_get_info(dev_handle, &info);
        printf("pid: 0x%08llX\n", info.pid);
        printf("bcr: 0x%02X\n", info.bcr);
        printf("dcr: 0x%02X\n", info.dcr);
        printf("dynamic_addr: 0x%02X\n", info.dynamic_addr);
        TEST_ASSERT(info.pid == 0xA0AA0000);
        TEST_ASSERT(info.bcr == 0x06);
        TEST_ASSERT(info.dcr == 0x00);
    }

    unity_send_signal("master write");

    unity_wait_for_signal("ready to delete");

    for (int i = 0; i < cnt; i++) {
        i3c_master_i3c_device_handle_t dev_handle;
        TEST_ESP_OK(i3c_master_get_device_handle(table_handle, i, &dev_handle));
        TEST_ESP_OK(i3c_master_bus_rm_i3c_device(dev_handle));
    }
    TEST_ESP_OK(i3c_master_free_device_handle_table(table_handle));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C master ENTDAA test", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_master_entdaa_test, i3c_slave_for_entdaa);

static void i3c_slave_read_test_via_entdaa(void)
{
    unity_wait_for_signal("i3c master init first");

    uint8_t *temp_data = (uint8_t*)heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT);
    assert(temp_data);

    i3c_slave_init();

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("master write");

    i3c_slave_ll_read_data(&I3C_SLV, temp_data, DATA_LENGTH);

    ESP_LOG_BUFFER_HEX(TAG, temp_data, DATA_LENGTH);

    for (int i = 0; i < DATA_LENGTH; i++) {
        TEST_ASSERT(temp_data[i] == i);
    }

    unity_send_signal("ready to delete");
    free(temp_data);
}

static void i3c_master_write_test_via_entdaa(void)
{
    uint8_t data_wr[DATA_LENGTH] = { 0 };
    int i = 0;

    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");

    i3c_master_i3c_device_table_handle_t table_handle = NULL;
    TEST_ESP_OK(i3c_master_scan_devices_by_entdaa(bus_handle, &table_handle));
    size_t cnt = 0;
    i3c_master_get_device_count(table_handle, &cnt);

    for (i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    ESP_LOG_BUFFER_HEX(TAG, data_wr, i);

    i3c_master_i3c_device_handle_t dev_handle;
    TEST_ESP_OK(i3c_master_get_device_handle(table_handle, 0, &dev_handle));
    TEST_ESP_OK(i3c_master_i3c_device_transmit(dev_handle, data_wr, DATA_LENGTH, -1));

    unity_send_signal("master write");

    unity_wait_for_signal("ready to delete");

    for (int i = 0; i < cnt; i++) {
        i3c_master_i3c_device_handle_t dev_handle;
        TEST_ESP_OK(i3c_master_get_device_handle(table_handle, i, &dev_handle));
        TEST_ESP_OK(i3c_master_bus_rm_i3c_device(dev_handle));
    }
    TEST_ESP_OK(i3c_master_free_device_handle_table(table_handle));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C master write slave test via ENTDAA in fifo mode", "[i3c][test_env=generic_multi_device][timeout=150]", i3c_master_write_test_via_entdaa, i3c_slave_read_test_via_entdaa);

static void i3c_slave_trigger_ibi_test(void)
{
    unity_wait_for_signal("i3c master init first");

    uint8_t *temp_data = (uint8_t*)heap_caps_malloc(DATA_LENGTH, MALLOC_CAP_DEFAULT);
    assert(temp_data);

    i3c_slave_init();

    unity_send_signal("i3c slave init finish");

    unity_wait_for_signal("master write");

    i3c_slave_ll_set_ibi_payload(&I3C_SLV, 0xcc);
    i3c_slave_ll_start_ibi(&I3C_SLV, true);

    unity_send_signal("ready to delete");
    free(temp_data);
}

static QueueHandle_t event_queue;

typedef struct {
    uint8_t ibi_id;
    uint8_t ibi_data_len;
} i3c_master_ibi_event_t;

static uint8_t i3c_ibi_data[32];

IRAM_ATTR static bool ibi_callback(i3c_master_i3c_device_handle_t dev_handle, const i3c_master_ibi_info_t *evt_data, void *user_data)
{
    BaseType_t xTaskWoken;

    i3c_master_ibi_event_t evt = {
        .ibi_id = evt_data->ibi_id,
        .ibi_data_len = evt_data->data_length,
    };
    memcpy(i3c_ibi_data, evt_data->ibi_data, evt_data->data_length);
    xQueueSendFromISR(event_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void i3c_master_get_ibi_test(void)
{
    event_queue = xQueueCreate(1, sizeof(int));
    assert(event_queue);
    i3c_master_bus_config_t i3c_mst_config = {
        .sda_io_num = I3C_MASTER_SDA_IO,
        .scl_io_num = I3C_MASTER_SCL_IO,
        .clock_source = I3C_MASTER_CLK_SRC_DEFAULT,
        .trans_queue_depth = 1,
        .intr_priority = 0,
        .i3c_scl_freq_hz_od = 600 * 1000,
        .i3c_scl_freq_hz_pp = 2 * 1000 * 1000,
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
    TEST_ESP_OK(i3c_new_master_bus(&i3c_mst_config, &bus_handle));

    i3c_device_i3c_config_t dev_cfg = {
        .dynamic_addr = TEST_DYNAMIC_ADDRESS,
        .static_addr = TEST_STATIC_ADDRESS,
    };
    i3c_master_i3c_device_handle_t dev;
    unity_send_signal("i3c master init first");

    unity_wait_for_signal("i3c slave init finish");
    TEST_ESP_OK(i3c_master_bus_add_i3c_static_device(bus_handle, &dev_cfg, &dev));

    i3c_ibi_config_t ibi_cfg = {
        .enable_ibi = true,
        .enable_ibi_payload = true,
    };
    i3c_master_i3c_device_ibi_config(dev, &ibi_cfg);

    i3c_master_i3c_event_callbacks_t cbs = {
        .on_trans_done = NULL,
        .on_ibi = ibi_callback,
    };
    i3c_master_i3c_device_register_event_callbacks(dev, &cbs, NULL);

    unity_send_signal("master write");

    i3c_master_ibi_event_t evt;
    if (xQueueReceive(event_queue, &evt, 100000) == pdTRUE) {
        printf("ibi_id: %d, ibi_data_len: %d\n", evt.ibi_id, evt.ibi_data_len);
        printf("ibi_data: ");
        for (int i = 0; i < evt.ibi_data_len; i++) {
            printf("%02X \n", i3c_ibi_data[i]);
        }
        TEST_ASSERT(evt.ibi_data_len == 1);
        TEST_ASSERT(i3c_ibi_data[0] == 0xcc);
    }

    unity_wait_for_signal("ready to delete");
    vQueueDelete(event_queue);
    TEST_ESP_OK(i3c_master_bus_rm_i3c_device(dev));
    TEST_ESP_OK(i3c_del_master_bus(bus_handle));
}

TEST_CASE_MULTIPLE_DEVICES("I3C master IBI test", "[i3c][test_env=generic_multi_device][ignore][timeout=150]", i3c_master_get_ibi_test, i3c_slave_trigger_ibi_test);
