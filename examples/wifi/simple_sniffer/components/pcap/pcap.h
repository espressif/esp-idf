// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "esp_err.h"

#define PCAP_MAGIC_BIG_ENDIAN 0xA1B2C3D4    /*!< Big-Endian */
#define PCAP_MAGIC_LITTLE_ENDIAN 0xD4C3B2A1 /*!< Little-Endian */

#define PCAP_VERSION_MAJOR 0x02 /*!< Major Version */
#define PCAP_VERSION_MINOR 0x04 /*!< Minor Version */

#define PCAP_TIME_ZONE_GMT 0x00 /*!< Time Zone */

/**
* @brief Link layer Type Definition, used for Pcap reader to decode payload
*
*/
typedef enum {
    PCAP_LINK_TYPE_LOOPBACK = 0,       /*!< Loopback devices, except for later OpenBSD */
    PCAP_LINK_TYPE_ETHERNET = 1,       /*!< Ethernet, and Linux loopback devices */
    PCAP_LINK_TYPE_TOKEN_RING = 6,     /*!< 802.5 Token Ring */
    PCAP_LINK_TYPE_ARCNET = 7,         /*!< ARCnet */
    PCAP_LINK_TYPE_SLIP = 8,           /*!< SLIP */
    PCAP_LINK_TYPE_PPP = 9,            /*!< PPP */
    PCAP_LINK_TYPE_FDDI = 10,          /*!< FDDI */
    PCAP_LINK_TYPE_ATM = 100,          /*!< LLC/SNAP encapsulated ATM */
    PCAP_LINK_TYPE_RAW_IP = 101,       /*!< Raw IP, without link */
    PCAP_LINK_TYPE_BSD_SLIP = 102,     /*!< BSD/OS SLIP */
    PCAP_LINK_TYPE_BSD_PPP = 103,      /*!< BSD/OS PPP */
    PCAP_LINK_TYPE_CISCO_HDLC = 104,   /*!< Cisco HDLC */
    PCAP_LINK_TYPE_802_11 = 105,       /*!< 802.11 */
    PCAP_LINK_TYPE_BSD_LOOPBACK = 108, /*!< OpenBSD loopback devices(with AF_value in network byte order) */
    PCAP_LINK_TYPE_LOCAL_TALK = 114    /*!< LocalTalk */
} pcap_link_type_t;

/**
* @brief Pcap configuration Type Definition
*
*/
typedef struct {
    FILE *fp;                   /* Pointer to a standard file handle */
    pcap_link_type_t link_type; /* Pcap Link Type */
} pcap_config_t;

/**
 * @brief Pcap Handle Type Definition
 *
 */
typedef void *pcap_handle_t;

/**
 * @brief Initialize a pcap session
 *
 * @param config configuration of creating pcap object
 * @param handle pcap handle
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t pcap_init(pcap_config_t *config, pcap_handle_t *handle);

/**
 * @brief De-initialize a pcap session
 *
 * @param handle pcap handle
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t pcap_deinit(pcap_handle_t handle);

/**
 * @brief Capture one packet into pcap file
 *
 * @param handle pcap handle
 * @param payload pointer of the captured data
 * @param length length of captured data
 * @param seconds second of capture time
 * @param microseconds microsecond of capture time
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t pcap_capture_packet(pcap_handle_t handle, void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds);

#ifdef __cplusplus
}
#endif
