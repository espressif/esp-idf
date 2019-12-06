// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include "unity.h"
#include "sdkconfig.h"
#include "soc/cpu.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/clk.h"
#include "esp32/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/clk.h"
#include "esp32s2beta/rom/uart.h"
#endif

static uint32_t s_test_start, s_test_stop;

void unity_putc(int c)
{
    if (c == '\n') {
        uart_tx_one_char('\r');
        uart_tx_one_char('\n');
    } else if (c == '\r') {
    } else {
        uart_tx_one_char(c);
    }
}

void unity_flush(void)
{
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
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
    /* UartRxString length argument is uint8_t */
    if (len >= UINT8_MAX) {
        len = UINT8_MAX;
    }
    /* Flush anything already in the RX buffer */
    uint8_t ignore;
    while (uart_rx_one_char(&ignore) == OK) {
    }
    /* Read input */
    UartRxString((uint8_t *) dst, len);
}

void unity_exec_time_start(void)
{
    RSR(CCOUNT, s_test_start);
}

void unity_exec_time_stop(void)
{
    RSR(CCOUNT, s_test_stop);
}

uint32_t unity_exec_time_get_ms(void)
{
    return (s_test_stop - s_test_start) / (esp_clk_cpu_freq() / 1000);
}
