/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Only platform dependencies are faked; compile the production sync module below. */
#define CONFIG_BLE_LOG_SYNC_IO_NUM 0
#define CONFIG_ESP_CONSOLE_UART_NUM 0
#define CONFIG_IDF_TARGET_ESP32C61 1
#define pdMS_TO_TICKS(n) (n)
#define pdTICKS_TO_MS(n) (n)
typedef int portMUX_TYPE;
#define portENTER_CRITICAL_SAFE(lock) (++s_critical_depth)
#define portEXIT_CRITICAL_SAFE(lock) exit_critical()
#include "ble_log_util.h"
#include "ble_log_sync.h"

static int64_t s_now, s_sampled_at;
static int s_critical_depth, s_samples, s_sample_delay;
#if CONFIG_BLE_LOG_LL_ENABLED
#define ESP_BT_CONTROLLER_STATUS_IDLE 0
static bool s_controller_idle;
static int esp_bt_controller_get_status(void) { return !s_controller_idle; }
uint32_t r_ble_lll_timer_current_tick_get(void)
{
    assert(s_critical_depth > 0 && !s_controller_idle);
    return s_now + 1;
}
#endif
static void exit_critical(void)
{
    assert(s_critical_depth > 0);
    --s_critical_depth;
}
static int64_t esp_timer_get_time(void) { return s_now; }
static uint32_t xTaskGetTickCountFromISR(void)
{
    assert(s_critical_depth > 0);
    s_sampled_at = s_now;
    s_now += s_sample_delay;
    s_samples++;
    return s_sampled_at / 1000;
}

#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_LOGE(tag, ...) record_error(__VA_ARGS__)
#define GPIO_IS_VALID_OUTPUT_GPIO(n) (!s_invalid_gpio)
#define UART_NUM_1 1
#define UART_SIGNAL_TXD_INV (1U << 5)
#define UART_DATA_8_BITS 8
#define UART_PARITY_DISABLE 0
#define UART_STOP_BITS_1 1
#define UART_HW_FLOWCTRL_DISABLE 0
#define UART_SCLK_XTAL 1
#define UART_SCLK_REF_TICK 2
typedef struct { int baud_rate, data_bits, parity, stop_bits, flow_ctrl, source_clk; } uart_config_t;
typedef int esp_err_t;

static int s_level, s_submissions;
static int s_tx_result, s_delay_us;
static int64_t s_low_at, s_high_at;
static uint16_t s_tx_idle_bits;
static bool s_transmitting, s_delayed_start, s_idle_done;
static bool s_fail_idle_check;
static bool s_uart_claimed, s_fail_uart_config;
static bool s_invalid_gpio, s_fail_uart_install, s_fail_gpio_input;
static char s_error_message[256];
static bool s_input_enabled, s_pin_connected, s_inverted;
static int s_pin_configs, s_inverse_delay, s_inverse_settle_delay;
static uint8_t s_wire_payload[7];

