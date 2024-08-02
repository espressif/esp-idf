/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "lp_core_test_app_spi_master.h"
#include "lp_core_test_app_spi_slave.h"
#include "ulp_lp_core.h"
#include "lp_core_spi.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"
#include "test_shared.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t lp_core_main_spi_master_bin_start[] asm("_binary_lp_core_test_app_spi_master_bin_start");
extern const uint8_t lp_core_main_spi_master_bin_end[]   asm("_binary_lp_core_test_app_spi_master_bin_end");
extern const uint8_t lp_core_main_spi_slave_bin_start[] asm("_binary_lp_core_test_app_spi_slave_bin_start");
extern const uint8_t lp_core_main_spi_slave_bin_end[]   asm("_binary_lp_core_test_app_spi_slave_bin_end");

static const char* TAG = "lp_core_spi_test";

#define TEST_GPIO_PIN_MISO   6
#define TEST_GPIO_PIN_MOSI   7
#define TEST_GPIO_PIN_CLK    8
#define TEST_GPIO_PIN_CS     4

#define TEST_DATA_LEN_BYTES 42
uint8_t expected_data[100] = {0};

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start, (firmware_end - firmware_start)) == ESP_OK);
    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
}

static void setup_test_data(void)
{
    uint8_t *tx_data = (uint8_t *)&ulp_spi_master_tx_buf;
    ulp_spi_tx_len = TEST_DATA_LEN_BYTES;

    /* Setup test data */
    for (int i = 0; i < ulp_spi_tx_len; i++) {
        tx_data[i] = (i + 1) % 256;
        expected_data[i] = tx_data[i];
    }
}

static void setup_expected_data(void)
{
    ulp_spi_rx_len = TEST_DATA_LEN_BYTES;

    /* Setup expected data */
    for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
        expected_data[i] = (i + 1) % 256;
    }
}

/* Base LP SPI bus settings */
lp_spi_host_t host_id = 0;
lp_spi_bus_config_t bus_config = {
    .miso_io_num = TEST_GPIO_PIN_MISO,
    .mosi_io_num = TEST_GPIO_PIN_MOSI,
    .sclk_io_num = TEST_GPIO_PIN_CLK,
};

/* Base LP SPI device settings */
lp_spi_device_config_t device = {
    .cs_io_num = TEST_GPIO_PIN_CS,
    .spi_mode = 0,
    .clock_speed_hz = 10 * 1000, // 10 MHz
    .duty_cycle = 128,  // 50% duty cycle
};

/* Base LP SPI slave device settings */
lp_spi_slave_config_t slv_device = {
    .cs_io_num = TEST_GPIO_PIN_CS,
    .spi_mode = 0,
};

static void lp_spi_master_init(int spi_flags, bool setup_master_loop_back)
{
    /* Initialize LP SPI bus */
    /* Setup loop back for tests which do not use an LP SPI slave for looping back the data. */
    bus_config.miso_io_num = setup_master_loop_back ? TEST_GPIO_PIN_MOSI : TEST_GPIO_PIN_MISO;
    TEST_ASSERT(lp_core_lp_spi_bus_initialize(host_id, &bus_config) == ESP_OK);

    /* Add LP SPI device */
    device.flags = spi_flags;
    TEST_ASSERT(lp_core_lp_spi_bus_add_device(host_id, &device) == ESP_OK);
}

static void lp_spi_slave_init(int spi_flags)
{
    /* Initialize LP SPI bus */
    TEST_ASSERT(lp_core_lp_spi_bus_initialize(host_id, &bus_config) == ESP_OK);

    /* Add LP SPI slave device */
    if (spi_flags != 0) {
        slv_device.flags = spi_flags;
    }
    TEST_ASSERT(lp_core_lp_spi_slave_initialize(host_id, &slv_device) == ESP_OK);
}

