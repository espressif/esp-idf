/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "unity.h"
#include "test_utils.h"
#include "driver/uart.h"
#include "lp_core_uart.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_uart_shared.h"
#include "hal/lp_core_ll.h"
#include "lp_sleep_common.h"
#include "lp_sleep_payload.h"
#include "lp_core_test_app_uart_sleep.h"
#include "lp_core_test_app_uart_wkup_sleep.h"

extern const uint8_t lp_core_test_app_uart_sleep_bin_start[] asm("_binary_lp_core_test_app_uart_sleep_bin_start");
extern const uint8_t lp_core_test_app_uart_sleep_bin_end[]   asm("_binary_lp_core_test_app_uart_sleep_bin_end");
extern const uint8_t lp_core_test_app_uart_wkup_sleep_bin_start[] asm("_binary_lp_core_test_app_uart_wkup_sleep_bin_start");
extern const uint8_t lp_core_test_app_uart_wkup_sleep_bin_end[]   asm("_binary_lp_core_test_app_uart_wkup_sleep_bin_end");

static const char *TAG = "lp_uart_sleep";

#define HP_UART_PORT UART_NUM_1
#define HP_UART_BUF  256

extern uint32_t lp_sleep_uart_clkdiv_reg(void);

static RTC_DATA_ATTR uint32_t s_uart_clkdiv_before;

static bool buffer_contains(const uint8_t *buf, int len, const void *pat, int pat_len)
{
    if (pat_len <= 0 || pat_len > len) {
        return false;
    }
    for (int i = 0; i <= len - pat_len; i++) {
        if (memcmp(buf + i, pat, pat_len) == 0) {
            return true;
        }
    }
    return false;
}

static void hp_uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    TEST_ESP_OK(uart_driver_install(HP_UART_PORT, HP_UART_BUF, 0, 0, NULL, 0));
    TEST_ESP_OK(uart_param_config(HP_UART_PORT, &uart_config));
    /* Straight same-index wires. HP TX is placed on the LP RX pin. */
    TEST_ESP_OK(uart_set_pin(HP_UART_PORT,
                             LP_UART_DEFAULT_RX_GPIO_NUM,
                             LP_UART_DEFAULT_TX_GPIO_NUM,
                             UART_PIN_NO_CHANGE,
                             UART_PIN_NO_CHANGE));
    TEST_ESP_OK(uart_flush_input(HP_UART_PORT));
}

static void lp_uart_keep_pins(void)
{
    lp_sleep_keep_pin(LP_UART_DEFAULT_TX_GPIO_NUM);
    lp_sleep_keep_pin(LP_UART_DEFAULT_RX_GPIO_NUM);
}

static void lp_uart_init_once(void)
{
    lp_core_uart_cfg_t cfg = LP_CORE_UART_DEFAULT_CONFIG();
    TEST_ESP_OK(lp_core_uart_init(&cfg));
    lp_uart_keep_pins();
    s_uart_clkdiv_before = lp_sleep_uart_clkdiv_reg();
    printf("LP UART clkdiv after init: 0x%08" PRIx32 "\n", s_uart_clkdiv_before);
}

static void lp_uart_check_transfer(void)
{
    uint32_t clkdiv = lp_sleep_uart_clkdiv_reg();
    printf("LP UART clkdiv: before 0x%08" PRIx32 " now 0x%08" PRIx32 "\n", s_uart_clkdiv_before, clkdiv);
    TEST_ASSERT_MESSAGE(s_uart_clkdiv_before != 0, "LP UART clkdiv was zero after init");
    TEST_ASSERT_EQUAL_HEX32(s_uart_clkdiv_before, clkdiv);

    printf("LP UART read returned %ld\n", (long)ulp_uart_sleep_rx_len);
    TEST_ASSERT_EQUAL(LP_SLEEP_UART_LEN, (int32_t)ulp_uart_sleep_rx_len);

    uint8_t expected[LP_SLEEP_UART_LEN];
    lp_sleep_fill_uart_rx(expected);
    ESP_LOG_BUFFER_HEX(TAG, (uint8_t *)ulp_uart_sleep_rx, LP_SLEEP_UART_LEN);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, (uint8_t *)ulp_uart_sleep_rx, LP_SLEEP_UART_LEN);
}

static int hp_uart_read_until(uint8_t *buf, int cap, const uint8_t *tx, int tx_len, int timeout_ms)
{
    int total = 0;
    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(timeout_ms) && total < cap) {
        int n = uart_read_bytes(HP_UART_PORT, buf + total, cap - total, pdMS_TO_TICKS(100));
        if (n > 0) {
            total += n;
            if (buffer_contains(buf, total, tx, tx_len) &&
                    buffer_contains(buf, total, LP_SLEEP_PRINT, strlen(LP_SLEEP_PRINT))) {
                break;
            }
        }
    }
    return total;
}

static void lp_uart_sleeper_first(void)
{
    lp_uart_init_once();
    lp_sleep_run(lp_core_test_app_uart_sleep_bin_start, lp_core_test_app_uart_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, true);
    hp_core_deep_sleep_enter();
}

static void lp_uart_sleeper_second(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_uart_check_transfer();

    /* Second cycle uses the LP UART configuration retained across deep sleep. */
    lp_uart_keep_pins();
    lp_sleep_run(lp_core_test_app_uart_sleep_bin_start, lp_core_test_app_uart_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER, false);
    hp_core_deep_sleep_enter();
}