static const char *esp_err_to_name(esp_err_t err) { return "ESP_FAIL"; }
static void record_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(s_error_message, sizeof(s_error_message), format, args);
    va_end(args);
}
static int gpio_get_level(int pin) { assert(s_pin_connected); return s_level; }
static int gpio_input_enable(int pin)
{
    if (s_fail_gpio_input) {
        return ESP_FAIL;
    }
    s_input_enabled = true;
    return ESP_OK;
}
static int gpio_reset_pin(int pin)
{
    assert(!s_critical_depth);
    s_input_enabled = s_pin_connected = false;
    s_level = 0;
    return ESP_OK;
}
static int uart_set_line_inverse(int uart, uint32_t mask)
{
    assert(s_uart_claimed && s_pin_connected && !s_transmitting);
    assert(mask == 0 || mask == UART_SIGNAL_TXD_INV);
    int outer_depth = s_critical_depth;
    assert(mask ? outer_depth > 0 : outer_depth == 0);
    ++s_critical_depth; /* The public driver nests its own critical section. */
    s_now += s_inverse_delay;
    s_inverted = mask != 0;
    s_level = !s_inverted;
    if (s_inverted) {
        s_low_at = s_now;
    } else {
        s_high_at = s_now;
    }
    s_now += s_inverse_settle_delay;
    exit_critical();
    assert(s_critical_depth == outer_depth);
    return ESP_OK;
}
static bool uart_is_driver_installed(int uart) { return s_uart_claimed; }
static int uart_driver_install(int uart, int rx, int tx, int size, void *queue, int flags)
{
    assert(!s_uart_claimed && uart == UART_NUM_1 && rx == 256 && tx == 0 && size == 0 && queue == NULL);
    if (s_fail_uart_install) {
        return ESP_FAIL;
    }
    s_uart_claimed = true;
    s_tx_idle_bits = 0;
    s_pin_configs = 0;
    s_idle_done = true;
    return ESP_OK;
}
static int uart_param_config(int uart, const uart_config_t *config)
{
    assert(config->baud_rate == 1000000 && config->data_bits == UART_DATA_8_BITS);
    assert(config->parity == UART_PARITY_DISABLE && config->stop_bits == UART_STOP_BITS_1);
    assert(config->flow_ctrl == UART_HW_FLOWCTRL_DISABLE);
    assert(config->source_clk == (SOC_UART_SUPPORT_XTAL_CLK ? UART_SCLK_XTAL : UART_SCLK_REF_TICK));
    return s_fail_uart_config ? ESP_FAIL : ESP_OK;
}
static int uart_set_pin(int uart, int tx, int rx, int rts, int cts)
{
    assert(!s_critical_depth && !s_transmitting && !s_pin_connected);
    assert(tx == CONFIG_BLE_LOG_SYNC_IO_NUM && rx == -1 && rts == -1 && cts == -1);
    assert(++s_pin_configs == 1); /* Routing is configured once, never per marker. */
    s_pin_connected = true;
    s_level = !s_inverted;
    return ESP_OK;
}
static int uart_disable_rx_intr(int uart) { return ESP_OK; }
static int uart_set_tx_idle_num(int uart, uint16_t bits) { s_tx_idle_bits = bits; return ESP_OK; }
static int uart_driver_delete(int uart) { assert(s_uart_claimed); s_uart_claimed = false; return ESP_OK; }
static int uart_wait_tx_done(int uart, int wait)
{
    assert(!s_critical_depth && s_uart_claimed);
    if (wait) {
        s_transmitting = false;
        s_idle_done = true;
        return ESP_OK;
    }
    s_now += 2; /* Current public driver has a fixed FSM settling delay. */
    return !s_fail_idle_check && !s_transmitting && s_idle_done ? ESP_OK : ESP_FAIL;
}
static void esp_rom_delay_us(uint32_t us)
{
    assert(!s_critical_depth);
    s_delay_us += us;
    s_now += us;
}
static int uart_tx_chars(int uart, const char *data, uint32_t size)
{
    assert(!s_critical_depth && !s_inverted && size == 7);
    assert(s_pin_connected && s_level == 1 && !s_transmitting);
    assert(s_now - s_high_at >= 2);
    s_submissions++;
    s_now += 2;
    if (s_tx_result > 0) {
        memcpy(s_wire_payload, data, s_tx_result);
        s_transmitting = true;
        s_idle_done = false;
        s_level = s_delayed_start ? 1 : 0;
    }
    return s_tx_result;
}

#endif

#include "../../../src/ble_log_sync.c"

#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED

