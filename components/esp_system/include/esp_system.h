// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_SYSTEM_H__
#define __ESP_SYSTEM_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_bit_defs.h"
#include "esp_idf_version.h"

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
 * @brief Reset reasons
 */
typedef enum {
    ESP_RST_UNKNOWN,    //!< Reset reason can not be determined
    ESP_RST_POWERON,    //!< Reset due to power-on event
    ESP_RST_EXT,        //!< Reset by external pin (not applicable for ESP32)
    ESP_RST_SW,         //!< Software reset via esp_restart
    ESP_RST_PANIC,      //!< Software reset due to exception/panic
    ESP_RST_INT_WDT,    //!< Reset (software or hardware) due to interrupt watchdog
    ESP_RST_TASK_WDT,   //!< Reset due to task watchdog
    ESP_RST_WDT,        //!< Reset due to other watchdogs
    ESP_RST_DEEPSLEEP,  //!< Reset after exiting deep sleep mode
    ESP_RST_BROWNOUT,   //!< Brownout reset (software or hardware)
    ESP_RST_SDIO,       //!< Reset over SDIO
} esp_reset_reason_t;

/**
 * Shutdown handler type
 */
typedef void (*shutdown_handler_t)(void);

/**
  * @brief  Register shutdown handler
  *
  * This function allows you to register a handler that gets invoked before
  * the application is restarted using esp_restart function.
  * @param handle function to execute on restart
  * @return
  *   - ESP_OK on success
  *   - ESP_ERR_INVALID_STATE if the handler has already been registered
  *   - ESP_ERR_NO_MEM if no more shutdown handler slots are available
  */
esp_err_t esp_register_shutdown_handler(shutdown_handler_t handle);

/**
  * @brief  Unregister shutdown handler
  *
  * This function allows you to unregister a handler which was previously
  * registered using esp_register_shutdown_handler function.
  *   - ESP_OK on success
  *   - ESP_ERR_INVALID_STATE if the given handler hasn't been registered before
  */
esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handle);


/**
  * @brief  Restart PRO and APP CPUs.
  *
  * This function can be called both from PRO and APP CPUs.
  * After successful restart, CPU reset reason will be SW_CPU_RESET.
  * Peripherals (except for WiFi, BT, UART0, SPI1, and legacy timers) are not reset.
  * This function does not return.
  */
void esp_restart(void) __attribute__ ((noreturn));

/**
 * @brief  Get reason of last reset
 * @return See description of esp_reset_reason_t for explanation of each value.
 */
esp_reset_reason_t esp_reset_reason(void);

/**
  * @brief  Get the size of available heap.
  *
  * Note that the returned value may be larger than the maximum contiguous block
  * which can be allocated.
  *
  * @return Available heap size, in bytes.
  */
uint32_t esp_get_free_heap_size(void);

/**
  * @brief  Get the size of available internal heap.
  *
  * Note that the returned value may be larger than the maximum contiguous block
  * which can be allocated.
  *
  * @return Available internal heap size, in bytes.
  */
uint32_t esp_get_free_internal_heap_size(void);

/**
  * @brief Get the minimum heap that has ever been available
  *
  * @return Minimum free heap ever available
  */
uint32_t esp_get_minimum_free_heap_size( void );

/**
 * @brief  Get one random 32-bit word from hardware RNG
 *
 * The hardware RNG is fully functional whenever an RF subsystem is running (ie Bluetooth or WiFi is enabled). For
 * random values, call this function after WiFi or Bluetooth are started.
 *
 * If the RF subsystem is not used by the program, the function bootloader_random_enable() can be called to enable an
 * entropy source. bootloader_random_disable() must be called before RF subsystem or I2S peripheral are used. See these functions'
 * documentation for more details.
 *
 * Any time the app is running without an RF subsystem (or bootloader_random) enabled, RNG hardware should be
 * considered a PRNG. A very small amount of entropy is available due to pre-seeding while the IDF
 * bootloader is running, but this should not be relied upon for any use.
 *
 * @return Random value between 0 and UINT32_MAX
 */
uint32_t esp_random(void);

/**
 * @brief Fill a buffer with random bytes from hardware RNG
 *
 * @note This function has the same restrictions regarding available entropy as esp_random()
 *
 * @param buf Pointer to buffer to fill with random numbers.
 * @param len Length of buffer in bytes
 */
void esp_fill_random(void *buf, size_t len);

