/* pcap encoder.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define PCAP_MAGIC_BIG_ENDIAN 0xA1B2C3D4    /*!< Big-Endian */
#define PCAP_MAGIC_LITTLE_ENDIAN 0xD4C3B2A1 /*!< Little-Endian */

#define PCAP_VERSION_MAJOR 0x02 /*!< Major Version */
#define PCAP_VERSION_MINOR 0x04 /*!< Minor Version */

#define PCAP_TIME_ZONE_GMT 0x00 /*!< Time Zone */

#define PCAP_FILE_NAME_MAX_LEN 32 /*!< Max Name Length of Pcap File */

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
 * @brief Create a pcap object
 *
 * @param config configuration of creating pcap object
 * @return esp_err_t ESP_OK on success, ESP_FAIL on IO error
 */
esp_err_t pcap_new(pcap_config_t *config);

/**
 * @brief Close pcap file and recyle related resources
 *
 * @return esp_err_t ESP_OK on success, ESP_FAIL on error
 */
esp_err_t pcap_close(void);

/**
 * @brief Capture one packet into file in pcap format
 *
 * @param payload pointer to the captured data
 * @param length length of captured data
 * @param seconds second of capture time
 * @param microseconds microsecond of capture time
 * @return esp_err_t ESP_OK on success, ESP_FAIL on IO error
 */
esp_err_t pcap_capture_packet(void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds);

#ifdef __cplusplus
}
#endif