static void finish_tx(void)
{
    s_now += 68;
    s_transmitting = false;
    s_level = 1; /* Hardware completion: no software callback runs. */
}
static void finish_idle(void)
{
    s_now += s_tx_idle_bits;
    s_idle_done = true;
}
static void setup(void)
{
    s_fail_idle_check = false;
    s_now = 0; /* First-send guard also works at boot time zero. */
    s_submissions = s_samples = 0;
    s_delayed_start = false;
    s_sample_delay = s_delay_us = 0;
    s_tx_result = 7;
    assert(ble_log_sync_init());
    assert(s_tx_idle_bits == 64);
    assert(s_level == 1 && s_pin_connected && !s_inverted && s_pin_configs == 1);
    assert(ble_log_sync_get_level() == 1);
    ble_log_ts_info_t early = {0};
    s_now = 63;
    assert(!ble_log_sync_send(0, &early));
    assert(!s_samples && !s_submissions && !early.esp_ts);
    s_now++;
}
static void teardown(void)
{
    ble_log_sync_deinit();
    assert(!s_transmitting && !s_uart_claimed);
    assert(!s_critical_depth && !s_level && !s_pin_connected);
}
static void check_async(void)
{
    setup();
    ble_log_ts_info_t info, saved;
    s_now = 0x78563410;
    assert(ble_log_sync_send(0xff123456, &info));
    saved = info;
    assert(info.esp_ts == (uint32_t)s_sampled_at && info.io_level == 0 && s_samples == 1);
    assert(!ble_log_sync_send(99, &info));
    const uint8_t expected[] = {0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78};
    assert(!memcmp(s_wire_payload, expected, sizeof(expected)));
    finish_tx();
    assert(s_level == 1 && s_pin_connected);
    s_now += 63;
    assert(!ble_log_sync_send(99, &info)); /* FIFO empty alone is insufficient. */
    assert(!memcmp(&saved, &info, sizeof(info)));
    s_now++;
    s_idle_done = true;
    s_fail_idle_check = true;
    assert(!ble_log_sync_send(99, &info)); /* A failed public status check must not change the line. */
    s_fail_idle_check = false;
    s_delayed_start = true;
    assert(ble_log_sync_send(99, &info));
    assert(info.io_level == 0 && info.esp_ts == (uint32_t)s_sampled_at && s_samples == 2);
    assert(!s_idle_done);
    finish_tx();
    finish_idle();
    assert(ble_log_sync_send(0xffffff, &info));
    assert(s_wire_payload[0] == 255 && s_wire_payload[1] == 255 && s_wire_payload[2] == 255);
    teardown(); /* Only shutdown waits for TX completion. */
}
static void check_reinit(void)
{
    setup();
    ble_log_ts_info_t info = {0};
    assert(ble_log_sync_send(0, &info));
    assert(!memcmp(s_wire_payload, (uint8_t[3]) {0}, 3));
    teardown();
    ble_log_ts_info_t saved = info;
    assert(!ble_log_sync_send(1, &info));
    assert(!memcmp(&saved, &info, sizeof(info)));
    setup();
    assert(ble_log_sync_send(0, &info));
    assert(s_samples == 1 && s_submissions == 1);
    teardown();
}
static void check_send_failure(void)
{
    for (int result = -1; result < 7; result++) {
        setup();
        s_tx_result = result;
        ble_log_ts_info_t info;
        assert(ble_log_sync_send(42, &info)); /* T0 stays valid even without a complete label. */
        assert(info.io_level == 0 && info.esp_ts == (uint32_t)s_sampled_at && s_samples == 1);
        ble_log_ts_info_t saved = info;
        assert(!ble_log_sync_send(43, &info));
        assert(!memcmp(&saved, &info, sizeof(info)));
        if (s_transmitting) {
            finish_tx();
            finish_idle();
        }
        s_now += 64;
        s_tx_result = 7;
        assert(ble_log_sync_send(43, &info)); /* Zero-byte failure must not block the next marker. */
        assert(s_samples == 2 && s_submissions == 2);
        teardown();
    }
}
static void check_break_timing(void)
{
    const int durations[] = {0, 5, 30};
    for (unsigned i = 0; i < sizeof(durations) / sizeof(durations[0]); i++) {
        setup();
        s_sample_delay = durations[i];
        s_now = UINT32_MAX - 6LL;
        ble_log_ts_info_t info;
        assert(ble_log_sync_send(0, &info));
        assert(info.esp_ts == UINT32_MAX - 4U);
        const uint8_t expected[] = {0, 0, 0, 0xfb, 0xff, 0xff, 0xff};
        assert(!memcmp(s_wire_payload, expected, sizeof(expected)));
        /* Only the unelapsed BREAK time is waited, plus a 1 us rounding guard. */
        assert(s_delay_us == (s_sample_delay < 13 ? 13 - s_sample_delay : 0) + 2);
        teardown();
    }
}
static void check_inverse_timing(void)
{
    setup();
    s_inverse_delay = 7;
    s_inverse_settle_delay = 3;
    s_now = 100;
    ble_log_ts_info_t info;
    assert(ble_log_sync_send(42, &info));
    assert(s_low_at == 109 && info.esp_ts == 112);
    assert(s_high_at - s_low_at == 3 + 13 + s_inverse_delay);
    assert(s_pin_configs == 1 && s_samples == 1 && s_submissions == 1);
    assert(!s_critical_depth && !s_inverted);
    s_inverse_delay = s_inverse_settle_delay = 0;
    teardown();
}
static void check_init_failure(void)
{
    s_uart_claimed = true;
    assert(!ble_log_sync_init());
    assert(strstr(s_error_message, "already installed"));
    ble_log_sync_deinit();
    assert(s_uart_claimed); /* Never delete another user's UART driver. */
    s_uart_claimed = false;
    bool *failures[] = {&s_invalid_gpio, &s_fail_uart_install, &s_fail_uart_config, &s_fail_gpio_input};
    const char *reasons[] = {"Invalid sync output GPIO", "driver installation failed", "configuration failed", "input enable failed"};
    for (unsigned i = 0; i < sizeof(failures) / sizeof(failures[0]); i++) {
        *failures[i] = true;
        assert(!ble_log_sync_init());
        assert(strstr(s_error_message, reasons[i]));
        assert(!s_uart_claimed && !s_pin_connected && !s_input_enabled);
        assert(ble_log_sync_get_level() == 0);
        ble_log_ts_info_t info = {.esp_ts = 123};
        assert(!ble_log_sync_send(0, &info) && info.esp_ts == 123);
        ble_log_sync_deinit();
        *failures[i] = false;
    }
}
#endif

