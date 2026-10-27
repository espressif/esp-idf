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

/* Only platform dependencies are faked; compile the production sender below. */
#define CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED 1
#define CONFIG_BLE_LOG_SYNC_IO_NUM 0
#define CONFIG_ESP_CONSOLE_UART_NUM 0
#define CONFIG_BLE_LOG_POOL_TRANS_CNT 8
#define CONFIG_BLE_LOG_POOL_NON_YIELD_RESERVE_CNT 1
#define CONFIG_BLE_LOG_POOL_TRANS_SIZE 640
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_LOGE(tag, ...) record_error(__VA_ARGS__)
#define pdMS_TO_TICKS(n) (n)
#define GPIO_IS_VALID_OUTPUT_GPIO(n) (!invalid_gpio)
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

static int64_t now;
static int level, critical_depth, samples, submissions;
static int tx_result, sample_delay, delay_us;
static int64_t low_at, high_at, sampled_at;
static uint16_t tx_idle_bits;
static bool transmitting, delayed_start, idle_done;
static bool fail_idle_check;
static bool uart_claimed, fail_uart_config;
static bool invalid_gpio, fail_uart_install, fail_gpio_input;
static char error_message[256];
static bool input_enabled, pin_connected, inverted;
static int pin_configs, inverse_delay, inverse_settle_delay;
static uint8_t wire_payload[7];

typedef int portMUX_TYPE;
#define portENTER_CRITICAL_SAFE(lock) (++critical_depth)
#define portEXIT_CRITICAL_SAFE(lock) exit_critical()
#include "ble_log_util.h"
#include "ble_log_sync.h"

static void exit_critical(void)
{
    assert(critical_depth > 0);
    --critical_depth;
}

static int64_t esp_timer_get_time(void) { return now; }
static const char *esp_err_to_name(esp_err_t err) { return "ESP_FAIL"; }
static void record_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(error_message, sizeof(error_message), format, args);
    va_end(args);
}
static int gpio_get_level(int pin) { assert(pin_connected); return level; }
static int gpio_input_enable(int pin)
{
    if (fail_gpio_input) {
        return ESP_FAIL;
    }
    input_enabled = true;
    return ESP_OK;
}
static int gpio_reset_pin(int pin)
{
    assert(!critical_depth);
    input_enabled = pin_connected = false;
    level = 0;
    return ESP_OK;
}
static int uart_set_line_inverse(int uart, uint32_t mask)
{
    assert(uart_claimed && pin_connected && !transmitting);
    assert(mask == 0 || mask == UART_SIGNAL_TXD_INV);
    int outer_depth = critical_depth;
    assert(mask ? outer_depth > 0 : outer_depth == 0);
    ++critical_depth; /* The public driver nests its own critical section. */
    now += inverse_delay;
    inverted = mask != 0;
    level = !inverted;
    if (inverted) {
        low_at = now;
    } else {
        high_at = now;
    }
    now += inverse_settle_delay;
    exit_critical();
    assert(critical_depth == outer_depth);
    return ESP_OK;
}
static void ble_log_rt_ts_sample(ble_log_ts_info_t *info)
{
    assert(critical_depth > 0 && input_enabled && pin_connected && inverted && !transmitting && level == 0);
    int outer_depth = critical_depth;
    BLE_LOG_ENTER_CRITICAL();
    *info = (ble_log_ts_info_t) { .int_src_code = BLE_LOG_INT_SRC_TS, .esp_ts = now, .lc_ts = now + 1, .os_ts = now / 1000, .io_level = level };
    sampled_at = now;
    now += sample_delay;
    samples++;
    BLE_LOG_EXIT_CRITICAL();
    assert(critical_depth == outer_depth);
}
static bool uart_is_driver_installed(int uart) { return uart_claimed; }
static int uart_driver_install(int uart, int rx, int tx, int size, void *queue, int flags)
{
    assert(!uart_claimed && uart == UART_NUM_1 && rx == 256 && tx == 0 && size == 0 && queue == NULL);
    if (fail_uart_install) {
        return ESP_FAIL;
    }
    uart_claimed = true;
    tx_idle_bits = 0;
    pin_configs = 0;
    idle_done = true;
    return ESP_OK;
}
static int uart_param_config(int uart, const uart_config_t *config)
{
    assert(config->baud_rate == 1000000 && config->data_bits == UART_DATA_8_BITS);
    assert(config->parity == UART_PARITY_DISABLE && config->stop_bits == UART_STOP_BITS_1);
    assert(config->flow_ctrl == UART_HW_FLOWCTRL_DISABLE);
    assert(config->source_clk == (SOC_UART_SUPPORT_XTAL_CLK ? UART_SCLK_XTAL : UART_SCLK_REF_TICK));
    return fail_uart_config ? ESP_FAIL : ESP_OK;
}
static int uart_set_pin(int uart, int tx, int rx, int rts, int cts)
{
    assert(!critical_depth && !transmitting && !pin_connected);
    assert(tx == CONFIG_BLE_LOG_SYNC_IO_NUM && rx == -1 && rts == -1 && cts == -1);
    assert(++pin_configs == 1); /* Routing is configured once, never per marker. */
    pin_connected = true;
    level = !inverted;
    return ESP_OK;
}
static int uart_disable_rx_intr(int uart) { return ESP_OK; }
static int uart_set_tx_idle_num(int uart, uint16_t bits) { tx_idle_bits = bits; return ESP_OK; }
static int uart_driver_delete(int uart) { assert(uart_claimed); uart_claimed = false; return ESP_OK; }
static int uart_wait_tx_done(int uart, int wait)
{
    assert(!critical_depth && uart_claimed);
    if (wait) {
        transmitting = false;
        idle_done = true;
        return ESP_OK;
    }
    now += 2; /* Current public driver has a fixed FSM settling delay. */
    return !fail_idle_check && !transmitting && idle_done ? ESP_OK : ESP_FAIL;
}
static void esp_rom_delay_us(uint32_t us)
{
    assert(!critical_depth);
    delay_us += us;
    now += us;
}
static int uart_tx_chars(int uart, const char *data, uint32_t size)
{
    assert(!critical_depth && !inverted && size == 7);
    assert(pin_connected && level == 1 && !transmitting);
    assert(now - high_at >= 2);
    submissions++;
    now += 2;
    if (tx_result > 0) {
        memcpy(wire_payload, data, tx_result);
        transmitting = true;
        idle_done = false;
        level = delayed_start ? 1 : 0;
    }
    return tx_result;
}

