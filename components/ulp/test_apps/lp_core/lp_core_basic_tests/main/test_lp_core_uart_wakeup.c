/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"
#include "ulp_lp_core_lp_uart_shared.h"
#include "lp_core_uart.h"
#include "ulp_lp_core.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_pins.h"
#include "test_utils.h"

#define ECHO_UART_PORT_NUM      1
#define ECHO_UART_BAUD_RATE     115200
#define BUF_SIZE (1024)

#define ECHO_TEST_TXD LP_U0RXD_GPIO_NUM
#define ECHO_TEST_RXD LP_U0TXD_GPIO_NUM
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

void init_host(void)
{
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    TEST_ESP_OK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    TEST_ESP_OK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    TEST_ESP_OK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    unity_send_signal("Host Ready");
}

void deinit_host(void)
{
    uart_driver_delete(ECHO_UART_PORT_NUM);
}

void send_and_verify_recived_data(const char* message, uint8_t length, bool should_wake_up)
{
    unity_wait_for_signal("Slave Ready");
    uart_flush_input(ECHO_UART_PORT_NUM);
    uart_write_bytes(ECHO_UART_PORT_NUM, message, length);

    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 1000 / portTICK_PERIOD_MS);

    bool wake_up_detected = false;
    const char *target = "Hello world";
    int target_len = 11;

    if (len > 0) {
        if (len >= target_len) {
            for (int i = 0; i <= len - target_len; i++) {
                bool match = true;
                for (int j = 0; j < target_len; j++) {
                    if (data[i + j] != target[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) {
                    wake_up_detected = true;
                    break;
                }
            }
        }

        data[len] = '\0'; // Null-terminate the received data
    }

    free(data);

    TEST_ESP_OK(should_wake_up != wake_up_detected);

}

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_test_app_uart_wakeup_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_test_app_uart_wakeup_bin_end");

static void lp_uart_init(void)
{
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();
    TEST_ESP_OK(lp_core_uart_init(&cfg));
    printf("LP UART initialized successfully\n");
}

static void lp_core_init(void)
{
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_UART,
    };
    TEST_ESP_OK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));
    TEST_ESP_OK(ulp_lp_core_run(&cfg));
    printf("LP core loaded with firmware and running successfully\n");
}

static void init_slave(uart_wakeup_cfg_t *wake_up_cfg)
{
    unity_wait_for_signal("Host Ready");
    lp_uart_init();
    lp_core_init();
    lp_core_uart_clear_buf();

    TEST_ESP_OK(lp_core_uart_wakeup_setup(wake_up_cfg));
    unity_send_signal("Slave Ready");
}

static void test_uart_wakeup_hello_sequence(void)
{
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_CHAR_SEQ,
        .wake_chars_seq = "he**o"
    };
    init_slave(&wake_up_cfg);
}

static void test_uart_wakeup_qwerty_sequence(void)
{
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_CHAR_SEQ,
        .wake_chars_seq = "qwert"
    };
    init_slave(&wake_up_cfg);
}

static void test_uart_wakeup_start_bit(void)
{
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_START_BIT,
    };
    init_slave(&wake_up_cfg);
}

static void test_uart_wakeup_rx_fifo_8_bytes(void)
{
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_FIFO_THRESH,
        .rx_fifo_threshold = 8
    };
    init_slave(&wake_up_cfg);
}

static void test_uart_wakeup_rx_fifo_10_bytes(void)
{
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_FIFO_THRESH,
        .rx_fifo_threshold = 10
    };
    init_slave(&wake_up_cfg);
}

static void test_uart_wakeup_active_threshold_8(void)
{
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_ACTIVE_THRESH,
        .rx_edge_threshold = 8
    };
    init_slave(&wake_up_cfg);
}

// Host

static void send_uart_wakeup_positive_hello_sequence(void)
{
    init_host();
    send_and_verify_recived_data("hello", 5, true);
    deinit_host();
}
static void send_uart_wakeup_negative_hello_sequence(void)
{
    init_host();
    send_and_verify_recived_data("hwllo", 5, false);
    deinit_host();
}

static void send_uart_wakeup_positive_qwerty_sequence(void)
{
    init_host();
    send_and_verify_recived_data("qwert", 5, true);
    deinit_host();
}

static void send_uart_wakeup_negative_qwerty_sequence(void)
{
    init_host();
    send_and_verify_recived_data("Qwert", 5, false);
    deinit_host();
}

static void send_uart_wakeup_start_bit(void)
{
    init_host();
    send_and_verify_recived_data("@", 1, true);
    deinit_host();
}

static void send_uart_wakeup_no_data(void)
{
    init_host();
    send_and_verify_recived_data("", 0, false);
    deinit_host();
}

static void send_uart_wakeup_rx_fifo_9_bytes(void)
{
    init_host();
    send_and_verify_recived_data("123456789", 9, true);
    deinit_host();
}

