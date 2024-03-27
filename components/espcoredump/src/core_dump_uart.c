/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "soc/uart_reg.h"
#include "soc/gpio_periph.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "esp_core_dump_types.h"
#include "esp_core_dump_port.h"
#include "esp_core_dump_common.h"
#include "esp_rom_sys.h"

const static char TAG[] __attribute__((unused)) = "esp_core_dump_uart";

#if CONFIG_ESP_COREDUMP_ENABLE_TO_UART

void esp_core_dump_print_write_start(void) __attribute__((alias("esp_core_dump_uart_print_write_start")));
void esp_core_dump_print_write_end(void) __attribute__((alias("esp_core_dump_uart_print_write_end")));
esp_err_t esp_core_dump_write_init(void) __attribute__((alias("esp_core_dump_uart_hw_init")));
esp_err_t esp_core_dump_write_prepare(core_dump_write_data_t *wr_data, uint32_t *data_len) __attribute__((alias("esp_core_dump_uart_write_prepare")));
esp_err_t esp_core_dump_write_start(core_dump_write_data_t *wr_data) __attribute__((alias("esp_core_dump_uart_write_start")));
esp_err_t esp_core_dump_write_end(core_dump_write_data_t *wr_data) __attribute__((alias("esp_core_dump_uart_write_end")));
esp_err_t esp_core_dump_write_data(core_dump_write_data_t *wr_data, void *data, uint32_t data_len) __attribute__((alias("esp_core_dump_uart_write_data")));

/* This function exists on every board, thus, we don't need to specify
 * explicitly the header for each board. */
int esp_clk_cpu_freq(void);

static void esp_core_dump_b64_encode(const uint8_t *src, uint32_t src_len, uint8_t *dst)
{
    const static char b64[] =
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

static void esp_core_dump_uart_print_write_start(void)
{
    ESP_COREDUMP_LOGI("Print core dump to uart...");
}

static void esp_core_dump_uart_print_write_end(void)
{
    ESP_COREDUMP_LOGI("Core dump has been written to uart.");
}

static esp_err_t esp_core_dump_uart_write_start(core_dump_write_data_t *wr_data)
{
    esp_err_t err = ESP_OK;

    ESP_COREDUMP_ASSERT(wr_data != NULL);
    esp_core_dump_checksum_init(&wr_data->checksum_ctx);
    ESP_COREDUMP_PRINT("================= CORE DUMP START =================\r\n");
    return err;
}

static esp_err_t esp_core_dump_uart_write_prepare(core_dump_write_data_t *wr_data, uint32_t *data_len)
{
    ESP_COREDUMP_ASSERT(data_len != NULL);
    *data_len += esp_core_dump_checksum_size();
    return ESP_OK;
}

static esp_err_t esp_core_dump_uart_write_end(core_dump_write_data_t *wr_data)
{
    esp_err_t err = ESP_OK;
    char buf[64 + 4] = { 0 };
    core_dump_checksum_bytes cs_addr = NULL;
    if (wr_data) {
        size_t cs_len = esp_core_dump_checksum_finish(&wr_data->checksum_ctx, &cs_addr);
        wr_data->off += cs_len;
        esp_core_dump_b64_encode((const uint8_t *)cs_addr, cs_len, (uint8_t*)&buf[0]);
        ESP_COREDUMP_PRINT("%s\r\n", buf);
    }
    ESP_COREDUMP_PRINT("================= CORE DUMP END =================\r\n");

    if (cs_addr) {
        esp_core_dump_print_checksum("Coredump checksum", cs_addr);
    }

    return err;
}

static esp_err_t esp_core_dump_uart_write_data(core_dump_write_data_t *wr_data, void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    char buf[64 + 4] = { 0 };
    char *addr = data;
    char *end = addr + data_len;

    ESP_COREDUMP_ASSERT(data != NULL);

    while (addr < end) {
        size_t len = end - addr;
        if (len > 48) {
            len = 48;
        }
        /* Copy to stack to avoid alignment restrictions. */
        char *tmp = buf + (sizeof(buf) - len);
        memcpy(tmp, addr, len);
        esp_core_dump_b64_encode((const uint8_t *)tmp, len, (uint8_t *)buf);
        addr += len;
        ESP_COREDUMP_PRINT("%s\r\n", buf);
    }

    if (wr_data) {
        wr_data->off += data_len;
        esp_core_dump_checksum_update(&wr_data->checksum_ctx, data, data_len);
    }
    return err;
}

static int esp_core_dump_uart_get_char(void)
{
    int i = -1;
    uint32_t reg = (READ_PERI_REG(UART_STATUS_REG(0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
    if (reg) {
        i = READ_PERI_REG(UART_FIFO_REG(0));
    }
    return i;
}

static esp_err_t esp_core_dump_uart_hw_init(void)
{
    uint32_t tm_end = 0;
    uint32_t tm_cur = 0;
    int ch = 0;

    // TODO: move chip dependent code to portable part
    //Make sure txd/rxd are enabled
    // use direct reg access instead of gpio_pullup_dis which can cause exception when flash cache is disabled
    REG_CLR_BIT(GPIO_PIN_REG_1, FUN_PU);
#if CONFIG_IDF_TARGET_ESP32P4
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_U_PAD_GPIO38, FUNC_GPIO38_UART0_RXD_PAD);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_U_PAD_GPIO37, FUNC_GPIO37_UART0_TXD_PAD);
#else
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD_U0RXD);
    gpio_hal_iomux_func_sel(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD_U0TXD);
#endif
    ESP_COREDUMP_LOGI("Press Enter to print core dump to UART...");
    const int cpu_ticks_per_ms = esp_clk_cpu_freq() / 1000;
    tm_end = esp_cpu_get_cycle_count() / cpu_ticks_per_ms + CONFIG_ESP_COREDUMP_UART_DELAY;
    ch = esp_core_dump_uart_get_char();
    while (!(ch == '\n' || ch == '\r')) {
        tm_cur = esp_cpu_get_cycle_count() / cpu_ticks_per_ms;
        if (tm_cur >= tm_end) {
            break;
        }
        ch = esp_core_dump_uart_get_char();
    }

    return ESP_OK;
}

void esp_core_dump_init(void)
{
    ESP_COREDUMP_LOGI("Init core dump to UART");
}
#endif
