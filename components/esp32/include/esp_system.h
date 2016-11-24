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

#include "esp_err.h"
#include "esp_deepsleep.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup System_APIs System APIs
  * @brief System APIs
  */

/** @addtogroup System_APIs
  * @{
  */

/**
  * @attention  application don't need to call this function anymore. It do nothing and will
  *             be removed in future version.
  */
void system_init(void) __attribute__ ((deprecated));


/**
  * @brief  Get information of the SDK version.
  *
  * @param  null
  *
  * @return Information of the SDK version.
  */
const char *system_get_sdk_version(void);

/**
  * @brief  Reset to default settings.
  *
  *         Reset to default settings of the following APIs : wifi_station_set_auto_connect,
  *         wifi_set_phy_mode, wifi_softap_set_config related, wifi_station_set_config
  *         related, and wifi_set_opmode.
  *
  * @param  null
  *
  * @return null
  */
void system_restore(void);

/**
  * @brief  Restart system.
  *
  * @param  null
  *
  * @return null
  */
void system_restart(void);

/**
  * @brief  Get system time, unit: microsecond.
  *
  * @param  null
  *
  * @return System time, unit: microsecond.
  */
uint32_t system_get_time(void);

/**
  * @brief  Get the size of available heap.
  *
  * @param  null
  *
  * @return Available heap size.
  */
uint32_t system_get_free_heap_size(void);

/**
  * @brief     Get RTC time, unit: RTC clock cycle.
  *
  * @param     null
  *
  * @return    RTC time.
  */
uint64_t system_get_rtc_time(void);

/**
  * @brief     Read user data from the RTC memory.
  *
  *            The user data segment (1024 bytes, as shown below) is used to store user data.
  *
  *             |<---- system data(512 bytes) ---->|<----------- user data(1024 bytes) --------->|
  *
  * @attention Read and write unit for data stored in the RTC memory is 4 bytes.
  * @attention src_addr is the block number (4 bytes per block). So when reading data
  *            at the beginning of the user data segment, src_addr will be 512/4 = 128,
  *            n will be data length.
  *
  * @param     uint16 src : source address of rtc memory, src_addr >= 128
  * @param     void *dst : data pointer
  * @param     uint16 n : data length, unit: byte
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_rtc_mem_read(uint16_t src, void *dst, uint16_t n);

/**
  * @brief     Write user data to  the RTC memory.
  *
  *            During deep-sleep, only RTC is working. So users can store their data
  *            in RTC memory if it is needed. The user data segment below (1024 bytes)
  *            is used to store the user data.
  *
  *            |<---- system data(512 bytes) ---->|<----------- user data(1024 bytes) --------->|
  *
  * @attention Read and write unit for data stored in the RTC memory is 4 bytes.
  * @attention src_addr is the block number (4 bytes per block). So when storing data
  *            at the beginning of the user data segment, src_addr will be 512/4 = 128,
  *            n will be data length.
  *
  * @param     uint16 src : source address of rtc memory, src_addr >= 128
  * @param     void *dst : data pointer
  * @param     uint16 n : data length, unit: byte
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool system_rtc_mem_write(uint16_t dst, const void *src, uint16_t n);

/** \defgroup System_boot_APIs Boot APIs
  * @brief boot APIs
  */

/** @addtogroup System_boot_APIs
  * @{
  */

/**
  * @}
  */
  
/** \defgroup Hardware_MAC_APIs Hardware MAC APIs
  * @brief Hardware MAC address APIs
  *
  * In WiFi MAC, only ESP32 station MAC is the hardware MAC, ESP32 softAP MAC is a software MAC 
  * calculated from ESP32 station MAC. 
  * So users need to call wifi_get_macaddr to query the ESP32 softAP MAC if ESP32 station MAC changed.
  *
  */

/** @addtogroup Hardware_MAC_APIs
  * @{
  */

/**
  * @brief  Read hardware MAC address.
  *
  * @param  uint8 mac[6] : the hardware MAC address, length: 6 bytes.
  *
  * @return esp_err_t
  */
esp_err_t system_efuse_read_mac(uint8_t mac[6]);


/**
  * @}
  */


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SYSTEM_H__ */