/**
  * @brief  Set base MAC address with the MAC address which is stored in BLK3 of EFUSE or
  *         external storage e.g. flash and EEPROM.
  *
  * Base MAC address is used to generate the MAC addresses used by the networking interfaces.
  * If using base MAC address stored in BLK3 of EFUSE or external storage, call this API to set base MAC
  * address with the MAC address which is stored in BLK3 of EFUSE or external storage before initializing
  * WiFi/BT/Ethernet.
  *
  * @note Base MAC must be a unicast MAC (least significant bit of first byte must be zero).
  *
  * @note If not using a valid OUI, set the "locally administered" bit
  *       (bit value 0x02 in the first byte) to avoid collisions.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_ARG If mac is NULL or is not a unicast MAC
  */
esp_err_t esp_base_mac_addr_set(const uint8_t *mac);

/**
  * @brief  Return base MAC address which is set using esp_base_mac_addr_set.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_MAC base MAC address has not been set
  */
esp_err_t esp_base_mac_addr_get(uint8_t *mac);

/**
  * @brief  Return base MAC address which was previously written to BLK3 of EFUSE.
  *
  * Base MAC address is used to generate the MAC addresses used by the networking interfaces.
  * This API returns the custom base MAC address which was previously written to BLK3 of EFUSE.
  * Writing this EFUSE allows setting of a different (non-Espressif) base MAC address. It is also
  * possible to store a custom base MAC address elsewhere, see esp_base_mac_addr_set() for details.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  *         ESP_ERR_INVALID_VERSION An invalid MAC version field was read from BLK3 of EFUSE
  *         ESP_ERR_INVALID_CRC An invalid MAC CRC was read from BLK3 of EFUSE
  */
esp_err_t esp_efuse_mac_get_custom(uint8_t *mac);

/**
  * @brief  Return base MAC address which is factory-programmed by Espressif in BLK0 of EFUSE.
  *
  * @param  mac  base MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_efuse_mac_get_default(uint8_t *mac);

/**
  * @brief  Read base MAC address and set MAC address of the interface.
  *
  * This function first get base MAC address using esp_base_mac_addr_get or reads base MAC address
  * from BLK0 of EFUSE. Then set the MAC address of the interface including wifi station, wifi softap,
  * bluetooth and ethernet.
  *
  * @param  mac  MAC address of the interface, length: 6 bytes.
  * @param  type  type of MAC address, 0:wifi station, 1:wifi softap, 2:bluetooth, 3:ethernet.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type);

/**
  * @brief Derive local MAC address from universal MAC address.
  *
  * This function derives a local MAC address from an universal MAC address.
  * A `definition of local vs universal MAC address can be found on Wikipedia
  * <https://en.wikipedia.org/wiki/MAC_address#Universal_vs._local>`.
  * In ESP32, universal MAC address is generated from base MAC address in EFUSE or other external storage.
  * Local MAC address is derived from the universal MAC address.
  *
  * @param  local_mac  Derived local MAC address, length: 6 bytes.
  * @param  universal_mac  Source universal MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_derive_local_mac(uint8_t* local_mac, const uint8_t* universal_mac);

/**
 * @brief Trigger a software abort
 *
 * @param details Details that will be displayed during panic handling.
 */
void  __attribute__((noreturn)) esp_system_abort(const char* details);

/**
 * @brief Chip models
 */
typedef enum {
    CHIP_ESP32  = 1, //!< ESP32
    CHIP_ESP32S2 = 2, //!< ESP32-S2
    CHIP_ESP32S3 = 4, //!< ESP32-S3
    CHIP_ESP32C3 = 5, //!< ESP32-C3
} esp_chip_model_t;

/* Chip feature flags, used in esp_chip_info_t */
#define CHIP_FEATURE_EMB_FLASH      BIT(0)      //!< Chip has embedded flash memory
#define CHIP_FEATURE_WIFI_BGN       BIT(1)      //!< Chip has 2.4GHz WiFi
#define CHIP_FEATURE_BLE            BIT(4)      //!< Chip has Bluetooth LE
#define CHIP_FEATURE_BT             BIT(5)      //!< Chip has Bluetooth Classic

/**
 * @brief The structure represents information about the chip
 */
typedef struct {
    esp_chip_model_t model;  //!< chip model, one of esp_chip_model_t
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint8_t cores;           //!< number of CPU cores
    uint8_t revision;        //!< chip revision number
} esp_chip_info_t;

/**
 * @brief Fill an esp_chip_info_t structure with information about the chip
 * @param[out] out_info structure to be filled
 */
void esp_chip_info(esp_chip_info_t* out_info);


#if CONFIG_ESP32_ECO3_CACHE_LOCK_FIX
/**
 * @brief Cache lock bug exists or not
 *
 * @return
 *          - ture : bug exists
 *          - false : bug not exists
 */
bool soc_has_cache_lock_bug(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SYSTEM_H__ */
