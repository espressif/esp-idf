/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <unistd.h>
#include "unity.h"
#include "test_utils.h"
#include "driver/uart.h"
#include "driver/uart_wakeup.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "soc/uart_pins.h"
#include "soc/soc_caps.h"
#include "test_common.h"

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32P4
#define DEFAULT_UART1_TX_IO_NUM     U1TXD_GPIO_NUM
#define DEFAULT_UART1_RX_IO_NUM     U1RXD_GPIO_NUM
#elif CONFIG_IDF_TARGET_ESP32
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_25
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_26
#elif CONFIG_IDF_TARGET_ESP32C2
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_1
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_0
#elif CONFIG_IDF_TARGET_ESP32C6
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_1
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_0
#elif CONFIG_IDF_TARGET_ESP32H2
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_4
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_5
#elif CONFIG_IDF_TARGET_ESP32C3
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_4
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_5
#elif CONFIG_IDF_TARGET_ESP32C61
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_4
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_5
#elif CONFIG_IDF_TARGET_ESP32C5
#define DEFAULT_UART1_TX_IO_NUM     GPIO_NUM_2
#define DEFAULT_UART1_RX_IO_NUM     GPIO_NUM_3
#endif

#define MASTER_UART_NUM         (1)
#define MASTER_UART_TX_IO_NUM   DEFAULT_UART1_TX_IO_NUM
#define MASTER_UART_RX_IO_NUM   DEFAULT_UART1_RX_IO_NUM
#define SLAVE_UART_NUM          (1)
#define SLAVE_UART_TX_IO_NUM    DEFAULT_UART1_RX_IO_NUM
#define SLAVE_UART_RX_IO_NUM    DEFAULT_UART1_TX_IO_NUM
#define UART_BAUD_RATE          (115200)
#define BUF_SIZE                (1024)
#define TIMER_WAKEUP_TIME_US    (1 * 100 * 1000)

static void force_stdout(void)
{
    fflush(stdout);
    fsync(fileno(stdout));
}

