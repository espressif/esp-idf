#include <string.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/efuse.h"
#endif

/* esp_system.h APIs relating to MAC addresses */

static const char* TAG = "system_api";

static uint8_t base_mac_addr[6] = { 0 };

esp_err_t esp_base_mac_addr_set(const uint8_t *mac)
{
    if (mac == NULL) {
        ESP_LOGE(TAG, "Base MAC address is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if (mac[0] & 0x01) {
        ESP_LOGE(TAG, "Base MAC must be a unicast MAC");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(base_mac_addr, mac, 6);

    return ESP_OK;
}

esp_err_t esp_base_mac_addr_get(uint8_t *mac)
{
    uint8_t null_mac[6] = {0};

    if (memcmp(base_mac_addr, null_mac, 6) == 0) {
        ESP_LOGI(TAG, "Base MAC address is not set, read default base MAC address from BLK0 of EFUSE");
        return ESP_ERR_INVALID_MAC;
    }

    memcpy(mac, base_mac_addr, 6);

    return ESP_OK;
}

esp_err_t esp_efuse_mac_get_custom(uint8_t *mac)
{
#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
    return ESP_ERR_NOT_SUPPORTED; // TODO: support custom MAC in efuse
#else
    uint8_t version;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM_VER, &version, 8);
    if (version != 1) {
        ESP_LOGE(TAG, "Base MAC address from BLK3 of EFUSE version error, version = %d", version);
        return ESP_ERR_INVALID_VERSION;
    }

    uint8_t efuse_crc;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM, mac, 48);
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM_CRC, &efuse_crc, 8);
    uint8_t calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
        ESP_LOGE(TAG, "Base MAC address from BLK3 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
        return ESP_ERR_INVALID_CRC;
    }
    return ESP_OK;
#endif // IDF_TARGET_ESP32S2BETA
}

esp_err_t esp_efuse_mac_get_default(uint8_t* mac)
{
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, mac, 48);
    if (err != ESP_OK) {
        return err;
    }
#ifdef CONFIG_IDF_TARGET_ESP32
// Only ESP32 has MAC CRC in efuse, ESP32-S2 has internal efuse consistency checks
    uint8_t efuse_crc;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY_CRC, &efuse_crc, 8);
    uint8_t calc_crc = esp_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
         // Small range of MAC addresses are accepted even if CRC is invalid.
         // These addresses are reserved for Espressif internal use.
        uint32_t mac_high = ((uint32_t)mac[0] << 8) | mac[1];
        if ((mac_high & 0xFFFF) == 0x18fe) {
            uint32_t mac_low = ((uint32_t)mac[2] << 24) | ((uint32_t)mac[3] << 16) | ((uint32_t)mac[4] << 8) | mac[5];
            if ((mac_low >= 0x346a85c7) && (mac_low <= 0x346a85f8)) {
                return ESP_OK;
            }
        } else {
            ESP_LOGE(TAG, "Base MAC address from BLK0 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            abort();
        }
    }
#endif // CONFIG_IDF_TARGET_ESP32
    return ESP_OK;
}

esp_err_t esp_derive_local_mac(uint8_t* local_mac, const uint8_t* universal_mac)
{
    uint8_t idx;

    if (local_mac == NULL || universal_mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(local_mac, universal_mac, 6);
    for (idx = 0; idx < 64; idx++) {
        local_mac[0] = universal_mac[0] | 0x02;
        local_mac[0] ^= idx << 2;

        if (memcmp(local_mac, universal_mac, 6)) {
            break;
        }
    }

    return ESP_OK;
}

esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type)
{
    uint8_t efuse_mac[6];

    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (type < ESP_MAC_WIFI_STA || type > ESP_MAC_ETH) {
        ESP_LOGE(TAG, "mac type is incorrect");
        return ESP_ERR_INVALID_ARG;
    }

    _Static_assert(UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR \
            || UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR, \
            "incorrect NUM_MAC_ADDRESS_FROM_EFUSE value");

    if (esp_base_mac_addr_get(efuse_mac) != ESP_OK) {
        esp_efuse_mac_get_default(efuse_mac);
    }

    switch (type) {
    case ESP_MAC_WIFI_STA:
        memcpy(mac, efuse_mac, 6);
        break;
    case ESP_MAC_WIFI_SOFTAP:
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            memcpy(mac, efuse_mac, 6);
            mac[5] += 1;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            esp_derive_local_mac(mac, efuse_mac);
        }
        break;
    case ESP_MAC_BT:
        memcpy(mac, efuse_mac, 6);
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            mac[5] += 2;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            mac[5] += 1;
        }
        break;
    case ESP_MAC_ETH:
        if (UNIVERSAL_MAC_ADDR_NUM == FOUR_UNIVERSAL_MAC_ADDR) {
            memcpy(mac, efuse_mac, 6);
            mac[5] += 3;
        }
        else if (UNIVERSAL_MAC_ADDR_NUM == TWO_UNIVERSAL_MAC_ADDR) {
            efuse_mac[5] += 1;
            esp_derive_local_mac(mac, efuse_mac);
        }
        break;
    default:
        ESP_LOGW(TAG, "incorrect mac type");
        break;
    }

    return ESP_OK;
}


