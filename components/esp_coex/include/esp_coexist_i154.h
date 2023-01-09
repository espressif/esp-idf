/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __COEXIST_I154_H__
#define __COEXIST_I154_H__

#ifdef SOC_IEEE802154_SUPPORTED
typedef enum {
    IEEE802154_RISK_TX = 1,
    IEEE802154_TX_AT,
    IEEE802154_RX_AT,
    IEEE802154_ACK,
    IEEE802154_NORMAL,
    IEEE802154_IDLE_RX,
    IEEE802154_EVENT_MAX,
} ieee802154_coex_event_t;

void esp_ieee802154_coex_pti_set(ieee802154_coex_event_t event);
#endif

#endif
