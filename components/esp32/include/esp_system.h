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
#include "esp_deep_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @attention  application don't need to call this function anymore. It do nothing and will
  *             be removed in future version.
  */
void system_init(void) __attribute__ ((deprecated));

/**
  * @brief  Reset to default settings.
  *
  * Function has been deprecated, please use esp_wifi_restore instead.
  * This name will be removed in a future release.
  */
void system_restore(void) __attribute__ ((deprecated));

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
  * @brief  Restart system.
  *
  * Function has been renamed to esp_restart.
  * This name will be removed in a future release.
  */
void system_restart(void) __attribute__ ((deprecated, noreturn));

/**
  * @brief  Get system time, unit: microsecond.
  *
  * This function is deprecated. Use 'gettimeofday' function for 64-bit precision.
  * This definition will be removed in a future release.
  */
uint32_t system_get_time(void)  __attribute__ ((deprecated));

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
  * @brief  Get the size of available heap.
  *
  * Function has been renamed to esp_get_free_heap_size.
  * This name will be removed in a future release.
  *
  * @return Available heap size, in bytes.
  */
uint32_t system_get_free_heap_size(void)  __attribute__ ((deprecated));

/**
 * @brief  Get one random 32-bit word from hardware RNG
 *
 * @return random value between 0 and UINT32_MAX
 */
uint32_t esp_random(void);

/**
  * @brief  Read hardware MAC address.
  *
  * In WiFi MAC, only ESP32 station MAC is the hardware MAC, ESP32 softAP MAC is a software MAC 
  * calculated from ESP32 station MAC. 
  * So users need to call esp_wifi_get_macaddr to query the ESP32 softAP MAC if ESP32 station MAC changed.
  *
  * @param  mac  hardware MAC address, length: 6 bytes.
  *
  * @return ESP_OK on success
  */
esp_err_t esp_efuse_read_mac(uint8_t* mac);

/**
  * @brief  Read hardware MAC address.
  *
  * Function has been renamed to esp_efuse_read_mac.
  * This name will be removed in a future release.
  *
  * @param  mac  hardware MAC address, length: 6 bytes.
  * @return ESP_OK on success
  */
esp_err_t system_efuse_read_mac(uint8_t mac[6]) __attribute__ ((deprecated));

/**
 * Get SDK version
 *
 * This function is deprecated and will be removed in a future release.
 *
 * @return constant string "master"
 */
const char* system_get_sdk_version(void)  __attribute__ ((deprecated));

/**
 * Get IDF version
 *
 * @return constant string from IDF_VER
 */
const char* esp_get_idf_version(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SYSTEM_H__ */
