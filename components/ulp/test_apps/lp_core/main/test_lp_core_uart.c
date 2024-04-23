/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "hal/uart_types.h"
#include "lp_core_test_app_uart.h"
#include "portmacro.h"
#include "ulp_lp_core.h"
#include "lp_core_uart.h"

#include "test_shared.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_log.h"

#include "driver/uart.h"

extern const uint8_t lp_core_main_uart_bin_start[] asm("_binary_lp_core_test_app_uart_bin_start");
extern const uint8_t lp_core_main_uart_bin_end[]   asm("_binary_lp_core_test_app_uart_bin_end");

static const char* TAG = "lp_core_uart_test";

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);

}

TEST_CASE("LP-Core LP-UART initialization test", "[lp_core]")
{
    /* Default UART configuration must be successful */
    ESP_LOGI(TAG, "Verifying default LP UART configuration");
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&cfg));

    /* NULL configuration should result in an error */
    ESP_LOGI(TAG, "Verifying NULL configuration");
    TEST_ASSERT(ESP_OK != lp_core_uart_init(NULL));

    /* RX Flow control must be less than SOC_LP_UART_FIFO_LEN */
    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Rx Flow Control Threshold");
    lp_core_uart_cfg_t cfg1 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg1.uart_proto_cfg.rx_flow_ctrl_thresh = SOC_LP_UART_FIFO_LEN + 1;
    TEST_ASSERT(ESP_OK != lp_core_uart_init(&cfg1));

#if !SOC_LP_GPIO_MATRIX_SUPPORTED
    /* If LP_GPIO Matrix is not supported then the UART pins must be fixed */
    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Tx IO pin");
    lp_core_uart_cfg_t cfg2 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg2.uart_pin_cfg.tx_io_num++;
    TEST_ASSERT(ESP_OK != lp_core_uart_init(&cfg2));

    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Rx IO pin");
    lp_core_uart_cfg_t cfg3 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg3.uart_pin_cfg.rx_io_num--;
    TEST_ASSERT(ESP_OK != lp_core_uart_init(&cfg3));
#else
    /* When LP_GPIO Matrix is supported then any valid LP_IO should be configurable */
    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Tx IO pin");
    lp_core_uart_cfg_t cfg4 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg4.uart_pin_cfg.tx_io_num++;
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&cfg4));

    ESP_LOGI(TAG, "Verifying LP UART configuration with incorrect Rx IO pin");
    lp_core_uart_cfg_t cfg5 = LP_CORE_UART_DEFAULT_CONFIG();
    cfg5.uart_pin_cfg.rx_io_num--;
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&cfg5));
#endif /* !SOC_LP_GPIO_MATRIX_SUPPORTED */
}

/* LP UART default config */
static lp_core_uart_cfg_t lp_uart_cfg = LP_CORE_UART_DEFAULT_CONFIG();

/* LP UART non-default configuration */
static lp_core_uart_cfg_t lp_uart_cfg1 = {
    .uart_proto_cfg.baud_rate = 9600,
    .uart_proto_cfg.data_bits = UART_DATA_8_BITS,
    .uart_proto_cfg.parity = UART_PARITY_ODD,
    .uart_proto_cfg.stop_bits = UART_STOP_BITS_2,
    .uart_proto_cfg.rx_flow_ctrl_thresh = 0,
    .uart_proto_cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    LP_UART_DEFAULT_GPIO_CONFIG()
};

/* Global test data */
const uint8_t start_pattern[4] = {0xDE, 0xAD, 0xBE, 0xEF};
uint8_t expected_rx_data[UART_BUF_SIZE];
#define TEST_DATA_LEN 234 // Select a random number of bytes to transmit

static void setup_test_data(uint8_t *tx_data, uint8_t *rx_data)
{
    if (tx_data) {
        /* Copy the start pattern followed by the test data */
        memcpy(tx_data, start_pattern, sizeof(start_pattern));
        for (int i = sizeof(start_pattern); i < TEST_DATA_LEN + sizeof(start_pattern); i++) {
            tx_data[i] = i + 7 - sizeof(start_pattern); // We use test data which is i + 7
        }
    }

    if (rx_data) {
        for (int i = 0; i < TEST_DATA_LEN; i++) {
            expected_rx_data[i] = i + 7;
        }
    }
}