static void check_sampling(void)
{
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    setup();
#endif
    s_now = 1234;
    ble_log_ts_info_t info;
    ble_log_sync_sample(&info);
    assert(info.int_src_code == BLE_LOG_INT_SRC_TS);
    assert(info.esp_ts == 1234 && info.os_ts == 1 && !s_critical_depth);
    assert(info.io_level == CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED);
#if CONFIG_BLE_LOG_LL_ENABLED
    assert(info.lc_ts == 1235);
    s_controller_idle = true;
    ble_log_sync_sample(&info);
    assert(info.lc_ts == 0);
    s_controller_idle = false;
#else
    assert(info.lc_ts == 0);
#endif
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    assert(!s_submissions && !s_inverted);
    s_fail_idle_check = true;
#endif
    int previous_samples = s_samples;
    ble_log_sync_sample_anchor(&info, 42);
    assert(s_samples == previous_samples + 1 && info.esp_ts == (uint32_t)s_sampled_at && !s_critical_depth);
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    assert(!s_submissions && !s_inverted);
    s_fail_idle_check = false;
    s_tx_result = 0;
    previous_samples = s_samples;
    ble_log_sync_sample_anchor(&info, 43);
    assert(s_samples == previous_samples + 1 && s_submissions == 1);
    assert(info.esp_ts == (uint32_t)s_sampled_at && info.io_level == 0);
    teardown();
#endif
    s_now = 5678;
    ble_log_sync_sample_anchor(&info, 44);
    assert(info.io_level == 0 && info.esp_ts == 5678 && !s_critical_depth);
}
int main(void)
{
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    check_init_failure();
    s_inverted = true; /* Initialization restores normal polarity. */
    setup();
    assert(!s_inverted && s_level == 1);
    teardown();
    check_async();
    check_reinit();
    check_send_failure();
    check_break_timing();
    check_inverse_timing();
#endif
    check_sampling();
    puts("sync behavior tests passed");
    return 0;
}
