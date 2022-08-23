/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"

#ifndef CONFIG_BT_LE_HCI_INTERFACE_USE_UART
#error "Please Enable Uart for HCI"
#endif

#define TAG "BLE_HCI"

void
app_main(void)
{
    esp_err_t ret;
    /* Initialize NVS — it is used to store PHY calibration data */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_bt_controller_config_t config_opts = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    /*
     *  Initialize Bluetooth Controller parameters.
     */
    ESP_ERROR_CHECK(esp_bt_controller_init(&config_opts));

    /*
     * Enable the task stack of the Bluetooth Controller.
     */
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));


}