static void hp_uart_read(void)
{
    /* Configure HP UART driver */
    uart_config_t hp_uart_cfg = {
        .baud_rate = lp_uart_cfg.uart_proto_cfg.baud_rate,
        .data_bits = lp_uart_cfg.uart_proto_cfg.data_bits,
        .parity    = lp_uart_cfg.uart_proto_cfg.parity,
        .stop_bits = lp_uart_cfg.uart_proto_cfg.stop_bits,
        .flow_ctrl = lp_uart_cfg.uart_proto_cfg.flow_ctrl,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    /* Install HP UART driver */
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &hp_uart_cfg));

    /* Cross-connect the HP UART pins and the LP UART pins for the test */
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, lp_uart_cfg.uart_pin_cfg.rx_io_num, lp_uart_cfg.uart_pin_cfg.tx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Setup test data */
    setup_test_data(NULL, expected_rx_data);

    /* Notify the LP UART that the HP UART is initialized */
    unity_send_signal("HP UART init done");

    /* Receive data from LP UART */
    int bytes_remaining = TEST_DATA_LEN + sizeof(start_pattern);
    uint8_t rx_data[UART_BUF_SIZE];
    int recv_idx = 0;
    while (bytes_remaining > 0) {
        int bytes_received = uart_read_bytes(UART_NUM_1, rx_data + recv_idx, UART_BUF_SIZE, 10 / portTICK_PERIOD_MS);
        if (bytes_received < 0) {
            TEST_FAIL_MESSAGE("HP UART read error");
        } else if (bytes_received > 0) {
            recv_idx += bytes_received;
            bytes_remaining -= bytes_received;
        }
    }

    /* Check if we received the start pattern */
    int data_idx = -1;
    for (int i = 0; i < UART_BUF_SIZE; i++) {
        if (!memcmp(rx_data + i, start_pattern, sizeof(start_pattern))) {
            data_idx = i + 4; // Index of byte just after the start_pattern
        }
    }

    /* Test should pass if we received the start_pattern */
    TEST_ASSERT_NOT_EQUAL(-1, data_idx);

    /* Verify test data */
    ESP_LOGI(TAG, "Verify Rx data");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data, rx_data + data_idx, TEST_DATA_LEN);

    /* Uninstall the HP UART driver */
    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

static void test_lp_uart_write(void)
{
    /* Setup LP UART with default configuration */
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&lp_uart_cfg));

    /* Wait for the HP UART device to be initialized */
    unity_wait_for_signal("HP UART init done");

    /* Load and Run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_uart_bin_start, lp_core_main_uart_bin_end);

    /* Setup test data */
    setup_test_data((uint8_t *)&ulp_tx_data, NULL);
    ulp_tx_len = TEST_DATA_LEN + sizeof(start_pattern);

    /* Start the test */
    ESP_LOGI(TAG, "Write test start");
    ulp_test_cmd = LP_CORE_LP_UART_WRITE_TEST;
}

static void hp_uart_read_options(void)
{
    /* Wait for LP UART to be initialized first */
    unity_wait_for_signal("LP UART init done");

    /* Configure HP UART driver */
    uart_config_t hp_uart_cfg = {
        .baud_rate = lp_uart_cfg1.uart_proto_cfg.baud_rate,
        .data_bits = lp_uart_cfg1.uart_proto_cfg.data_bits,
        .parity    = lp_uart_cfg1.uart_proto_cfg.parity,
        .stop_bits = lp_uart_cfg1.uart_proto_cfg.stop_bits,
        .flow_ctrl = lp_uart_cfg1.uart_proto_cfg.flow_ctrl,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    /* Install HP UART driver */
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &hp_uart_cfg));

    /* Cross-connect the HP UART pins and the LP UART pins for the test */
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, lp_uart_cfg1.uart_pin_cfg.rx_io_num, lp_uart_cfg1.uart_pin_cfg.tx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Setup test data */
    setup_test_data(NULL, expected_rx_data);

    /* Notify the LP UART that the HP UART is initialized */
    unity_send_signal("HP UART init done");

    /* Receive data from LP UART */
    int bytes_remaining = TEST_DATA_LEN + sizeof(start_pattern);
    uint8_t rx_data[UART_BUF_SIZE];

    int recv_idx = 0;
    while (bytes_remaining > 0) {
        int bytes_received = uart_read_bytes(UART_NUM_1, rx_data + recv_idx, UART_BUF_SIZE, 10 / portTICK_PERIOD_MS);
        if (bytes_received < 0) {
            TEST_FAIL_MESSAGE("HP UART read error");
        } else if (bytes_received > 0) {
            recv_idx += bytes_received;
            bytes_remaining -= bytes_received;
        }
    }

    /* Check if we received the start pattern */
    int data_idx = -1;
    for (int i = 0; i < UART_BUF_SIZE; i++) {
        if (!memcmp(rx_data + i, start_pattern, sizeof(start_pattern))) {
            data_idx = i + 4; // Index of byte just after the start_pattern
        }
    }

    /* Test should pass if we received the start_pattern */
    TEST_ASSERT_NOT_EQUAL(-1, data_idx);

    /* Verify test data */
    ESP_LOGI(TAG, "Verify Rx data");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data, rx_data + data_idx, TEST_DATA_LEN);

    /* Uninstall the HP UART driver */
    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

