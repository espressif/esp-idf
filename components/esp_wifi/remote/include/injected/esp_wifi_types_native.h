/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_wifi_types_generic.h"
#if CONFIG_SLAVE_SOC_WIFI_HE_SUPPORT
#include "esp_wifi_he_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_SLAVE_IDF_TARGET_ESP32C2
#define ESP_WIFI_MAX_CONN_NUM  (4)        /**< max number of stations which can connect to ESP32C2 soft-AP */
#elif CONFIG_SLAVE_IDF_TARGET_ESP32C3 || CONFIG_SLAVE_IDF_TARGET_ESP32C6 || CONFIG_SLAVE_IDF_TARGET_ESP32C5 || CONFIG_SLAVE_IDF_TARGET_ESP32C61
#define ESP_WIFI_MAX_CONN_NUM  (10)       /**< max number of stations which can connect to ESP32C3/ESP32C6/ESP32C5/ESP32C61 soft-AP */
#else
#define ESP_WIFI_MAX_CONN_NUM  (15)       /**< max number of stations which can connect to ESP32/ESP32S3/ESP32S2 soft-AP */
#endif

/** @brief List of stations associated with the Soft-AP */
typedef struct wifi_sta_list_t {
    wifi_sta_info_t sta[ESP_WIFI_MAX_CONN_NUM]; /**< station list */
    int       num; /**< number of stations in the list (other entries are invalid) */
} wifi_sta_list_t;

#if CONFIG_SLAVE_SOC_WIFI_HE_SUPPORT
typedef esp_wifi_rxctrl_t wifi_pkt_rx_ctrl_t;
#else
/** @brief Received packet radio metadata header, this is the common header at the beginning of all promiscuous mode RX callback buffers */
typedef struct {
    signed rssi: 8;               /**< Received Signal Strength Indicator(RSSI) of packet. unit: dBm */
    unsigned rate: 5;             /**< PHY rate encoding of the packet. Only valid for non HT(11bg) packet */
    unsigned : 1;                 /**< reserved */
    unsigned sig_mode: 2;         /**< Protocol of the received packet, 0: non HT(11bg) packet; 1: HT(11n) packet; 3: VHT(11ac) packet */
    unsigned : 16;                /**< reserved */
    unsigned mcs: 7;              /**< Modulation Coding Scheme. If is HT(11n) packet, shows the modulation, range from 0 to 76(MSC0 ~ MCS76) */
    unsigned cwb: 1;              /**< Channel Bandwidth of the packet. 0: 20MHz; 1: 40MHz */
    unsigned : 16;                /**< reserved */
    unsigned smoothing: 1;         /**< Set to 1 indicates that channel estimate smoothing is recommended.
                                       Set to 0 indicates that only per-carrierindependent (unsmoothed) channel estimate is recommended. */
    unsigned not_sounding: 1;      /**< Set to 0 indicates that PPDU is a sounding PPDU. Set to 1indicates that the PPDU is not a sounding PPDU.
                                       sounding PPDU is used for channel estimation by the request receiver */
    unsigned : 1;                 /**< reserved */
    unsigned aggregation: 1;      /**< Aggregation. 0: MPDU packet; 1: AMPDU packet */
    unsigned stbc: 2;             /**< Space Time Block Code(STBC). 0: non STBC packet; 1: STBC packet */
    unsigned fec_coding: 1;       /**< Forward Error Correction(FEC). Flag is set for 11n packets which are LDPC */
    unsigned sgi: 1;              /**< Short Guide Interval(SGI). 0: Long GI; 1: Short GI */
#if CONFIG_SLAVE_IDF_TARGET_ESP32
    signed noise_floor: 8;        /**< noise floor of Radio Frequency Module(RF). unit: dBm*/
#elif CONFIG_SLAVE_IDF_TARGET_ESP32S2 || CONFIG_SLAVE_IDF_TARGET_ESP32S3 || CONFIG_SLAVE_IDF_TARGET_ESP32C3 || CONFIG_SLAVE_IDF_TARGET_ESP32C2
    unsigned : 8;                 /**< reserved */
#endif
    unsigned ampdu_cnt: 8;        /**< the number of subframes aggregated in AMPDU */
    unsigned channel: 4;          /**< primary channel on which this packet is received */
    unsigned secondary_channel: 4; /**< secondary channel on which this packet is received. 0: none; 1: above; 2: below */
    unsigned : 8;                 /**< reserved */
    unsigned timestamp: 32;       /**< timestamp. The local time when this packet is received. It is precise only if modem sleep or light sleep is not enabled. unit: microsecond */
    unsigned : 32;                /**< reserved */
#if CONFIG_SLAVE_IDF_TARGET_ESP32S2
    unsigned : 32;                /**< reserved */
#elif CONFIG_SLAVE_IDF_TARGET_ESP32S3 || CONFIG_SLAVE_IDF_TARGET_ESP32C3 || CONFIG_SLAVE_IDF_TARGET_ESP32C2
    signed noise_floor: 8;        /**< noise floor of Radio Frequency Module(RF). unit: dBm*/
    unsigned : 24;                /**< reserved */
    unsigned : 32;                /**< reserved */
#endif
    unsigned : 31;                /**< reserved */
    unsigned ant: 1;              /**< antenna number from which this packet is received. 0: WiFi antenna 0; 1: WiFi antenna 1 */
#if CONFIG_SLAVE_IDF_TARGET_ESP32S2
    signed noise_floor: 8;        /**< noise floor of Radio Frequency Module(RF). unit: dBm*/
    unsigned : 24;                /**< reserved */
#elif CONFIG_SLAVE_IDF_TARGET_ESP32S3 || CONFIG_SLAVE_IDF_TARGET_ESP32C3 || CONFIG_SLAVE_IDF_TARGET_ESP32C2
    unsigned : 32;                /**< reserved */
    unsigned : 32;                /**< reserved */
    unsigned : 32;                /**< reserved */
#endif
    unsigned sig_len: 12;         /**< length of packet including Frame Check Sequence(FCS) */
    unsigned : 12;                /**< reserved */
    unsigned rx_state: 8;         /**< state of the packet. 0: no error; others: error numbers which are not public */
} wifi_pkt_rx_ctrl_t;
#endif

