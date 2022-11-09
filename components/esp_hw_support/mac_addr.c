/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "sdkconfig.h"
#include "esp_rom_efuse.h"
#include "esp_mac.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"

/* esp_system.h APIs relating to MAC addresses */

#if CONFIG_ESP32_UNIVERSAL_MAC_ADDRESSES_FOUR   || \
    CONFIG_ESP32S3_UNIVERSAL_MAC_ADDRESSES_FOUR || \
    CONFIG_ESP32C2_UNIVERSAL_MAC_ADDRESSES_FOUR || \
    CONFIG_ESP32C3_UNIVERSAL_MAC_ADDRESSES_FOUR
#define MAC_ADDR_UNIVERSE_BT_OFFSET 2
#else
#define MAC_ADDR_UNIVERSE_BT_OFFSET 1
#endif

#if CONFIG_IEEE802154_ENABLED
#define ESP_MAC_ADDRESS_LEN 8
#else
#define ESP_MAC_ADDRESS_LEN 6
#endif
static const char *TAG = "system_api";

static uint8_t base_mac_addr[ESP_MAC_ADDRESS_LEN] = { 0 };

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

    memcpy(base_mac_addr, mac, ESP_MAC_ADDRESS_LEN);

    return ESP_OK;
}

esp_err_t esp_base_mac_addr_get(uint8_t *mac)
{
    if (mac == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (base_mac_addr[0] == 0 && memcmp(base_mac_addr, &base_mac_addr[1], ESP_MAC_ADDRESS_LEN - 1) == 0) {
        ESP_LOGI(TAG, "Base MAC address is not set");
        return ESP_ERR_INVALID_MAC;
    }

    memcpy(mac, base_mac_addr, ESP_MAC_ADDRESS_LEN);

    return ESP_OK;
}

esp_err_t esp_efuse_mac_get_custom(uint8_t *mac)
{
#if !CONFIG_IDF_TARGET_ESP32
    size_t size_bits = esp_efuse_get_field_size(ESP_EFUSE_USER_DATA_MAC_CUSTOM);
    assert((size_bits % 8) == 0);
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_USER_DATA_MAC_CUSTOM, mac, size_bits);
    if (err != ESP_OK) {
        return err;
    }
    size_t size = size_bits / 8;
    if (mac[0] == 0 && memcmp(mac, &mac[1], size - 1) == 0) {
        ESP_LOGE(TAG, "eFuse MAC_CUSTOM is empty");
        return ESP_ERR_INVALID_MAC;
    }
#if (ESP_MAC_ADDRESS_LEN == 8)
    err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_EXT, &mac[6], ESP_MAC_ADDRESS_LEN * 8 - size_bits);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Reading MAC_EXT failed, error=%d", err);
        return err;
    }
#endif
    return ESP_OK;
#else
    uint8_t version;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM_VER, &version, 8);
    if (version != 1) {
        // version 0 means has not been setup
        if (version == 0) {
            ESP_LOGD(TAG, "No base MAC address in eFuse (version=0)");
        } else if (version != 1) {
            ESP_LOGE(TAG, "Base MAC address version error, version = %d", version);
        }
        return ESP_ERR_INVALID_VERSION;
    }

    uint8_t efuse_crc;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM, mac, 48);
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_CUSTOM_CRC, &efuse_crc, 8);
    uint8_t calc_crc = esp_rom_efuse_mac_address_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
        ESP_LOGE(TAG, "Base MAC address from BLK3 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
        return ESP_ERR_INVALID_CRC;
    }
    return ESP_OK;
#endif
}

esp_err_t esp_efuse_mac_get_default(uint8_t *mac)
{
    size_t size_bits = esp_efuse_get_field_size(ESP_EFUSE_MAC_FACTORY);
    assert((size_bits % 8) == 0);
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, mac, size_bits);
    if (err != ESP_OK) {
        return err;
    }
#if (ESP_MAC_ADDRESS_LEN == 8)
    err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_EXT, &mac[6], ESP_MAC_ADDRESS_LEN * 8 - size_bits);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Reading MAC_EXT failed, error=%d", err);
        return err;
    }