static void test_lp_uart_write_options(void)
{
    /* Setup LP UART with updated configuration */
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&lp_uart_cfg1));

    /* Notify HP UART once LP UART is initialized */
    unity_send_signal("LP UART init done");

    /* Wait for the HP UART device to be initialized */
    unity_wait_for_signal("HP UART init done");

    /* Load and Run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_uart_bin_start, lp_core_main_uart_bin_end);

    /* Setup test data */
    setup_test_data((uint8_t *)&ulp_tx_data, NULL);
    ulp_tx_len = TEST_DATA_LEN + sizeof(start_pattern);

    /* Start the test */
    ESP_LOGI(TAG, "Write test start");
    ulp_test_cmd = LP_CORE_LP_UART_WRITE_TEST;
}

static void hp_uart_write(void)
{
    /* Configure HP UART driver */
    uart_config_t hp_uart_cfg = {
        .baud_rate = lp_uart_cfg.uart_proto_cfg.baud_rate,
        .data_bits = lp_uart_cfg.uart_proto_cfg.data_bits,
        .parity    = lp_uart_cfg.uart_proto_cfg.parity,
        .stop_bits = lp_uart_cfg.uart_proto_cfg.stop_bits,
        .flow_ctrl = lp_uart_cfg.uart_proto_cfg.flow_ctrl,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    /* Install HP UART driver */
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &hp_uart_cfg));

    /* Cross-connect the HP UART pins and the LP UART pins for the test */
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, lp_uart_cfg.uart_pin_cfg.rx_io_num, lp_uart_cfg.uart_pin_cfg.tx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Setup test data */
    uint8_t tx_data[UART_BUF_SIZE];
    setup_test_data(tx_data, NULL);

    /* Notify the LP UART that the HP UART is initialized */
    unity_send_signal("HP UART init done");

    /* Wait for the LP UART to be in receiving state */
    unity_wait_for_signal("LP UART recv ready");

    /* Write data to LP UART */
    uart_write_bytes(UART_NUM_1, (const char *)tx_data, TEST_DATA_LEN + sizeof(start_pattern));

    /* Uninstall the HP UART driver */
    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

static void test_lp_uart_read(void)
{
    /* Setup LP UART with updated configuration */
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&lp_uart_cfg));

    /* Wait for the HP UART device to be initialized */
    unity_wait_for_signal("HP UART init done");

    /* Load and Run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_uart_bin_start, lp_core_main_uart_bin_end);

    /* Setup test data */
    setup_test_data(NULL, expected_rx_data);
    ulp_tx_len = TEST_DATA_LEN + sizeof(start_pattern);

    /* Start the test */
    ESP_LOGI(TAG, "Read test start");
    ulp_test_cmd = LP_CORE_LP_UART_READ_TEST;
    vTaskDelay(10);

    /* Notify the HP UART to write data */
    unity_send_signal("LP UART recv ready");

    /* Wait for test completion */
    while (ulp_test_cmd_reply != LP_CORE_COMMAND_OK) {
        vTaskDelay(10);
    }

    /* Check if we received the start pattern */
    uint8_t *rx_data = (uint8_t*)&ulp_rx_data;
    int data_idx = -1;
    for (int i = 0; i < UART_BUF_SIZE; i++) {
        if (!memcmp(rx_data + i, start_pattern, sizeof(start_pattern))) {
            data_idx = i + 4; // Index of byte just after the start_pattern
        }
    }

    /* Verify test data */
    ESP_LOGI(TAG, "Verify Rx data");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data, rx_data + data_idx, TEST_DATA_LEN);
}

