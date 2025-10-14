/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __COEXIST_I154_H__
#define __COEXIST_I154_H__

/**
 * @brief 802.15.4 coex event
 */
typedef enum {
    IEEE802154_HIGH = 1,
    IEEE802154_MIDDLE,
    IEEE802154_LOW,
    IEEE802154_IDLE,
    IEEE802154_EVENT_MAX,
} ieee802154_coex_event_t;

/**
 * @brief 802.15.4 coexistence configurations
 */
typedef struct {
    ieee802154_coex_event_t idle;
    ieee802154_coex_event_t txrx;
    ieee802154_coex_event_t txrx_at;
} esp_ieee802154_coex_config_t;

/**
 * @brief Set 802.15.4 tx/rx pti
 * @param 802.15.4 coexistence event
 */
void esp_coex_ieee802154_txrx_pti_set(ieee802154_coex_event_t event);

/**
 * @brief Set 802.15.4 ack pti
 * @param 802.15.4 coexistence event
 */
void esp_coex_ieee802154_ack_pti_set(ieee802154_coex_event_t event);

/**
 * @brief Indicate that a coexistence break occurred in 802.15.4
 */
void esp_coex_ieee802154_coex_break_notify(void);

/**
 * @brief Enter the TX stage for 802.15.4 external coexistence handling
 */
void esp_coex_ieee802154_extcoex_tx_stage(void);

/**
 * @brief Enter the RX stage for 802.15.4 external coexistence handling
 */
void esp_coex_ieee802154_extcoex_rx_stage(void);

/**
 * @brief Enable the 802.15.4 status for coexistence
 */
void esp_coex_ieee802154_status_enable(void);

/**
 * @brief Disable the 802.15.4 status for coexistence
 */
void esp_coex_ieee802154_status_disable(void);

#endif
