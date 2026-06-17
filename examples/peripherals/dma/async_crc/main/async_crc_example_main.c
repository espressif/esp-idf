/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_console.h"
#include "cmd_crc.h"

void app_main(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "crc>";

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    // initialize the CRC engine and register the CRC command
    register_crc();

    printf("\n ==============================================================\n");
    printf("             Steps to Use CRC                                  \n");
    printf("                                                               \n");
    printf("  1. Try 'help', check all supported commands                  \n");
    printf("  2. Try 'crc \"test\"' to calculate the default 8-bit CRC     \n");
    printf("  3. Or customize: 'crc --width=16 --poly=0x1021 \"test\"'     \n");
    printf("                                                               \n");
    printf(" ==============================================================\n\n");

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
