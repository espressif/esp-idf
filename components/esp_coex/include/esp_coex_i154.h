/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __COEXIST_I154_H__
#define __COEXIST_I154_H__

#ifdef CONFIG_SOC_IEEE802154_SUPPORTED
typedef enum {
    IEEE802154_HIGH = 1,
    IEEE802154_MIDDLE,
    IEEE802154_LOW,
    IEEE802154_IDLE,
    IEEE802154_EVENT_MAX,
} ieee802154_coex_event_t;

void esp_coex_ieee802154_txrx_pti_set(ieee802154_coex_event_t event);
void esp_coex_ieee802154_ack_pti_set(ieee802154_coex_event_t event);
void esp_coex_ieee802154_coex_break_notify(void);
void esp_coex_ieee802154_extcoex_tx_stage(void);
void esp_coex_ieee802154_extcoex_rx_stage(void);
#endif

#endif
