/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "hal/cpu_hal.h"
#include "esp_rom_uart.h"
#include "esp_private/esp_clk.h"

static uint32_t s_test_start, s_test_stop;

void unity_putc(int c)
{
    if (c == '\n') {
        esp_rom_uart_tx_one_char('\r');
        esp_rom_uart_tx_one_char('\n');
    } else if (c == '\r') {
    } else {
        esp_rom_uart_tx_one_char(c);
    }
}

void unity_flush(void)
{
    esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
}

/* To start a unit test from a GDB session without console input,
 * - set a break at unity_gets ('hb unity_gets')
 * - resume the program ('c')
 * - modify this value to the desired command line ('set {char[64]} unity_input_from_gdb = "5"')
 * - optionally set a break point in the test being debugged
 * - resume the program ('c')
 */
char unity_input_from_gdb[64];

void unity_gets(char *dst, size_t len)
{
    size_t unity_input_from_gdb_len = strlen(unity_input_from_gdb);
    if (unity_input_from_gdb_len > 0 && unity_input_from_gdb_len < len - 1) {
        memcpy(dst, unity_input_from_gdb, unity_input_from_gdb_len);
        dst[unity_input_from_gdb_len] = '\n';
        dst[unity_input_from_gdb_len + 1] = 0;
        memset(unity_input_from_gdb, 0, sizeof(unity_input_from_gdb));
        return;
    }
    /* esp_rom_uart_rx_string length argument is uint8_t */
    if (len >= UINT8_MAX) {
        len = UINT8_MAX;
    }
    /* Flush anything already in the RX buffer */
    uint8_t ignore;
    while (esp_rom_uart_rx_one_char(&ignore) == 0) {
    }
    /* Read input */
    esp_rom_uart_rx_string((uint8_t *) dst, len);
}

void unity_exec_time_start(void)
{
    s_test_start = cpu_hal_get_cycle_count();
}

void unity_exec_time_stop(void)
{
    s_test_stop = cpu_hal_get_cycle_count();
}

uint32_t unity_exec_time_get_ms(void)
{
    return (s_test_stop - s_test_start) / (esp_clk_cpu_freq() / 1000);
}