static void hp_uart_write_options(void)
{
    /* Configure HP UART driver */
    uart_config_t hp_uart_cfg = {
        .baud_rate = lp_uart_cfg1.uart_proto_cfg.baud_rate,
        .data_bits = lp_uart_cfg1.uart_proto_cfg.data_bits,
        .parity    = lp_uart_cfg1.uart_proto_cfg.parity,
        .stop_bits = lp_uart_cfg1.uart_proto_cfg.stop_bits,
        .flow_ctrl = lp_uart_cfg1.uart_proto_cfg.flow_ctrl,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    /* Install HP UART driver */
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &hp_uart_cfg));

    /* Cross-connect the HP UART pins and the LP UART pins for the test */
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, lp_uart_cfg1.uart_pin_cfg.rx_io_num, lp_uart_cfg1.uart_pin_cfg.tx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Setup test data */
    uint8_t tx_data[UART_BUF_SIZE];
    setup_test_data(tx_data, NULL);

    /* Notify the LP UART that the HP UART is initialized */
    unity_send_signal("HP UART init done");

    /* Wait for the LP UART to be in receiving state */
    unity_wait_for_signal("LP UART recv ready");

    /* Write data to LP UART */
    uart_write_bytes(UART_NUM_1, (const char *)tx_data, TEST_DATA_LEN + sizeof(start_pattern));

    /* Uninstall the HP UART driver */
    uart_driver_delete(UART_NUM_1);
    vTaskDelay(1);
}

static void test_lp_uart_read_options(void)
{
    /* Setup LP UART with updated configuration */
    TEST_ASSERT(ESP_OK == lp_core_uart_init(&lp_uart_cfg1));

    /* Wait for the HP UART device to be initialized */
    unity_wait_for_signal("HP UART init done");

    /* Load and Run the LP core firmware */
    ulp_lp_core_cfg_t lp_cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };
    load_and_start_lp_core_firmware(&lp_cfg, lp_core_main_uart_bin_start, lp_core_main_uart_bin_end);

    /* Setup test data */
    setup_test_data(NULL, expected_rx_data);
    ulp_tx_len = TEST_DATA_LEN + sizeof(start_pattern);

    /* Start the test */
    ESP_LOGI(TAG, "Read test start");
    ulp_test_cmd = LP_CORE_LP_UART_READ_TEST;
    vTaskDelay(10);

    /* Notify the HP UART to write data */
    unity_send_signal("LP UART recv ready");

    /* Wait for test completion */
    while (ulp_test_cmd_reply != LP_CORE_COMMAND_OK) {
        vTaskDelay(10);
    }

    /* Check if we received the start pattern */
    uint8_t *rx_data = (uint8_t*)&ulp_rx_data;
    int data_idx = -1;
    for (int i = 0; i < UART_BUF_SIZE; i++) {
        if (!memcmp(rx_data + i, start_pattern, sizeof(start_pattern))) {
            data_idx = i + 4; // Index of byte just after the start_pattern
        }
    }

    /* Verify test data */
    ESP_LOGI(TAG, "Verify Rx data");
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data, rx_data + data_idx, TEST_DATA_LEN);
}

/* Test LP UART write operation with default LP UART initialization configuration */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - default config", "[lp_core][test_env=generic_multi_device][timeout=150]", test_lp_uart_write, hp_uart_read);
/* Test LP UART write operation with updated LP UART initialization configuration */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART write test - optional config", "[lp_core][test_env=generic_multi_device][timeout=150]", test_lp_uart_write_options, hp_uart_read_options);
/* Test LP UART read operation with default LP UART initialization configuration */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - default config", "[lp_core][test_env=generic_multi_device][timeout=150]", test_lp_uart_read, hp_uart_write);
/* Test LP UART read operation with updated LP UART initialization configuration */
TEST_CASE_MULTIPLE_DEVICES("LP-Core LP-UART read test - optional config", "[lp_core][test_env=generic_multi_device][timeout=150]", test_lp_uart_read_options, hp_uart_write_options);
