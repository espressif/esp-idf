/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "sdkconfig.h"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_MAC_WIFI_STA,      /**< MAC for WiFi Station (6 bytes) */
    ESP_MAC_WIFI_SOFTAP,   /**< MAC for WiFi Soft-AP (6 bytes) */
    ESP_MAC_BT,            /**< MAC for Bluetooth (6 bytes) */
    ESP_MAC_ETH,           /**< MAC for Ethernet (6 bytes) */
    ESP_MAC_IEEE802154,    /**< if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC for IEEE802154 (8 bytes) */
    ESP_MAC_BASE,          /**< Base MAC for that used for other MAC types (6 bytes) */
    ESP_MAC_EFUSE_FACTORY, /**< MAC_FACTORY eFuse which was burned by Espressif in production (6 bytes) */
    ESP_MAC_EFUSE_CUSTOM,  /**< MAC_CUSTOM eFuse which was can be burned by customer (6 bytes) */
    ESP_MAC_EFUSE_EXT,     /**< if CONFIG_SOC_IEEE802154_SUPPORTED=y, MAC_EXT eFuse which is used as an extender for IEEE802154 MAC (2 bytes) */
} esp_mac_type_t;

/** @cond */
#define ONE_UNIVERSAL_MAC_ADDR 1
#define TWO_UNIVERSAL_MAC_ADDR 2
#define FOUR_UNIVERSAL_MAC_ADDR 4
#define UNIVERSAL_MAC_ADDR_NUM CONFIG_ESP_MAC_UNIVERSAL_MAC_ADDRESSES
/** @endcond */


/**
  * @brief  Set base MAC address with the MAC address which is stored in BLK3 of EFUSE or
  *         external storage e.g. flash and EEPROM.
  *
  * Base MAC address is used to generate the MAC addresses used by network interfaces.
  *
  * If using a custom base MAC address, call this API before initializing any network interfaces.
  * Refer to the ESP-IDF Programming Guide for details about how the Base MAC is used.
  *
  * @note Base MAC must be a unicast MAC (least significant bit of first byte must be zero).
  *
  * @note If not using a valid OUI, set the "locally administered" bit
  *       (bit value 0x02 in the first byte) to avoid collisions.
  *
  * @param  mac base MAC address, length: 6 bytes.
  *         length: 6 bytes for MAC-48
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_ARG If mac is NULL or is not a unicast MAC
  */
esp_err_t esp_base_mac_addr_set(const uint8_t *mac);

/**
  * @brief  Return base MAC address which is set using esp_base_mac_addr_set.
  *
  * @note If no custom Base MAC has been set, this returns the pre-programmed Espressif base MAC address.
  *
  * @param  mac base MAC address, length: 6 bytes.
  *         length: 6 bytes for MAC-48
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_ARG mac is NULL
  *         ESP_ERR_INVALID_MAC base MAC address has not been set
  */
esp_err_t esp_base_mac_addr_get(uint8_t *mac);

/**
  * @brief  Return base MAC address which was previously written to BLK3 of EFUSE.
  *
  * Base MAC address is used to generate the MAC addresses used by the networking interfaces.
  * This API returns the custom base MAC address which was previously written to EFUSE BLK3 in
  * a specified format.
  *
  * Writing this EFUSE allows setting of a different (non-Espressif) base MAC address. It is also
  * possible to store a custom base MAC address elsewhere, see esp_base_mac_addr_set() for details.
  *
  * @note This function is currently only supported on ESP32.
  *
  * @param  mac base MAC address, length: 6 bytes/8 bytes.
  *         length: 6 bytes for MAC-48
  *                 8 bytes for EUI-64(used for IEEE 802.15.4, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_ARG mac is NULL
  *         ESP_ERR_INVALID_MAC CUSTOM_MAC address has not been set, all zeros (for esp32-xx)
  *         ESP_ERR_INVALID_VERSION An invalid MAC version field was read from BLK3 of EFUSE (for esp32)
  *         ESP_ERR_INVALID_CRC An invalid MAC CRC was read from BLK3 of EFUSE (for esp32)
  */
esp_err_t esp_efuse_mac_get_custom(uint8_t *mac);

/**
  * @brief  Return base MAC address which is factory-programmed by Espressif in EFUSE.
  *
  * @param  mac base MAC address, length: 6 bytes/8 bytes.
  *         length: 6 bytes for MAC-48
  *                 8 bytes for EUI-64(used for IEEE 802.15.4, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_ARG mac is NULL
  */
esp_err_t esp_efuse_mac_get_default(uint8_t *mac);

/**
  * @brief  Read base MAC address and set MAC address of the interface.
  *
  * This function first get base MAC address using esp_base_mac_addr_get().
  * Then calculates the MAC address of the specific interface requested,
  * refer to ESP-IDF Programming Guide for the algorithm.
  *
  * The MAC address set by the esp_iface_mac_addr_set() function will not depend on the base MAC address.
  *
  * @param  mac base MAC address, length: 6 bytes/8 bytes.
  *         length: 6 bytes for MAC-48
  *                 8 bytes for EUI-64(used for IEEE 802.15.4, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
  * @param  type Type of MAC address to return
  *
  * @return ESP_OK on success
  */
esp_err_t esp_read_mac(uint8_t *mac, esp_mac_type_t type);

/**
  * @brief Derive local MAC address from universal MAC address.
  *
  * This function copies a universal MAC address and then sets the "locally
  * administered" bit (bit 0x2) in the first octet, creating a locally
  * administered MAC address.
  *
  * If the universal MAC address argument is already a locally administered MAC
  * address, then the first octet is XORed with 0x4 in order to create a different
  * locally administered MAC address.
  *
  * @param  local_mac base MAC address, length: 6 bytes.
  *         length: 6 bytes for MAC-48
  * @param  universal_mac  Source universal MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_derive_local_mac(uint8_t *local_mac, const uint8_t *universal_mac);

/**
  * @brief  Set custom MAC address of the interface. This function allows you to overwrite the MAC addresses
  *         of the interfaces set by the base MAC address.
  *
  * @param  mac  MAC address, length: 6 bytes/8 bytes.
  *         length: 6 bytes for MAC-48
  *                 8 bytes for EUI-64(used for ESP_MAC_IEEE802154 type, if CONFIG_SOC_IEEE802154_SUPPORTED=y)
  * @param  type  Type of MAC address
  *
  * @return ESP_OK on success
  */
esp_err_t esp_iface_mac_addr_set(const uint8_t *mac, esp_mac_type_t type);

/**
  * @brief  Return the size of the MAC type in bytes.
  *
  * If CONFIG_SOC_IEEE802154_SUPPORTED is set then for these types:
  *  - ESP_MAC_IEEE802154 is 8 bytes.
  *  - ESP_MAC_BASE, ESP_MAC_EFUSE_FACTORY and ESP_MAC_EFUSE_CUSTOM the MAC size is 6 bytes.
  *  - ESP_MAC_EFUSE_EXT is 2 bytes.
  * If CONFIG_SOC_IEEE802154_SUPPORTED is not set then for all types it returns 6 bytes.
  *
  * @param  type  Type of MAC address
  *
  * @return 0 MAC type not found (not supported)
  *         6 bytes for MAC-48.
  *         8 bytes for EUI-64.
  */
size_t esp_mac_addr_len_get(esp_mac_type_t type);

#ifdef __cplusplus
}
#endif
