/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_console.h"
#include "esp_err.h"
#include "esp_log.h"
#include <inttypes.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_rom_sys.h"
#include "esp_rom_serial_output.h"

#if SOC_USB_SERIAL_JTAG_SUPPORTED
static const char *TAG = "usj_repro";
static volatile uint32_t s_blink_counter;
static volatile uint32_t s_verbose_counter;

static void init_rom_printf_on_uart0(void)
{
    const uart_config_t cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));

    /* Force ROM console output to UART0 regardless of primary app console selection. */
    esp_rom_output_set_as_console(UART_NUM_0);
    esp_rom_install_channel_putc(1, esp_rom_output_putc);
}

static void verbose_task(void *arg)
{
    (void)arg;
    esp_rom_printf("%s: verbose task started\n", TAG);

    while (true) {
        ESP_LOGI(TAG, "verbose task alive");
        s_verbose_counter++;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

static void blinker_task(void *arg)
{
    (void)arg;
    esp_rom_printf("%s: blinker task started\n", TAG);

    while (true) {
        s_blink_counter++;
        if ((s_blink_counter % 100) == 0) {
            esp_rom_printf("rom hb blink=%" PRIu32 " verbose=%" PRIu32 "\n", s_blink_counter, s_verbose_counter);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void app_main(void)
{
    init_rom_printf_on_uart0();

    esp_rom_printf("USB-Serial-JTAG REPL no-input repro\n");

    // Assume USB Serial/JTAG is connected at the beginning, the connection monitor will update the status after SOF is received
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();

    esp_rom_printf("Starting blinker task before esp_console_start_repl\n");
    BaseType_t blinker_ret = xTaskCreate(blinker_task, "blinker", 2048, NULL, 5, NULL);
    if (blinker_ret != pdPASS) {
        esp_rom_printf("ERR: blinker task create failed\n");
    }

    esp_rom_printf("Starting console REPL over USB-Serial-JTAG\n");
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
    ESP_ERROR_CHECK(esp_console_start_repl(repl));

    esp_rom_printf("Starting verbose task after esp_console_start_repl\n");
    BaseType_t verbose_ret = xTaskCreate(verbose_task, "verbose", 2048, NULL, 5, NULL);
    if (verbose_ret != pdPASS) {
        esp_rom_printf("ERR: verbose task create failed\n");
    }
}

#else

void app_main(void)
{
    ESP_LOGE("usj_repro", "SOC_USB_SERIAL_JTAG_SUPPORTED is required for this repro");
}

#endif
