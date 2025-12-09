/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_bt.h"
#include "hal/uhci_ll.h"
#include "hal/uart_ll.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char *tag = "CONTROLLER_UART_HCI";

#define UHCI_PORT_NUM (0)

static void uart_gpio_reset(void)
{
    int __DECLARE_RCC_ATOMIC_ENV __attribute__ ((unused)); // To avoid build errors/warnings about __DECLARE_RCC_ATOMIC_ENV
    uart_ll_enable_bus_clock(CONFIG_BTDM_CTRL_HCI_UART_NO, true);
    uart_ll_reset_register(CONFIG_BTDM_CTRL_HCI_UART_NO);
    uhci_ll_enable_bus_clock(UHCI_PORT_NUM, true);
    uhci_ll_reset_register(UHCI_PORT_NUM);

#ifdef CONFIG_BTDM_CTRL_HCI_UART_NO
    ESP_LOGI(tag, "HCI UART%d Pin select: TX 5, RX 18, CTS 23, RTS 19 Baudrate:%d", CONFIG_BTDM_CTRL_HCI_UART_NO, CONFIG_BTDM_CTRL_HCI_UART_BAUDRATE);

    uart_set_pin(CONFIG_BTDM_CTRL_HCI_UART_NO, 5, 18, 19, 23);
#endif
}

void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS — it is used to store PHY calibration data */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );


    /* As the UART1/2 pin conflict with flash pin, so do matrix of the signal and pin */
    uart_gpio_reset();

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Bluetooth Controller initialize failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Bluetooth Controller initialize failed: %s", esp_err_to_name(ret));
        return;
    }
}
