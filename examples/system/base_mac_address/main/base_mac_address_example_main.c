/* Base mac address example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */

#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"

#define TAG "BASE_MAC"

#ifdef CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE
/* This is an example to show getting base MAC address from other external storage (flash, EEPROM, etc). */
static esp_err_t external_storage_mac_get(uint8_t *mac)
{
    uint8_t external_storage_mac_addr[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };

    if (mac == NULL) {
        ESP_LOGE(TAG, "The mac parameter is NULL");
        abort();
    }

    memcpy(mac, external_storage_mac_addr, 6);
    return ESP_OK;
}
#endif//CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE

void app_main()
{
#if defined(CONFIG_BASE_MAC_STORED_EFUSE_BLK3) || defined(CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE)
    uint8_t mac_addr[8] = {0};
    esp_err_t ret = ESP_OK;

#ifdef CONFIG_BASE_MAC_STORED_EFUSE_BLK3
    /* Get base MAC address from BLK3 of EFUSE */
    ret = esp_efuse_mac_get_custom(mac_addr);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Get base MAC address from BLK3 of EFUSE error");
        /* If get custom base MAC address error, the application developer can decide what to do:
         * abort or use the default base MAC address which is stored in BLK0 of EFUSE by doing
         * nothing.
         */
#ifdef CONFIG_BASE_MAC_STORED_EFUSE_BLK3_ERROR_ABORT
        abort();
#else
        ESP_LOGI(TAG, "Use base MAC address which is stored in BLK0 of EFUSE");
#endif//CONFIG_BASE_MAC_STORED_EFUSE_BLK3_ERROR_ABORT
    }
    else {
        esp_base_mac_addr_set(mac_addr);
        ESP_LOGI(TAG, "Use base MAC address which is stored in BLK3 of EFUSE");
    }
#endif//CONFIG_BASE_MAC_STORED_EFUSE_BLK3

#ifdef CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE
    /* the application developer can get base MAC address which is stored in other external
     * storage (flash, EEPROM, etc) by calling some functions here.
     */
    ret = external_storage_mac_get(mac_addr);
    if (ret == ESP_OK) {
        esp_base_mac_addr_set(mac_addr);
        ESP_LOGI(TAG, "Use base MAC address which is stored in other external storage(flash, EEPROM, etc)");
    }
    else {
        ESP_LOGI(TAG, "Use base MAC address which is stored in BLK0 of EFUSE");
    }
#endif//CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE
#else
    ESP_LOGI(TAG, "Use base MAC address which is stored in BLK0 of EFUSE");
#endif//CONFIG_BASE_MAC_STORED_EFUSE_BLK3 || CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE
}
