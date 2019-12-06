// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include "soc/uart_periph.h"
#include "soc/gpio_periph.h"
#include "driver/gpio.h"
#include "esp_core_dump_priv.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/clk.h"
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#include "esp32s2beta/clk.h"
#endif

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_uart";

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_UART

static void esp_core_dump_b64_encode(const uint8_t *src, uint32_t src_len, uint8_t *dst) {
    const static DRAM_ATTR char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i, j, a, b, c;

    for (i = j = 0; i < src_len; i += 3) {
        a = src[i];
        b = i + 1 >= src_len ? 0 : src[i + 1];
        c = i + 2 >= src_len ? 0 : src[i + 2];

        dst[j++] = b64[a >> 2];
        dst[j++] = b64[((a & 3) << 4) | (b >> 4)];
        if (i + 1 < src_len) {
            dst[j++] = b64[(b & 0x0F) << 2 | (c >> 6)];
        }
        if (i + 2 < src_len) {
            dst[j++] = b64[c & 0x3F];
        }
    }
    while (j % 4 != 0) {
        dst[j++] = '=';
    }
    dst[j++] = '\0';
}

static esp_err_t esp_core_dump_uart_write_start(void *priv)
{
    esp_err_t err = ESP_OK;
    ets_printf(DRAM_STR("================= CORE DUMP START =================\r\n"));
    return err;
}

static esp_err_t esp_core_dump_uart_write_end(void *priv)
{
    esp_err_t err = ESP_OK;
    ets_printf(DRAM_STR("================= CORE DUMP END =================\r\n"));
    return err;
}

static esp_err_t esp_core_dump_uart_write_data(void *priv, void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    char buf[64 + 4], *addr = data;
    char *end = addr + data_len;

    while (addr < end) {
        size_t len = end - addr;
        if (len > 48) len = 48;
        /* Copy to stack to avoid alignment restrictions. */
        char *tmp = buf + (sizeof(buf) - len);
        memcpy(tmp, addr, len);
        esp_core_dump_b64_encode((const uint8_t *)tmp, len, (uint8_t *)buf);
        addr += len;
        ets_printf(DRAM_STR("%s\r\n"), buf);
    }

    return err;
}

static int esp_core_dump_uart_get_char(void) {
    int i;
    uint32_t reg = (READ_PERI_REG(UART_STATUS_REG(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
    if (reg) {
        i = READ_PERI_REG(UART_FIFO_REG(0));
    } else {
        i = -1;
    }
    return i;
}

void esp_core_dump_to_uart(XtExcFrame *frame)
{
    core_dump_write_config_t wr_cfg;
    uint32_t tm_end, tm_cur;
    int ch;

    memset(&wr_cfg, 0, sizeof(wr_cfg));
    wr_cfg.prepare = NULL;
    wr_cfg.start = esp_core_dump_uart_write_start;
    wr_cfg.end = esp_core_dump_uart_write_end;
    wr_cfg.write = esp_core_dump_uart_write_data;
    wr_cfg.priv = NULL;

    //Make sure txd/rxd are enabled
    // use direct reg access instead of gpio_pullup_dis which can cause exception when flash cache is disabled
    REG_CLR_BIT(GPIO_PIN_REG_1, FUN_PU);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);

    ESP_COREDUMP_LOGI("Press Enter to print core dump to UART...");
    const int cpu_ticks_per_ms = esp_clk_cpu_freq() / 1000;
    tm_end = xthal_get_ccount() / cpu_ticks_per_ms + CONFIG_ESP32_CORE_DUMP_UART_DELAY;
    ch = esp_core_dump_uart_get_char();
    while (!(ch == '\n' || ch == '\r')) {
        tm_cur = xthal_get_ccount() / cpu_ticks_per_ms;
        if (tm_cur >= tm_end){
            break;
        }
        ch = esp_core_dump_uart_get_char();
    }
    ESP_COREDUMP_LOGI("Print core dump to uart...");
    esp_core_dump_write((void*)frame, &wr_cfg);
    ESP_COREDUMP_LOGI("Core dump has been written to uart.");
}
#endif