#include "../../../src/ble_log_sync.c"

static void finish_tx(void)
{
    now += 68;
    transmitting = false;
    level = 1; /* Hardware completion: no software callback runs. */
}
static void finish_idle(void)
{
    now += tx_idle_bits;
    idle_done = true;
}
static void setup(void)
{
    fail_idle_check = false;
    now = 0; /* First-send guard also works at boot time zero. */
    submissions = samples = 0;
    delayed_start = false;
    sample_delay = delay_us = 0;
    tx_result = 7;
    assert(ble_log_sync_init());
    assert(tx_idle_bits == 64);
    assert(level == 1 && pin_connected && !inverted && pin_configs == 1);
    assert(ble_log_sync_get_level() == 1);
    ble_log_ts_info_t early = {0};
    now = 63;
    assert(!ble_log_sync_send(0, &early));
    assert(!samples && !submissions && !early.esp_ts);
    now++;
}
static void teardown(void)
{
    ble_log_sync_deinit();
    assert(!transmitting && !uart_claimed);
    assert(!critical_depth && !level && !pin_connected);
}
static void check_async(void)
{
    setup();
    ble_log_ts_info_t info, saved;
    now = 0x78563410;
    assert(ble_log_sync_send(0xff123456, &info));
    saved = info;
    assert(info.esp_ts == (uint32_t)sampled_at && info.io_level == 0 && samples == 1);
    assert(!ble_log_sync_send(99, &info));
    const uint8_t expected[] = {0x56, 0x34, 0x12, 0x12, 0x34, 0x56, 0x78};
    assert(!memcmp(wire_payload, expected, sizeof(expected)));
    finish_tx();
    assert(level == 1 && pin_connected);
    now += 63;
    assert(!ble_log_sync_send(99, &info)); /* FIFO empty alone is insufficient. */
    assert(!memcmp(&saved, &info, sizeof(info)));
    now++;
    idle_done = true;
    fail_idle_check = true;
    assert(!ble_log_sync_send(99, &info)); /* A failed public status check must not change the line. */
    fail_idle_check = false;
    delayed_start = true;
    assert(ble_log_sync_send(99, &info));
    assert(info.io_level == 0 && info.esp_ts == (uint32_t)sampled_at && samples == 2);
    assert(!idle_done);
    finish_tx();
    finish_idle();
    assert(ble_log_sync_send(0xffffff, &info));
    assert(wire_payload[0] == 255 && wire_payload[1] == 255 && wire_payload[2] == 255);
    teardown(); /* Only shutdown waits for TX completion. */
}
static void check_reinit(void)
{
    setup();
    ble_log_ts_info_t info = {0};
    assert(ble_log_sync_send(0, &info));
    assert(!memcmp(wire_payload, (uint8_t[3]) {0}, 3));
    teardown();
    ble_log_ts_info_t saved = info;
    assert(!ble_log_sync_send(1, &info));
    assert(!memcmp(&saved, &info, sizeof(info)));
    setup();
    assert(ble_log_sync_send(0, &info));
    assert(samples == 1 && submissions == 1);
    teardown();
}
static void check_send_failure(void)
{
    for (int result = -1; result < 7; result++) {
        setup();
        tx_result = result;
        ble_log_ts_info_t info;
        assert(ble_log_sync_send(42, &info)); /* T0 stays valid even without a complete label. */
        assert(info.io_level == 0 && info.esp_ts == (uint32_t)sampled_at && samples == 1);
        ble_log_ts_info_t saved = info;
        assert(!ble_log_sync_send(43, &info));
        assert(!memcmp(&saved, &info, sizeof(info)));
        if (transmitting) {
            finish_tx();
            finish_idle();
        }
        now += 64;
        tx_result = 7;
        assert(ble_log_sync_send(43, &info)); /* Zero-byte failure must not block the next marker. */
        assert(samples == 2 && submissions == 2);
        teardown();
    }
}
static void check_break_timing(void)
{
    const int durations[] = {0, 5, 30};
    for (unsigned i = 0; i < sizeof(durations) / sizeof(durations[0]); i++) {
        setup();
        sample_delay = durations[i];
        now = UINT32_MAX - 6LL;
        ble_log_ts_info_t info;
        assert(ble_log_sync_send(0, &info));
        assert(info.esp_ts == UINT32_MAX - 4U);
        const uint8_t expected[] = {0, 0, 0, 0xfb, 0xff, 0xff, 0xff};
        assert(!memcmp(wire_payload, expected, sizeof(expected)));
        /* Only the unelapsed BREAK time is waited, plus a 1 us rounding guard. */
        assert(delay_us == (sample_delay < 13 ? 13 - sample_delay : 0) + 2);
        teardown();
    }
}
static void check_inverse_timing(void)
{
    setup();
    inverse_delay = 7;
    inverse_settle_delay = 3;
    now = 100;
    ble_log_ts_info_t info;
    assert(ble_log_sync_send(42, &info));
    assert(low_at == 109 && info.esp_ts == 112);
    assert(high_at - low_at == 3 + 13 + inverse_delay);
    assert(pin_configs == 1 && samples == 1 && submissions == 1);
    assert(!critical_depth && !inverted);
    inverse_delay = inverse_settle_delay = 0;
    teardown();
}
static void check_init_failure(void)
{
    uart_claimed = true;
    assert(!ble_log_sync_init());
    assert(strstr(error_message, "already installed"));
    ble_log_sync_deinit();
    assert(uart_claimed); /* Never delete another user's UART driver. */
    uart_claimed = false;
    bool *failures[] = {&invalid_gpio, &fail_uart_install, &fail_uart_config, &fail_gpio_input};
    const char *reasons[] = {"Invalid sync output GPIO", "driver installation failed", "configuration failed", "input enable failed"};
    for (unsigned i = 0; i < sizeof(failures) / sizeof(failures[0]); i++) {
        *failures[i] = true;
        assert(!ble_log_sync_init());
        assert(strstr(error_message, reasons[i]));
        assert(!uart_claimed && !pin_connected && !input_enabled);
        assert(ble_log_sync_get_level() == 0);
        ble_log_ts_info_t info = {.esp_ts = 123};
        assert(!ble_log_sync_send(0, &info) && info.esp_ts == 123);
        ble_log_sync_deinit();
        *failures[i] = false;
    }
}
int main(void)
{
    check_init_failure();
    inverted = true; /* Initialization restores normal polarity. */
    setup();
    assert(!inverted && level == 1);
    teardown();
    check_async();
    check_reinit();
    check_send_failure();
    check_break_timing();
    check_inverse_timing();
    puts("sync behavior tests passed");
    return 0;
}
