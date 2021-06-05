// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_MAC_WIFI_STA,
    ESP_MAC_WIFI_SOFTAP,
    ESP_MAC_BT,
    ESP_MAC_ETH,
} esp_mac_type_t;

/** @cond */
#define TWO_UNIVERSAL_MAC_ADDR 2
#define FOUR_UNIVERSAL_MAC_ADDR 4
#if CONFIG_IDF_TARGET_ESP32
#define UNIVERSAL_MAC_ADDR_NUM CONFIG_ESP32_UNIVERSAL_MAC_ADDRESSES
#elif CONFIG_IDF_TARGET_ESP32S2
#define UNIVERSAL_MAC_ADDR_NUM CONFIG_ESP32S2_UNIVERSAL_MAC_ADDRESSES
#elif CONFIG_IDF_TARGET_ESP32S3
#define UNIVERSAL_MAC_ADDR_NUM CONFIG_ESP32S3_UNIVERSAL_MAC_ADDRESSES
#elif CONFIG_IDF_TARGET_ESP32C3
#define UNIVERSAL_MAC_ADDR_NUM CONFIG_ESP32C3_UNIVERSAL_MAC_ADDRESSES
#endif
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
  *
  * @return ESP_OK on success
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
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_VERSION An invalid MAC version field was read from BLK3 of EFUSE
  *         ESP_ERR_INVALID_CRC An invalid MAC CRC was read from BLK3 of EFUSE
  */
esp_err_t esp_efuse_mac_get_custom(uint8_t *mac);

/**
  * @brief  Return base MAC address which is factory-programmed by Espressif in EFUSE.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_efuse_mac_get_default(uint8_t *mac);

/**
  * @brief  Read base MAC address and set MAC address of the interface.
  *
  * This function first get base MAC address using esp_base_mac_addr_get().
  * Then calculates the MAC address of the specific interface requested,
  * refer to ESP-IDF Programming Guide for the algorithm.
  *
  * @param  mac  MAC address of the interface, length: 6 bytes.
  * @param  type Type of MAC address to return
  *
  * @return ESP_OK on success
  */
esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type);

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
  * @param  local_mac  Derived local MAC address, length: 6 bytes.
  * @param  universal_mac  Source universal MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_derive_local_mac(uint8_t* local_mac, const uint8_t* universal_mac);

#ifdef __cplusplus
}
#endif