/* Initialize UART */
static esp_err_t uart_initialization(uart_port_param_t *port_param)
{
    uart_port_t uart_num = port_param->port_num;
    uart_config_t uart_config = {
        .baud_rate  = UART_BAUD_RATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = port_param->default_src_clk,
        .rx_flow_ctrl_thresh = port_param->rx_flow_ctrl_thresh,
    };
    const int uart_tx = port_param->tx_pin_num;
    const int uart_rx = port_param->rx_pin_num;
    TEST_ESP_OK(uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 20, NULL, 0));
    TEST_ESP_OK(uart_param_config(uart_num, &uart_config));
    TEST_ESP_OK(uart_set_pin(uart_num, uart_tx, uart_rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    return ESP_OK;
}

/* Configure UART wakeup */
static esp_err_t uart_wakeup_config(uart_port_param_t *port_param, uart_wakeup_cfg_t *uart_wakeup_cfg)
{
    /* This function configures the wakeup behavior for a specified UART port based on the provided configuration.
     * The behavior depends on the selected wakeup mode and additional parameters such as active threshold or
     * character sequence, if applicable. It is important that the provided configuration matches the capabilities
     * of the SOC to ensure proper operation. Besides, the Rx pin need extra configuration to enable it can work during light sleep */

    uart_port_t uart_num = port_param->port_num;
    // Initializes the UART wakeup functionality.
    TEST_ESP_OK(uart_wakeup_setup(uart_num, uart_wakeup_cfg));
    TEST_ESP_OK(esp_sleep_enable_uart_wakeup(uart_num));

    return ESP_OK;
}

void init_master(void)
{
    uart_port_param_t port_param = {
        .port_num = MASTER_UART_NUM,
#if SOC_UART_SUPPORT_REF_TICK
        .default_src_clk = UART_SCLK_REF_TICK,
#else
        .default_src_clk = UART_SCLK_XTAL,
#endif
        .tx_pin_num = MASTER_UART_TX_IO_NUM,
        .rx_pin_num = MASTER_UART_RX_IO_NUM,
        .rx_flow_ctrl_thresh = 120
    };
    TEST_ESP_OK(uart_initialization(&port_param));
    unity_send_signal("Master Ready");
}

void deinit_master(void)
{
    TEST_ESP_OK(uart_driver_delete(MASTER_UART_NUM));
}

void send_and_verify_recived_data(const char* message, uint8_t length, bool should_wake_up)
{
    unity_wait_for_signal("Slave Ready");
    force_stdout();

    uart_flush_input(MASTER_UART_NUM);
    uart_write_bytes(MASTER_UART_NUM, message, length);
    /* Wait for uart write finish */
    uart_wait_tx_idle_polling(MASTER_UART_NUM);

    bool wake_up_detected = false;
    const char *target = "Wakeup OK!";
    int target_len = 11;
    bool match = true;
    char *data = (char *) malloc(BUF_SIZE);
    int len = uart_read_bytes(MASTER_UART_NUM, data, target_len, 5000 / portTICK_PERIOD_MS);

    if (len > 0) {
        if (len != target_len) {
            match = false;
        } else {
            for (int i = 0; i < target_len; i++) {
                if (data[i] != target[i]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                wake_up_detected = true;
            }
        }
    }

    data[len] = '\0'; // Null-terminate the received data

    free(data);

    TEST_ESP_OK(should_wake_up != wake_up_detected);

}

static void init_slave(uart_wakeup_cfg_t *wake_up_cfg)
{
    uart_port_param_t port_param = {
        .port_num = SLAVE_UART_NUM,
#if SOC_UART_SUPPORT_REF_TICK
        .default_src_clk = UART_SCLK_REF_TICK,
#else
        .default_src_clk = UART_SCLK_XTAL,
#endif
        .tx_pin_num = SLAVE_UART_TX_IO_NUM,
        .rx_pin_num = SLAVE_UART_RX_IO_NUM,
        .rx_flow_ctrl_thresh = 120
    };
    unity_wait_for_signal("Master Ready");
    TEST_ESP_OK(uart_initialization(&port_param));
    TEST_ESP_OK(uart_wakeup_config(&port_param, wake_up_cfg));
    unity_send_signal("Slave Ready");
    force_stdout();
}

static void deinit_slave(void)
{
    TEST_ESP_OK(uart_driver_delete(SLAVE_UART_NUM));
}

static void enter_sleep_and_send_respond(void)
{
    /* Get timestamp before entering sleep */
    int64_t t_before_us = esp_timer_get_time();

    /* Enter sleep mode */
    esp_light_sleep_start();

    /* Get timestamp after waking up from sleep */
    int64_t t_after_us = esp_timer_get_time();

    /* Clear uart input buffer */
    uart_flush_input(SLAVE_UART_NUM);
    printf("sleep duration: %lld\n", t_after_us - t_before_us);

    /* Determine the reason for uart wakeup */
    switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_UART:
        /* Hang-up for a while to switch and execute the uart task
            * Otherwise the chip may fall sleep again before running uart task */
        vTaskDelay(1);
        uart_write_bytes(SLAVE_UART_NUM, "Wakeup OK!", 11);
        break;
    default:
        uart_write_bytes(SLAVE_UART_NUM, "Wakeup failed!", 15);
        break;
    }

    /* Wait for uart write finish */
    uart_wait_tx_idle_polling(SLAVE_UART_NUM);
}

// slave
#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
static void test_uart_wakeup_mode_0(void)
{
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US));
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_ACTIVE_THRESH,
        .rx_edge_threshold = 8,
    };
    init_slave(&wake_up_cfg);

    enter_sleep_and_send_respond();

    deinit_slave();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
static void test_uart_wakeup_mode_1(void)
{
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US));
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_FIFO_THRESH,
        .rx_fifo_threshold = 8
    };
    init_slave(&wake_up_cfg);

    enter_sleep_and_send_respond();

    deinit_slave();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE
