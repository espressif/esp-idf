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

static esp_err_t external_storage_mac_get(uint8_t *mac)
{
    /* This function simulates getting a base MAC address from external storage
     * by simply setting the base MAC to an arbitrary address. Users should
     * re-implement this function to access external storage (e.g. flash, EEPROM) */
    uint8_t external_storage_mac_addr[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    if (mac == NULL) {
        ESP_LOGE(TAG, "The mac parameter is NULL");
        abort();
    }

    memcpy(mac, external_storage_mac_addr, 6);
    return ESP_OK;
}
#endif//CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE

void app_main(void)
{
    //Get the base MAC address from different sources
    uint8_t base_mac_addr[6] = {0};
    esp_err_t ret = ESP_OK;
#ifdef CONFIG_BASE_MAC_STORED_EFUSE_BLK3
    //Get base MAC address from EFUSE BLK3
    ret = esp_efuse_mac_get_custom(base_mac_addr);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get base MAC address from EFUSE BLK3. (%s)", esp_err_to_name(ret));
#ifdef CONFIG_BASE_MAC_STORED_EFUSE_BLK3_ERROR_ABORT
        ESP_LOGE(TAG, "Aborting");
        abort();
#else
        ESP_LOGI(TAG, "Defaulting to base MAC address in BLK0 of EFUSE");
        esp_efuse_mac_get_default(base_mac_addr);
        ESP_LOGI(TAG, "Base MAC Address read from EFUSE BLK0");
#endif//CONFIG_BASE_MAC_STORED_EFUSE_BLK3_ERROR_ABORT
    } else {
        ESP_LOGI(TAG, "Base MAC Address read from EFUSE BLK3");
    }
#elif defined(CONFIG_BASE_MAC_STORED_OTHER_EXTERNAL_STORAGE)
    //Get base MAC address from other external storage, or set by software
    ret = external_storage_mac_get(base_mac_addr);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get base MAC address from external storage. (%s)", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Aborting");
        abort();
    } else {
        ESP_LOGI(TAG, "Base MAC Address read from external storage");
    }
#else
    //Get base MAC address from EFUSE BLK0(default option)
    ret = esp_efuse_mac_get_default(base_mac_addr);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get base MAC address from EFUSE BLK0. (%s)", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Aborting");
        abort();
    } else {
        ESP_LOGI(TAG, "Base MAC Address read from EFUSE BLK0");
    }
#endif

    //Set the base MAC address using the retrieved MAC address
    ESP_LOGI(TAG, "Using \"0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\" as base MAC address",
             base_mac_addr[0], base_mac_addr[1], base_mac_addr[2], base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);
    esp_base_mac_addr_set(base_mac_addr);

    //Get the derived MAC address for each network interface
    uint8_t derived_mac_addr[6] = {0};
    //Get MAC address for WiFi Station interface
    ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA));
    ESP_LOGI("WIFI_STA MAC", "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
             derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
             derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);

    //Get MAC address for SoftAp interface
    ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_SOFTAP));
    ESP_LOGI("SoftAP MAC", "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
             derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
             derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);

    //Get MAC address for Bluetooth
    ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_BT));
    ESP_LOGI("BT MAC", "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
             derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
             derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);

    //Get MAC address for Ethernet
    ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_ETH));
    ESP_LOGI("Ethernet MAC", "0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
             derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
             derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
}
