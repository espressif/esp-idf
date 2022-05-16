/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdio.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PCAP_DEFAULT_VERSION_MAJOR 0x02 /*!< Major Version */
#define PCAP_DEFAULT_VERSION_MINOR 0x04 /*!< Minor Version */
#define PCAP_DEFAULT_TIME_ZONE_GMT 0x00 /*!< Time Zone */

/**
 * @brief Type of pcap file handle
 *
 */
typedef struct pcap_file_t *pcap_file_handle_t;

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
    FILE *fp;                   /*!< Pointer to a standard file handle */
    unsigned int major_version; /*!< Pcap version: major */
    unsigned int minor_version; /*!< Pcap version: minor */
    unsigned int time_zone;     /*!< Pcap timezone code */
    struct {
        unsigned int little_endian: 1; /*!< Whether the pcap file is recored in little endian format */
    } flags;
} pcap_config_t;

/**
 * @brief Create a new pcap session, and returns pcap file handle
 *
 * @note This function won't create the low level FILE* object, the user should take care of the creation of the File Stream.
 *
 * @param[in] config pcap file configuration
 * @param[out] ret_pcap Returned pcap file handle
 * @return
 *      - ESP_OK: Create pcap file successfully
 *      - ESP_ERR_INVALID_ARG: Create pcap file failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create pcap file failed because out of memory
 *      - ESP_FAIL: Create pcap file failed
 */
esp_err_t pcap_new_session(const pcap_config_t *config, pcap_file_handle_t *ret_pcap);

/**
 * @brief Delete the pcap session, and close the File Stream
 *
 * @param[in] pcap pcap file handle created by `pcap_new_session()`
 * @return
 *      - ESP_OK: Delete pcap session successfully
 *      - ESP_ERR_INVALID_ARG: Delete pcap session failed because of invalid argument
 *      - ESP_FAIL: Delete pcap session failed
 */
esp_err_t pcap_del_session(pcap_file_handle_t pcap);

/**
 * @brief Write pcap file header
 *
 * @param[in] pcap pcap file handle created by `pcap_new_session()`
 * @param[in] link_type Network link layer type
 * @return
 *      - ESP_OK: Write pcap file header successfully
 *      - ESP_ERR_INVALID_ARG: Write pcap file header failed because of invalid argument
 *      - ESP_FAIL: Write pcap file header failed
 */
esp_err_t pcap_write_header(pcap_file_handle_t pcap, pcap_link_type_t link_type);

/**
 * @brief Capture one packet into pcap file
 *
 * @param[in] pcap pcap file handle created by `pcap_new_session()`
 * @param[in] payload pointer of the captured data buffer
 * @param[in] length length of captured data buffer
 * @param[in] seconds second of capture time
 * @param[in] microseconds microsecond of capture time
 * @return
 *      - ESP_OK: Write network packet into pcap file successfully
 *      - ESP_ERR_INVALID_ARG: Write network packet into pcap file failed because of invalid argument
 *      - ESP_FAIL: Write network packet into pcap file failed
 */
esp_err_t pcap_capture_packet(pcap_file_handle_t pcap, void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds);

/**
 * @brief Print the summary of pcap file into stream
 *
 * @param[in] pcap pcap file handle created by `pcap_new_session()`
 * @param[in] print_file the file stream to save the summary
 * @return
 *      - ESP_OK: Print pcap file summary successfully
 *      - ESP_ERR_INVALID_ARG: Print pcap file summary failed because of invalid argument
 *      - ESP_FAIL: Print pcap file summary failed
 */
esp_err_t pcap_print_summary(pcap_file_handle_t pcap, FILE *print_file);

#ifdef __cplusplus
}
#endif