static void test_uart_wakeup_mode_2(void)
{
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US));
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_START_BIT,
    };
    init_slave(&wake_up_cfg);

    enter_sleep_and_send_respond();

    deinit_slave();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
static void test_uart_wakeup_mode_3(void)
{
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US));
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_CHAR_SEQ,
        .wake_chars_seq = "hello"
    };
    init_slave(&wake_up_cfg);

    enter_sleep_and_send_respond();

    deinit_slave();
}
#endif

// master
#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
static void send_uart_wakeup_mode_0_edge_8(void)
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
    init_master();

    send_and_verify_recived_data("TT", 2, true);

    deinit_master();
}

static void send_uart_wakeup_mode_0_edge_7(void)
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
    init_master();

    send_and_verify_recived_data("Ta", 2, false);

    deinit_master();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
static void send_uart_wakeup_mode_1_9_bytes(void)
{
    init_master();

    send_and_verify_recived_data("123456789", 9, true);

    deinit_master();
}

static void send_uart_wakeup_mode_1_8_bytes(void)
{
    init_master();

    send_and_verify_recived_data("12345678", 8, false);

    deinit_master();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE
static void send_uart_wakeup_mode_2_start_bit(void)
{
    init_master();

    send_and_verify_recived_data("@", 1, true);

    deinit_master();
}

static void send_uart_wakeup_mode_2_no_start_bit(void)
{
    init_master();

    send_and_verify_recived_data("", 0, false);

    deinit_master();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
static void send_uart_wakeup_mode_3_positive_sequence(void)
{
    init_master();

    send_and_verify_recived_data("hello", 5, true);

    deinit_master();
}

static void send_uart_wakeup_mode_3_negative_sequence(void)
{
    init_master();

    send_and_verify_recived_data("hwllo", 5, false);

    deinit_master();
}
#endif

#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
TEST_CASE_MULTIPLE_DEVICES("Mode 0 - Active threshold - 8 - success", "[uart][wakeup][Mode_0][timeout=5]",
                           test_uart_wakeup_mode_0, send_uart_wakeup_mode_0_edge_8);

TEST_CASE_MULTIPLE_DEVICES("Mode 0 - Active threshold - 8 - fail", "[uart][wakeup][Mode_0][timeout=5]",
                           test_uart_wakeup_mode_0, send_uart_wakeup_mode_0_edge_7);
#endif

#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
TEST_CASE_MULTIPLE_DEVICES("Mode 1 - RX FIFO - 9 bytes - success", "[uart][wakeup][Mode_1][timeout=5]",
                           test_uart_wakeup_mode_1, send_uart_wakeup_mode_1_9_bytes);

TEST_CASE_MULTIPLE_DEVICES("Mode 1 - RX FIFO - 8 bytes- fail", "[uart][wakeup][Mode_1][timeout=5]",
                           test_uart_wakeup_mode_1, send_uart_wakeup_mode_1_8_bytes);
#endif

#if SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE
TEST_CASE_MULTIPLE_DEVICES("Mode 2 - Start bit sequence - valid data - success", "[uart][wakeup][Mode_2][timeout=5]",
                           test_uart_wakeup_mode_2, send_uart_wakeup_mode_2_start_bit);

TEST_CASE_MULTIPLE_DEVICES("Mode 2 - Start bit sequence - no data- fail", "[uart][wakeup][Mode_2][timeout=5]",
                           test_uart_wakeup_mode_2, send_uart_wakeup_mode_2_no_start_bit);
#endif

#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
TEST_CASE_MULTIPLE_DEVICES("Mode 3 - Wakeup sequence - positive hello - success", "[uart][wakeup][Mode_3][timeout=5]",
                           test_uart_wakeup_mode_3, send_uart_wakeup_mode_3_positive_sequence);

TEST_CASE_MULTIPLE_DEVICES("Mode 3 - Wakeup sequence - negative hello- fail", "[uart][wakeup][Mode_3][timeout=5]",
                           test_uart_wakeup_mode_3, send_uart_wakeup_mode_3_negative_sequence);
#endif
