/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "esp_rom_efuse.h"
#include "esp_mac.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"

/* esp_system.h APIs relating to MAC addresses */

#if CONFIG_ESP_MAC_UNIVERSAL_MAC_ADDRESSES_FOUR
#define MAC_ADDR_UNIVERSE_BT_OFFSET 2
#else
#define MAC_ADDR_UNIVERSE_BT_OFFSET 1
#endif

#if SOC_IEEE802154_SUPPORTED
#define ESP_MAC_ADDRESS_LEN 8
#else
#define ESP_MAC_ADDRESS_LEN 6
#endif

static const char *TAG = "system_api";

typedef enum {
    STATE_INIT      = 0,
    STATE_SET       = (1 << 0),
} state_t;

typedef struct {
    esp_mac_type_t type: 4;
    state_t state: 4;
    uint8_t len;
    uint8_t mac[ESP_MAC_ADDRESS_LEN];
} mac_t;

static mac_t s_mac_table[] = {
#if SOC_WIFI_SUPPORTED
    {ESP_MAC_WIFI_STA,      STATE_INIT, 6, {0}},
    {ESP_MAC_WIFI_SOFTAP,   STATE_INIT, 6, {0}},
#endif

#ifdef CONFIG_ESP_MAC_ADDR_UNIVERSE_BT
    {ESP_MAC_BT,            STATE_INIT, 6, {0}},
#endif

    {ESP_MAC_ETH,           STATE_INIT, 6, {0}},

#ifdef SOC_IEEE802154_SUPPORTED
    {ESP_MAC_IEEE802154,    STATE_INIT, ESP_MAC_ADDRESS_LEN, {0}},
    {ESP_MAC_EFUSE_EXT,     STATE_INIT, 2, {0}},
#endif

    {ESP_MAC_BASE,          STATE_INIT, 6, {0}},
    {ESP_MAC_EFUSE_FACTORY, STATE_INIT, 6, {0}},
    {ESP_MAC_EFUSE_CUSTOM,  STATE_INIT, 6, {0}},
};

#define ITEMS_IN_MAC_TABLE (sizeof(s_mac_table) / sizeof(mac_t))

static esp_err_t generate_mac(uint8_t *mac, uint8_t *base_mac_addr, esp_mac_type_t type);
static esp_err_t get_efuse_factory_mac(uint8_t *mac);
static esp_err_t get_efuse_mac_custom(uint8_t *mac);
#if SOC_IEEE802154_SUPPORTED
static esp_err_t get_efuse_mac_ext(uint8_t *mac);
#endif

static int get_idx(esp_mac_type_t type)
{
    for (int idx = 0; idx < ITEMS_IN_MAC_TABLE; idx++) {
        if (s_mac_table[idx].type == type) {
            return idx;
        }
    }
    ESP_LOGE(TAG, "%d mac type is incorrect (not found)", type);
    return -1;
}

static esp_err_t get_mac_addr_from_mac_table(uint8_t *mac, int idx, bool silent)
{
    if (idx == -1) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (!(s_mac_table[idx].state & STATE_SET)) {
        esp_mac_type_t type = s_mac_table[idx].type;
        if (ESP_MAC_BASE <= type && type <= ESP_MAC_EFUSE_EXT) {
            esp_err_t err = ESP_OK;
            if (type == ESP_MAC_EFUSE_FACTORY
#ifndef CONFIG_ESP_MAC_USE_CUSTOM_MAC_AS_BASE_MAC
                    || type == ESP_MAC_BASE
#endif
             ) {
                err = get_efuse_factory_mac(s_mac_table[idx].mac);
             } else if (type == ESP_MAC_EFUSE_CUSTOM
#ifdef CONFIG_ESP_MAC_USE_CUSTOM_MAC_AS_BASE_MAC
                            || type == ESP_MAC_BASE
#endif
             ) {
                 err = get_efuse_mac_custom(s_mac_table[idx].mac);
             }
#if SOC_IEEE802154_SUPPORTED
            else if (type == ESP_MAC_EFUSE_EXT) {
                err = get_efuse_mac_ext(s_mac_table[idx].mac);
            }
#endif
            if (err != ESP_OK) {
                return err;
            }
            s_mac_table[idx].state = STATE_SET;
        } else {
            if (!silent) {
                ESP_LOGE(TAG, "MAC address (type %d) is not set in mac table", type);
            }
            return ESP_ERR_INVALID_MAC;
        }
    }
    memcpy(mac, s_mac_table[idx].mac, s_mac_table[idx].len);
    return ESP_OK;
}

