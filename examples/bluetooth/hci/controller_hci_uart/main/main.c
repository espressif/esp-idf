/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sdkconfig.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_log.h"

#if CONFIG_ENABLE_HCI_CONSOLE
#include "hci_common.h"
#endif

static const char *tag = "CONTROLLER_UART_HCI";

void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS — it is used to store PHY calibration data */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Bluetooth Controller initialize failed: %s", esp_err_to_name(ret));
        return;
    }

#if defined(CONFIG_BTDM_CTRL_MODE_BTDM) && CONFIG_BTDM_CTRL_MODE_BTDM
    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
#elif defined(CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY) && CONFIG_BTDM_CTRL_MODE_BR_EDR_ONLY
    ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
#else
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
#endif
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Bluetooth Controller enable failed: %s", esp_err_to_name(ret));
        return;
    }

#if CONFIG_ENABLE_HCI_CONSOLE
    hci_console_init();
#endif
}