static void send_uart_wakeup_rx_fifo_8_bytes(void)
{
    init_host();
    send_and_verify_recived_data("12345678", 8, false);
    deinit_host();
}

static void send_uart_wakeup_rx_fifo_11_bytes(void)
{
    init_host();
    send_and_verify_recived_data("12345678901", 11, true);
    deinit_host();
}

static void send_uart_wakeup_rx_fifo_10_bytes(void)
{
    init_host();
    send_and_verify_recived_data("1234567890", 10, false);
    deinit_host();
}

static void send_uart_wakeup_active_threshold_8(void)
{

    /*
    * This is only true in 8N1 mode (8 data bits, no parity, 1 stop bit).
    * Sending "TT" corresponds to:
    * +-------+--------------+------+--------+
    * | Start |  Data Bits   | Stop | Raises |
    * +-------+--------------+------+--------+
    * |   0   |  00101010(T) |   1  |   4    |
    * |   0   |  00101010(T) |   1  |   4    |
    * +-------+--------------+------+--------+
    */
    init_host();
    send_and_verify_recived_data("TT", 2, true);
    deinit_host();
}

static void send_uart_wakeup_active_threshold_7(void)
{
    /*
    * This is only true in 8N1 mode (8 data bits, no parity, 1 stop bit).
    * Sending "Ta" corresponds to:
    * +-------+--------------+------+--------+
    * | Start |  Data Bits   | Stop | Raises |
    * +-------+--------------+------+--------+
    * |   0   |  00101010(T) |   1  |   4    |
    * |   0   |  10000110(a) |   1  |   3    |
    * +-------+--------------+------+--------+
    */
    init_host();
    send_and_verify_recived_data("Ta", 2, false);
    deinit_host();
}

TEST_CASE_MULTIPLE_DEVICES("Hello sequence - positive hello", "[uart][wakeup][seq][timeout=5]",
                           test_uart_wakeup_hello_sequence, send_uart_wakeup_positive_hello_sequence);

TEST_CASE_MULTIPLE_DEVICES("Hello sequence - negative hello", "[uart][wakeup][seq][timeout=5]",
                           test_uart_wakeup_hello_sequence, send_uart_wakeup_negative_hello_sequence);

TEST_CASE_MULTIPLE_DEVICES("QWERTY sequence - positive", "[uart][wakeup][seq][timeout=5]",
                           test_uart_wakeup_qwerty_sequence, send_uart_wakeup_positive_qwerty_sequence);

TEST_CASE_MULTIPLE_DEVICES("QWERTY sequence - negative", "[uart][wakeup][seq][timeout=5]",
                           test_uart_wakeup_qwerty_sequence, send_uart_wakeup_negative_qwerty_sequence);

TEST_CASE_MULTIPLE_DEVICES("Start bit sequence - valid data", "[uart][wakeup][start_bit][timeout=5]",
                           test_uart_wakeup_start_bit, send_uart_wakeup_start_bit);

TEST_CASE_MULTIPLE_DEVICES("Start bit sequence - no data", "[uart][wakeup][start_bit][timeout=5]",
                           test_uart_wakeup_start_bit, send_uart_wakeup_no_data);

TEST_CASE_MULTIPLE_DEVICES("RX FIFO - 8 bytes", "[uart][wakeup][rx_fifo][timeout=5]",
                           test_uart_wakeup_rx_fifo_8_bytes, send_uart_wakeup_rx_fifo_8_bytes);

TEST_CASE_MULTIPLE_DEVICES("RX FIFO - 9 bytes", "[uart][wakeup][rx_fifo][timeout=5]",
                           test_uart_wakeup_rx_fifo_8_bytes, send_uart_wakeup_rx_fifo_9_bytes);

TEST_CASE_MULTIPLE_DEVICES("RX FIFO - 10 bytes", "[uart][wakeup][rx_fifo][timeout=5]",
                           test_uart_wakeup_rx_fifo_10_bytes, send_uart_wakeup_rx_fifo_10_bytes);

TEST_CASE_MULTIPLE_DEVICES("RX FIFO - 11 bytes", "[uart][wakeup][rx_fifo][timeout=5]",
                           test_uart_wakeup_rx_fifo_10_bytes, send_uart_wakeup_rx_fifo_11_bytes);

TEST_CASE_MULTIPLE_DEVICES("Active threshold - 8", "[uart][wakeup][edge][timeout=5]",
                           test_uart_wakeup_active_threshold_8, send_uart_wakeup_active_threshold_8);

TEST_CASE_MULTIPLE_DEVICES("Active threshold - 7", "[uart][wakeup][edge][timeout=5]",
                           test_uart_wakeup_active_threshold_8, send_uart_wakeup_active_threshold_7);