size_t esp_mac_addr_len_get(esp_mac_type_t type)
{
    for (int idx = 0; idx < ITEMS_IN_MAC_TABLE; idx++) {
        if (s_mac_table[idx].type == type) {
            return s_mac_table[idx].len;
        }
    }
    return 0;
}

esp_err_t esp_iface_mac_addr_set(const uint8_t *mac, esp_mac_type_t type)
{
    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    int idx = get_idx(type);
    if (idx == -1) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (type == ESP_MAC_EFUSE_FACTORY || type == ESP_MAC_EFUSE_CUSTOM) {
        ESP_LOGE(TAG, "EFUSE MAC can not be set using this API");
        return ESP_ERR_INVALID_ARG;
    }

    if (type == ESP_MAC_BASE) {
        if (mac[0] & 0x01) {
            ESP_LOGE(TAG, "Base MAC must be a unicast MAC");
            return ESP_ERR_INVALID_ARG;
        }
    }

    memcpy(s_mac_table[idx].mac, mac, s_mac_table[idx].len);
    s_mac_table[idx].state = STATE_SET;
    return ESP_OK;
}

esp_err_t esp_base_mac_addr_set(const uint8_t *mac)
{
    return esp_iface_mac_addr_set(mac, ESP_MAC_BASE);
}

esp_err_t esp_base_mac_addr_get(uint8_t *mac)
{
    return esp_read_mac(mac, ESP_MAC_BASE);
}

#if SOC_IEEE802154_SUPPORTED
static esp_err_t get_efuse_mac_ext(uint8_t *mac)
{
    // ff:fe
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_EXT, mac, 16);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Reading MAC_EXT failed, error=%d", err);
        return err;
    }
    return ESP_OK;
}

static esp_err_t insert_mac_ext_into_mac(uint8_t *mac)
{
    uint8_t mac_tmp[3];
    memcpy(mac_tmp, &mac[3], 3);
    esp_err_t err = get_efuse_mac_ext(&mac[3]);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(&mac[5], mac_tmp, 3);
    return err;
}
#endif

esp_err_t esp_efuse_mac_get_custom(uint8_t *mac)
{
    esp_err_t err = get_efuse_mac_custom(mac);
    if (err != ESP_OK) {
        return err;
    }
#if SOC_IEEE802154_SUPPORTED
    return insert_mac_ext_into_mac(mac);
#else
    return ESP_OK;
#endif
}

static esp_err_t get_efuse_mac_custom(uint8_t *mac)
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
#ifdef CONFIG_ESP_MAC_IGNORE_MAC_CRC_ERROR
        ESP_LOGW(TAG, "Ignore MAC CRC error");
#else
        return ESP_ERR_INVALID_CRC;
#endif
    }
#endif
    return ESP_OK;
}

esp_err_t esp_efuse_mac_get_default(uint8_t *mac)
{
    esp_err_t err = get_efuse_factory_mac(mac);
    if (err != ESP_OK) {
        return err;
    }
#if SOC_IEEE802154_SUPPORTED
    return insert_mac_ext_into_mac(mac);
#else
    return ESP_OK;
#endif
}

