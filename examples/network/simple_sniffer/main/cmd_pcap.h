/* cmd_pcap example — declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#include "pcap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Capture a pcap package with parameters
 *
 * @param payload pointer of the captured data
 * @param length length of captured data
 * @param seconds second of capture time
 * @param microseconds microsecond of capture time
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t packet_capture(void *payload, uint32_t length, uint32_t seconds, uint32_t microseconds);

/**
 * @brief Tell the pcap component to start sniff and write
 *
 * @param link_type link type of the captured package
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t sniff_packet_start(pcap_link_type_t link_type);

/**
 * @brief Tell the pcap component to stop sniff
 *
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
esp_err_t sniff_packet_stop(void);

/**
 * @brief Register pcap command
 *
 */
void register_pcap_cmd(void);

#ifdef __cplusplus
}
#endif
