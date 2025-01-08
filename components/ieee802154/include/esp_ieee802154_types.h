/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#define US_PER_SYMBLE 16

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The radio state types.
 */
typedef enum {
    ESP_IEEE802154_RADIO_DISABLE,   /*!< Radio not up */
    ESP_IEEE802154_RADIO_IDLE,      /*!< Radio in the idle state */
    ESP_IEEE802154_RADIO_SLEEP,     /*!< Radio in the sleep state */
    ESP_IEEE802154_RADIO_RECEIVE,   /*!< Radio in the receive state */
    ESP_IEEE802154_RADIO_TRANSMIT,  /*!< Radio in the transmit state */
} esp_ieee802154_state_t;

/**
 * @brief The transmit error types.
 */
typedef enum {
    ESP_IEEE802154_TX_ERR_NONE,         /*!< No transmit error */
    ESP_IEEE802154_TX_ERR_CCA_BUSY,     /*!< Channel is busy */
    ESP_IEEE802154_TX_ERR_ABORT,        /*!< Transmit abort */
    ESP_IEEE802154_TX_ERR_NO_ACK,       /*!< No Ack frame received until timeout */
    ESP_IEEE802154_TX_ERR_INVALID_ACK,  /*!< Invalid Ack frame */
    ESP_IEEE802154_TX_ERR_COEXIST,      /*!< Rejected by coexist system */
    ESP_IEEE802154_TX_ERR_SECURITY,     /*!< Invalid security configuration */
} esp_ieee802154_tx_error_t;

/**
 * @brief The CCA mode types.
 */
typedef enum {
    ESP_IEEE802154_CCA_MODE_CARRIER,         /*!< Carrier only */
    ESP_IEEE802154_CCA_MODE_ED,              /*!< Energy Detect only */
    ESP_IEEE802154_CCA_MODE_CARRIER_OR_ED,   /*!< Carrier or Energy Detect */
    ESP_IEEE802154_CCA_MODE_CARRIER_AND_ED,  /*!< Carrier and Energy Detect */
} esp_ieee802154_cca_mode_t;

/**
 * @brief The frame pending mode types.
 */
typedef enum {
    ESP_IEEE802154_AUTO_PENDING_DISABLE,   /*!< Frame pending bit always set to 1 in the ack to Data Request */
    ESP_IEEE802154_AUTO_PENDING_ENABLE,    /*!< Frame pending bit set to 1 if src address matches, in the ack to Data Request */
    ESP_IEEE802154_AUTO_PENDING_ENHANCED,  /*!< Frame pending bit set to 1 if src address matches, in all ack frames */
    ESP_IEEE802154_AUTO_PENDING_ZIGBEE,    /*!< Frame pending bit set to 0 only if src address is short address and matches in table, in the ack to Data Request */
} esp_ieee802154_pending_mode_t;

/**
* @brief The four groups of mac filter interface index.
*/
typedef enum {
    ESP_IEEE802154_MULTIPAN_0 = 0,
    ESP_IEEE802154_MULTIPAN_1 = 1,
    ESP_IEEE802154_MULTIPAN_2 = 2,
    ESP_IEEE802154_MULTIPAN_3 = 3,
    ESP_IEEE802154_MULTIPAN_MAX
} esp_ieee802154_multipan_index_t;

/**
 * @brief The information of received 15.4 frame.
 *
 */
typedef struct {
    bool pending;                /*!< The frame was acked with frame pending set */
    bool process;                /*!< The frame needs to be processed by the upper layer */
    uint8_t channel;             /*!< Channel */
    int8_t rssi;                 /*!< RSSI */
    uint8_t lqi;                 /*!< LQI */
    uint64_t timestamp;          /*!< The timestamp when the frame's SFD field was received */
} esp_ieee802154_frame_info_t;

/**
 * @brief The O-QPSK PHY transmission power table for 2.4 GHz channels as defined by the IEEE 802.15.4 specification.
 *
 */
typedef struct {
    int8_t channel[16];         /*!< Index 0 -> Channel 11, Index 1 -> Channel 12 ... Index 15 -> Channel 26 */
} esp_ieee802154_txpower_table_t;

// These callbacks will be processed in ISR context, please locate them in IRAM, see `IRAM_ATTR`.
/**
 * @brief The callback to process Rx Done event, refer to `esp_ieee802154_receive_done`.
 *
 */
typedef void (*esp_ieee802154_receive_done_cb_t)(uint8_t *data, esp_ieee802154_frame_info_t *frame_info);

/**
 * @brief The callback to process Rx SFD Done event, refer to `esp_ieee802154_receive_sfd_done`.
 *
 */
typedef void (*esp_ieee802154_receive_sfd_done_cb_t)(void);

/**
 * @brief The callback to process Tx Done event, refer to `esp_ieee802154_transmit_done`.
 *
 */
typedef void (*esp_ieee802154_transmit_done_cb_t)(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info);

/**
 * @brief The callback to process Tx Failed event, refer to `esp_ieee802154_transmit_failed`.
 *
 */
typedef void (*esp_ieee802154_transmit_failed_cb_t)(const uint8_t *frame, esp_ieee802154_tx_error_t error);

/**
 * @brief The callback to process Tx SFD Done event, refer to `esp_ieee802154_transmit_sfd_done`.
 *
 */
typedef void (*esp_ieee802154_transmit_sfd_done_cb_t)(uint8_t *frame);

/**
 * @brief The callback to process ED Done event, refer to `esp_ieee802154_energy_detect_done`.
 *
 */
typedef void (*esp_ieee802154_energy_detect_done_cb_t)(int8_t power);

/**
 * @brief The callback to process enh-ack generating, refer to `esp_ieee802154_enh_ack_generator`.
 *
 */
typedef esp_err_t (*esp_ieee802154_enh_ack_generator_cb_t)(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info, uint8_t* enhack_frame);

/**
 * @brief The callback list for event process.
 *
 * @note These callbacks might be called in ISR context.
 *
 */
typedef struct {
    esp_ieee802154_receive_done_cb_t rx_done_cb;                   /*!<The callback to process Rx Done event */
    esp_ieee802154_receive_sfd_done_cb_t rx_sfd_done_cb;           /*!<The callback to process Rx SFD Done event */
    esp_ieee802154_transmit_done_cb_t tx_done_cb;                  /*!<The callback to process Tx Done event */
    esp_ieee802154_transmit_failed_cb_t tx_failed_cb;              /*!<The callback to process Tx Failed event */
    esp_ieee802154_transmit_sfd_done_cb_t tx_sfd_done_cb;          /*!<The callback to process Tx SFD Done event */
    esp_ieee802154_energy_detect_done_cb_t ed_done_cb;             /*!<The callback to process ED Done event */
    esp_ieee802154_enh_ack_generator_cb_t enh_ack_generator_cb;    /*!<The callback to process enh-ack generating */
} esp_ieee802154_event_cb_list_t;

#ifdef __cplusplus
}
#endif