static esp_err_t get_efuse_factory_mac(uint8_t *mac)
{
    size_t size_bits = esp_efuse_get_field_size(ESP_EFUSE_MAC_FACTORY);
    assert((size_bits % 8) == 0);
    esp_err_t err = esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, mac, size_bits);
    if (err != ESP_OK) {
        return err;
    }
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
#ifdef CONFIG_ESP_MAC_IGNORE_MAC_CRC_ERROR
            ESP_LOGW(TAG, "Ignore MAC CRC error");
#else
            return ESP_ERR_INVALID_CRC;
#endif
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
    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    int idx = get_idx(type);
    if (idx == -1) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    if (get_mac_addr_from_mac_table(mac, idx, true) == ESP_OK) {
        return ESP_OK;
    }
    // A MAC with a specific type has not yet been set (or generated)

    // then go ahead and generate it based on the base mac
    uint8_t base_mac_addr[ESP_MAC_ADDRESS_LEN];
    esp_err_t err = get_mac_addr_from_mac_table(base_mac_addr, get_idx(ESP_MAC_BASE), false);
    if (err) {
        ESP_LOGE(TAG, "Error reading BASE MAC address");
        return ESP_FAIL;
    }

    err = generate_mac(mac, base_mac_addr, type);
    if (err) {
        ESP_LOGE(TAG, "MAC address generation error");
        return err;
    }
    // MAC was generated. We write it into the s_mac_table
    s_mac_table[idx].state = STATE_SET;
    memcpy(s_mac_table[idx].mac, mac, s_mac_table[idx].len);
    return err;
}

static esp_err_t generate_mac(uint8_t *mac, uint8_t *base_mac_addr, esp_mac_type_t type)
{
    switch (type) {
#if SOC_WIFI_SUPPORTED
    case ESP_MAC_WIFI_STA:
        memcpy(mac, base_mac_addr, 6);
        break;
    case ESP_MAC_WIFI_SOFTAP:
        #if CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP
            memcpy(mac, base_mac_addr, 6);
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
            esp_derive_local_mac(mac, base_mac_addr);
        #endif // CONFIG_ESP_MAC_ADDR_UNIVERSE_WIFI_AP
        break;
#endif // SOC_WIFI_SUPPORTED

#if CONFIG_ESP_MAC_ADDR_UNIVERSE_BT
    case ESP_MAC_BT:
        memcpy(mac, base_mac_addr, 6);
        #if SOC_WIFI_SUPPORTED
            // If the chips do not have wifi module, the mac address do not need to add the BT offset
            mac[5] += MAC_ADDR_UNIVERSE_BT_OFFSET;
        #endif // SOC_WIFI_SUPPORTED
        break;
#endif // CONFIG_ESP_MAC_ADDR_UNIVERSE_BT

    case ESP_MAC_ETH:
#if CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH
        memcpy(mac, base_mac_addr, 6);
        #if SOC_WIFI_SUPPORTED || CONFIG_ESP_MAC_ADDR_UNIVERSE_BT
            mac[5] += 3;
        #endif // SOC_WIFI_SUPPORTED || CONFIG_ESP_MAC_ADDR_UNIVERSE_BT
#else
        base_mac_addr[5] += 1;
        esp_derive_local_mac(mac, base_mac_addr);
#endif // CONFIG_ESP_MAC_ADDR_UNIVERSE_ETH
        break;

#if SOC_IEEE802154_SUPPORTED
    case ESP_MAC_IEEE802154:
        // 60:55:f9
        memcpy(mac, base_mac_addr, 3);
        // 60:55:f9 + ff:fe
        esp_read_mac(&mac[3], ESP_MAC_EFUSE_EXT);
        // 60:55:f9:ff:fe + f7:2c:a2
        memcpy(&mac[5], &base_mac_addr[3], 3);
        // 60:55:f9:ff:fe:f7:2c:a2
        break;
#endif // SOC_IEEE802154_SUPPORTED

    default:
        ESP_LOGE(TAG, "unsupported mac type");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}