/**
  * @brief Channel state information(CSI) configuration type
  *
  */
#if CONFIG_SLAVE_SOC_WIFI_HE_SUPPORT
typedef wifi_csi_acquire_config_t wifi_csi_config_t;
#else
typedef struct {
    bool lltf_en;           /**< enable to receive legacy long training field(lltf) data. Default enabled */
    bool htltf_en;          /**< enable to receive HT long training field(htltf) data. Default enabled */
    bool stbc_htltf2_en;    /**< enable to receive space time block code HT long training field(stbc-htltf2) data. Default enabled */
    bool ltf_merge_en;      /**< enable to generate htlft data by averaging lltf and ht_ltf data when receiving HT packet. Otherwise, use ht_ltf data directly. Default enabled */
    bool channel_filter_en; /**< enable to turn on channel filter to smooth adjacent sub-carrier. Disable it to keep independence of adjacent sub-carrier. Default enabled */
    bool manu_scale;        /**< manually scale the CSI data by left shifting or automatically scale the CSI data. If set true, please set the shift bits. false: automatically. true: manually. Default false */
    uint8_t shift;          /**< manually left shift bits of the scale of the CSI data. The range of the left shift bits is 0~15 */
    bool dump_ack_en;       /**< enable to dump 802.11 ACK frame, default disabled */
} wifi_csi_config_t;
#endif // !CONFIG_SLAVE_SOC_WIFI_HE_SUPPORT

/** @brief Payload passed to 'buf' parameter of promiscuous mode RX callback.
 */
typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl; /**< metadata header */
    uint8_t payload[0];       /**< Data or management payload. Length of payload is described by rx_ctrl.sig_len. Type of content determined by packet type argument of callback. */
} wifi_promiscuous_pkt_t;

/**
  * @brief CSI data type
  *
  */
typedef struct wifi_csi_info_t {
    wifi_pkt_rx_ctrl_t rx_ctrl;/**< received packet radio metadata header of the CSI data */
    uint8_t mac[6];            /**< source MAC address of the CSI data */
    uint8_t dmac[6];           /**< destination MAC address of the CSI data */
    bool first_word_invalid;   /**< first four bytes of the CSI data is invalid or not, true indicates the first four bytes is invalid due to hardware limitation */
    int8_t *buf;               /**< valid buffer of CSI data */
    uint16_t len;              /**< valid length of CSI data */
    uint8_t *hdr;              /**< header of the wifi packet */
    uint8_t *payload;          /**< payload of the wifi packet */
    uint16_t payload_len;      /**< payload len of the wifi packet */
    uint16_t rx_seq;           /**< rx sequence number of the wifi packet */
} wifi_csi_info_t;

#ifdef __cplusplus
}
#endif
