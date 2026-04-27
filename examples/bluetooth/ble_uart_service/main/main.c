/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * BLE UART Service example. Backend (NimBLE / Bluedroid) is picked
 * by the host-stack Kconfig at compile time. Whatever the central
 * writes to the RX characteristic is echoed back over TX.
 */

#include <stdio.h>

#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "ble_uart.h"

static const char *TAG = "app";

static void ble_uart_on_rx(const uint8_t *data, size_t len)
{
    ESP_LOGI(TAG, "rx len: %u bytes", (unsigned)len);
    if (data == NULL || len == 0) {
        return;
    }
    ESP_LOG_BUFFER_HEX(TAG, data, len);
    ble_uart_tx(data, len);  /* echo back */
}

void app_main(void)
{
    /* NVS is required by the BT controller (PHY calibration) and the
     * bond store, so it must be live before ble_uart_install(). */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Device name = "<prefix>-XXXX" with XXXX = last two MAC bytes.
     * If esp_read_mac() fails, mac stays zero and the suffix degrades
     * to "0000" — log so the operator notices uniqueness was lost. */
    uint8_t mac[6] = {0};
    esp_err_t mac_err = esp_read_mac(mac, ESP_MAC_BT);
    if (mac_err != ESP_OK) {
        ESP_LOGW(TAG, "esp_read_mac(BT) failed (%s); device name suffix will be 0000",
                 esp_err_to_name(mac_err));
    }
    char name[24];
    snprintf(name, sizeof(name), "%s-%02X%02X",
             CONFIG_BLE_UART_DEVICE_NAME_PREFIX, mac[4], mac[5]);

    ESP_ERROR_CHECK(ble_uart_install(&(ble_uart_config_t){
        .encrypted      = true,
        .device_name    = name,
        .ble_uart_on_rx = ble_uart_on_rx,
    }));

    ESP_ERROR_CHECK(ble_uart_open());
}
