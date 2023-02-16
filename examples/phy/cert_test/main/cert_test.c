/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "nvs_flash.h"

#include "esp_console.h"
#include "cmd_system.h"

#include "cmd_phy.h"
#include "esp_phy_cert_test.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "phy>";

#if CONFIG_ESP_CONSOLE_UART
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_CDC
    esp_console_dev_usb_cdc_config_t cdc_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&cdc_config, &repl_config, &repl));
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    esp_console_dev_usb_serial_jtag_config_t usbjtag_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usbjtag_config, &repl_config, &repl));
#endif

    /* Register commands */
    register_system();

#if CONFIG_ESP_PHY_ENABLE_CERT_TEST
    register_phy_cmd();

    /* rftest.a requirements */
    esp_wifi_power_domain_on();

    esp_phy_rftest_config(1);
    esp_phy_rftest_init();
#endif

    printf("\n ==================================================\n");
    printf(" |            RF certification test               |\n");
    printf(" |                                                |\n");
    printf(" |  1. Print 'help' to gain overview of commands  |\n");
    printf(" |  2. Wi-Fi certification test                   |\n");
    printf(" |  3. Bluetooth certification test               |\n");
    printf(" |                                                |\n");
    printf(" =================================================\n\n");

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
