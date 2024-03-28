/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdbool.h>
#include "unity.h"
#include "sdkconfig.h"
#include "esp_cpu.h"
#include "esp_rom_uart.h"
#include "esp_private/esp_clk.h"

static uint32_t s_test_start, s_test_stop;

void unity_putc(int c)
{
    if (c == '\n') {
        esp_rom_output_tx_one_char('\r');
        esp_rom_output_tx_one_char('\n');
    } else if (c == '\r') {
    } else {
        esp_rom_output_tx_one_char(c);
    }
}

void unity_flush(void)
{
    if(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM != -1) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
    }
}

#define iscontrol(c) ((c) <= '\x1f' || (c) == '\x7f')

static void esp_unity_readline(char* dst, size_t len)
{
    /* Read line from console with support for echoing and backspaces */
    size_t write_index = 0;
    for (;;) {
        char c = 0;
        bool got_char = esp_rom_output_rx_one_char((uint8_t*)&c) == 0;
        if (!got_char) {
          continue;
        }
        if (c == '\r' || c == '\n') {
            /* Add null terminator and return on newline */
            unity_putc('\n');
            dst[write_index] = '\0';
            return;
        } else if (c == '\b') {
            if (write_index > 0) {
                /* Delete previously entered character */
                write_index--;
                esp_rom_output_tx_one_char('\b');
                esp_rom_output_tx_one_char(' ');
                esp_rom_output_tx_one_char('\b');
            }
        } else if (len > 0 && write_index < len - 1 && !iscontrol(c)) {
            /* Write a max of len - 1 characters to allow for null terminator */
            unity_putc(c);
            dst[write_index++] = c;
        }
    }
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
    /* Flush anything already in the RX buffer */
    uint8_t ignore;
    while (esp_rom_output_rx_one_char(&ignore) == 0) { }
    /* Read input */
    esp_unity_readline(dst, len);
}

void unity_exec_time_start(void)
{
    s_test_start = esp_cpu_get_cycle_count();
}

void unity_exec_time_stop(void)
{
    s_test_stop = esp_cpu_get_cycle_count();
}

uint32_t unity_exec_time_get_ms(void)
{
    return (s_test_stop - s_test_start) / (esp_clk_cpu_freq() / 1000);
}