static void lp_uart_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    lp_uart_check_transfer();
}

static void lp_uart_peer(void)
{
    uint8_t tx[LP_SLEEP_UART_LEN];
    uint8_t response[LP_SLEEP_UART_LEN];
    uint8_t buf[HP_UART_BUF];
    lp_sleep_fill_uart_tx(tx);
    lp_sleep_fill_uart_rx(response);

    hp_uart_init();

    for (int cycle = 0; cycle < 2; cycle++) {
        printf("UART_PEER_ARMED %d\n", cycle);
        fflush(stdout);

        int total = hp_uart_read_until(buf, sizeof(buf), tx, LP_SLEEP_UART_LEN, 8000);
        ESP_LOGI(TAG, "Peer received %d bytes", total);
        ESP_LOG_BUFFER_HEX(TAG, buf, total);
        TEST_ASSERT_MESSAGE(buffer_contains(buf, total, tx, LP_SLEEP_UART_LEN), "LP UART TX payload missing");
        TEST_ASSERT_MESSAGE(buffer_contains(buf, total, LP_SLEEP_PRINT, strlen(LP_SLEEP_PRINT)),
                            "LP UART print missing");

        TEST_ESP_OK(uart_flush_input(HP_UART_PORT));
        uart_write_bytes(HP_UART_PORT, response, LP_SLEEP_UART_LEN);
        TEST_ESP_OK(uart_wait_tx_done(HP_UART_PORT, pdMS_TO_TICKS(200)));
        printf("UART_PEER_OK %d\n", cycle);
        fflush(stdout);
    }

    TEST_ESP_OK(uart_driver_delete(HP_UART_PORT));
}

static void lp_uart_wkup_sleeper_enter(void)
{
    lp_uart_init_once();
    uart_wakeup_cfg_t wake_up_cfg = {
        .wakeup_mode = UART_WK_MODE_CHAR_SEQ,
        .wake_chars_seq = "he**o",
    };
    TEST_ESP_OK(lp_core_uart_wakeup_setup(&wake_up_cfg));
    lp_core_uart_clear_buf();

    lp_sleep_run(lp_core_test_app_uart_wkup_sleep_bin_start, lp_core_test_app_uart_wkup_sleep_bin_end,
                 ULP_LP_CORE_WAKEUP_SOURCE_LP_UART, true);
    hp_core_deep_sleep_enter();
}

static void lp_uart_wkup_sleeper_check(void)
{
    lp_sleep_expect_ulp_wakeup();
    printf("LP UART wakeup cause: 0x%" PRIx32 "\n", ulp_uart_wkup_cause);
    TEST_ASSERT_EQUAL_HEX32(LP_CORE_LL_WAKEUP_SOURCE_LP_UART, ulp_uart_wkup_cause);
    TEST_ASSERT_EQUAL_HEX32(s_uart_clkdiv_before, lp_sleep_uart_clkdiv_reg());
}

static void lp_uart_wkup_peer(void)
{
    uint8_t buf[HP_UART_BUF];
    int total = 0;

    hp_uart_init();
    printf("UART_WKUP_PEER_ARMED\n");
    fflush(stdout);
    unity_wait_for_signal("send");

    uart_write_bytes(HP_UART_PORT, LP_SLEEP_UART_WAKE_SEQ, strlen(LP_SLEEP_UART_WAKE_SEQ));
    TEST_ESP_OK(uart_wait_tx_done(HP_UART_PORT, pdMS_TO_TICKS(200)));

    TickType_t start = xTaskGetTickCount();
    while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(5000) && total < (int)sizeof(buf)) {
        int n = uart_read_bytes(HP_UART_PORT, buf + total, sizeof(buf) - total, pdMS_TO_TICKS(100));
        if (n > 0) {
            total += n;
            if (buffer_contains(buf, total, LP_SLEEP_UART_WAKE_REPLY, strlen(LP_SLEEP_UART_WAKE_REPLY))) {
                break;
            }
        }
    }
    ESP_LOGI(TAG, "Wakeup peer received %d bytes", total);
    ESP_LOG_BUFFER_HEX(TAG, buf, total);
    TEST_ASSERT_MESSAGE(buffer_contains(buf, total, LP_SLEEP_UART_WAKE_REPLY, strlen(LP_SLEEP_UART_WAKE_REPLY)),
                        "LP UART did not print after character-sequence wakeup");
    TEST_ESP_OK(uart_driver_delete(HP_UART_PORT));
}

TEST_CASE_MULTIPLE_STAGES(
    "LP UART TX RX print and FIFO during deep sleep",
    "[lp_core][lp_uart_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_uart_sleeper_first,
    lp_uart_sleeper_second,
    lp_uart_sleeper_check);

TEST_CASE(
    "LP UART peer during deep sleep",
    "[lp_core][lp_uart_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_uart_peer();
}

TEST_CASE_MULTIPLE_STAGES(
    "LP UART character sequence wakeup from deep sleep",
    "[lp_core][lp_uart_wkup_sleep][test_env=generic_multi_device][timeout=120][deepsleep]",
    lp_uart_wkup_sleeper_enter,
    lp_uart_wkup_sleeper_check);

TEST_CASE(
    "LP UART wakeup peer during deep sleep",
    "[lp_core][lp_uart_wkup_sleep][test_env=generic_multi_device][timeout=120]")
{
    lp_uart_wkup_peer();
}