static void lp_spi_master_execute_test(bool wait_for_slave_ready)
{
    /* Load and run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_spi_master_bin_start, lp_core_main_spi_master_bin_end);

    if (wait_for_slave_ready) {
        /* Wait for the HP SPI device to be initialized */
        unity_wait_for_signal("LP SPI slave ready");
    }

    /* Setup test data */
    setup_test_data();

    /* Start the test */
    ulp_spi_test_cmd = LP_CORE_LP_SPI_WRITE_READ_TEST;

    while (ulp_spi_test_cmd != LP_CORE_NO_COMMAND) {
        /* Wait for the test to complete */
        vTaskDelay(1);
    }

    /* Verify the received data if we expect the data to be looped back from the LP SPI slave */
    uint8_t *rx_data = (uint8_t *)&ulp_spi_master_rx_buf;
    for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
        ESP_LOGI(TAG, "LP SPI master received data: 0x%02x", rx_data[i]);
    }

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_data, ulp_spi_tx_len);
}

static void lp_spi_slave_execute_test(void)
{
    /* Load and run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_spi_slave_bin_start, lp_core_main_spi_slave_bin_end);

    /* Setup expected test data */
    setup_expected_data();

    /* Send signal to LP SPI master */
    unity_send_signal("LP SPI slave ready");

    /* Wait for the test to complete */
    while (ulp_spi_test_cmd_reply != LP_CORE_COMMAND_OK) {
        vTaskDelay(1);
    }

    /* Verify the received data */
    uint8_t *rx_data = (uint8_t *)&ulp_spi_slave_rx_buf;
    for (int i = 0; i < TEST_DATA_LEN_BYTES; i++) {
        ESP_LOGI(TAG, "LP SPI slave received data: 0x%02x", rx_data[i]);
    }

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_data, TEST_DATA_LEN_BYTES);
}

void test_lp_spi_master(void)
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(0, false);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(true);
}

void test_lp_spi_slave(void)
{
    /* Initialize LP SPI in slave mode */
    lp_spi_slave_init(0);

    /* Start the LP SPI slave test */
    lp_spi_slave_execute_test();
}
void test_lp_spi_master_3wire(void)
{
    /* Initialize LP SPI in master mode */
    int spi_flags = LP_SPI_DEVICE_3WIRE;
    lp_spi_master_init(spi_flags, false);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(true);
}

void test_lp_spi_slave_3wire(void)
{
    /* Initialize LP SPI in slave mode */
    int spi_flags = LP_SPI_DEVICE_3WIRE;
    lp_spi_slave_init(spi_flags);

    /* Start the LP SPI slave test */
    lp_spi_slave_execute_test();
}

void test_lp_spi_master_lsbfirst(void)
{
    /* Initialize LP SPI in master mode */
    int spi_flags = LP_SPI_DEVICE_BIT_LSBFIRST;
    lp_spi_master_init(spi_flags, false);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(true);
}

void test_lp_spi_slave_lsbfirst(void)
{
    /* Initialize LP SPI in slave mode */
    int spi_flags = LP_SPI_DEVICE_BIT_LSBFIRST;
    lp_spi_slave_init(spi_flags);

    /* Start the LP SPI slave test */
    lp_spi_slave_execute_test();
}

/* Test LP-SPI master loopback */
TEST_CASE("LP-Core LP-SPI master loopback test", "[lp_core]")
{
    /* Initialize LP SPI in master mode */
    lp_spi_master_init(0, true);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(false);
}

/* Test LP-SPI master loopback with active low CS line */
TEST_CASE("LP-Core LP-SPI master loopback test with active high CS line", "[lp_core]")
{
    /* Initialize LP SPI in master mode */
    int spi_flags = LP_SPI_DEVICE_CS_ACTIVE_HIGH;
    lp_spi_master_init(spi_flags, true);

    /* Start the LP SPI master test */
    lp_spi_master_execute_test(false);
}

/* Test LP-SPI master and LP-SPI slave communication */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-SPI master and LP-SPI slave read write test", "[lp_core_spi][test_env=generic_multi_device][timeout=150]", test_lp_spi_master, test_lp_spi_slave);

/* Test LP-SPI master in 3-Wire SPI mode */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-SPI master and LP-SPI slave in 3-Wire SPI mode", "[lp_core_spi][test_env=generic_multi_device][timeout=150]", test_lp_spi_master_3wire, test_lp_spi_slave_3wire);

/* Test LP-SPI master and LP-SPI slave in LSB first mode */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-SPI master and LP-SPI in LSB first SPI mode", "[lp_core_spi][test_env=generic_multi_device][timeout=150]", test_lp_spi_master_lsbfirst, test_lp_spi_slave_lsbfirst);