#endif
#ifdef CONFIG_IDF_TARGET_ESP32
// Only ESP32 has MAC CRC in efuse
    uint8_t efuse_crc;
    esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY_CRC, &efuse_crc, 8);
    uint8_t calc_crc = esp_rom_efuse_mac_address_crc8(mac, 6);

    if (efuse_crc != calc_crc) {
        // Small range of MAC addresses are accepted even if CRC is invalid.
        // These addresses are reserved for Espressif internal use.
        uint32_t mac_high = ((uint32_t)mac[0] << 8) | mac[1];
        uint32_t mac_low = ((uint32_t)mac[2] << 24) | ((uint32_t)mac[3] << 16) | ((uint32_t)mac[4] << 8) | mac[5];
        if (((mac_high & 0xFFFF) == 0x18fe) && (mac_low >= 0x346a85c7) && (mac_low <= 0x346a85f8)) {
            return ESP_OK;
        } else {
            ESP_LOGE(TAG, "Base MAC address from BLK0 of EFUSE CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            abort();
        }
    }
#endif // CONFIG_IDF_TARGET_ESP32
    return ESP_OK;
}

esp_err_t esp_derive_local_mac(uint8_t *local_mac, const uint8_t *universal_mac)
{
    if (local_mac == NULL || universal_mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(local_mac, universal_mac, 6);

    const unsigned UL_BIT = 0x2;
    local_mac[0] |= UL_BIT;

    if (local_mac[0] == universal_mac[0]) {
        // universal_mac was already local, so flip this bit instead
        // (this is kept to be compatible with the previous behaviour of this function)
        local_mac[0] ^= 0x4;
    }

    return ESP_OK;
}

esp_err_t esp_read_mac(uint8_t *mac, esp_mac_type_t type)
{
    uint8_t efuse_mac[ESP_MAC_ADDRESS_LEN];

    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }
#if CONFIG_IEEE802154_ENABLED
    if (type < ESP_MAC_WIFI_STA || type > ESP_MAC_IEEE802154) {
#else
    if (type < ESP_MAC_WIFI_STA || type > ESP_MAC_ETH) {
#endif
        ESP_LOGE(TAG, "mac type is incorrect");
        return ESP_ERR_INVALID_ARG;
    }

    // if base mac address is not set, read one from EFUSE and then write back
    if (esp_base_mac_addr_get(efuse_mac) != ESP_OK) {
        ESP_LOGI(TAG, "read default base MAC address from EFUSE");
        esp_efuse_mac_get_default(efuse_mac);
        esp_base_mac_addr_set(efuse_mac);
    }

    switch (type) {
    case ESP_MAC_WIFI_STA:
        memcpy(mac, efuse_mac, 6);
        break;
    case ESP_MAC_WIFI_SOFTAP:
#if CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP
        memcpy(mac, efuse_mac, 6);
        // as a result of some esp32s2 chips burned with one MAC address by mistake,
        // there are some MAC address are reserved for this bug fix.
        // related mistake MAC address is 0x7cdfa1003000~0x7cdfa1005fff,
        // reserved MAC address is 0x7cdfa1020000~0x7cdfa1022fff (MAC address + 0x1d000).
#ifdef CONFIG_IDF_TARGET_ESP32S2
        uint8_t mac_begin[6] = { 0x7c, 0xdf, 0xa1, 0x00, 0x30, 0x00 };
        uint8_t mac_end[6]   = { 0x7c, 0xdf, 0xa1, 0x00, 0x5f, 0xff };
        if (memcmp(mac, mac_begin, 6) >= 0 && memcmp(mac_end, mac, 6) >= 0 ) {
            mac[3] += 0x02; // contain carry bit
            mac[4] += 0xd0;
        } else {
            mac[5] += 1;
        }
#else
        mac[5] += 1;
#endif // IDF_TARGET_ESP32S2
#else
        esp_derive_local_mac(mac, efuse_mac);
#endif // CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP
        break;
    case ESP_MAC_BT:
#if CONFIG_ESP_MAC_ADDR_UNIVERSE_BT
        memcpy(mac, efuse_mac, 6);
#if !CONFIG_IDF_TARGET_ESP32H4
        // esp32h4 chips do not have wifi module, so the mac address do not need to add the BT offset
        mac[5] += MAC_ADDR_UNIVERSE_BT_OFFSET;
#endif //!CONFIG_IDF_TARGET_ESP32H4
#else
        return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_ESP_MAC_ADDR_UNIVERSE_BT
        break;
    case ESP_MAC_ETH:
#if CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH
        memcpy(mac, efuse_mac, 6);
        mac[5] += 3;
#else
        efuse_mac[5] += 1;
        esp_derive_local_mac(mac, efuse_mac);
#endif // CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH
        break;
#if CONFIG_IEEE802154_ENABLED
    case ESP_MAC_IEEE802154:
        memcpy(mac, efuse_mac, 8);
        break;
#endif
    default:
        ESP_LOGE(TAG, "unsupported mac type");
        break;
    }

    return ESP_OK;
}
