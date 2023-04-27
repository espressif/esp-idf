/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
// TODO: replace etm code using common interface

#define IEEE802154_ETM_CHANNEL0           0
#define IEEE802154_ETM_CHANNEL1           1

/**
 * @brief The scene of IEEE802154 radio coexistence.
 */
typedef enum {
    IEEE802154_SCENE_IDLE,      /*!< IEEE802154 radio coexistence scene IDLE */
    IEEE802154_SCENE_TX,        /*!< IEEE802154 radio coexistence scene TX */
    IEEE802154_SCENE_RX,        /*!< IEEE802154 radio coexistence scene RX */
    IEEE802154_SCENE_TX_AT,     /*!< IEEE802154 radio coexistence scene TX AT */
    IEEE802154_SCENE_RX_AT,     /*!< IEEE802154 radio coexistence scene RX AT */
} ieee802154_txrx_scene_t;

#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE

/**
 * @brief  Set the IEEE802154 radio coexistence scene during transmitting or receiving.
 *
 * @param[in]  txrx_scene  The scene of IEEE802154 radio coexistence.
 *
 */
void ieee802154_set_txrx_pti(ieee802154_txrx_scene_t txrx_scene);

#define IEEE802154_SET_TXRX_PTI(txrx_scene) ieee802154_set_txrx_pti(txrx_scene)

#else

#define IEEE802154_SET_TXRX_PTI(txrx_scene)

#endif // CONFIG_ESP_COEX_SW_COEXIST_ENABLE

/**
 * @brief  Convert the frequence to the index of channel.
 *
 * @param[in]  freq  The frequence where the radio is processing.
 *
 * @return
 *          The channel index.
 *
 */
uint8_t ieee802154_freq_to_channel(uint8_t freq);

/**
 * @brief  Convert the index of channel to the frequence.
 *
 * @param[in]  channel  The index of channel where the radio is processing.
 *
 * @return
 *          The frequence.
 *
 */
uint8_t ieee802154_channel_to_freq(uint8_t channel);

// TZ-97: implement these two functions using ETM common interface
/**
 * @brief  Configure the ETM module, using [channel] for monitoring the event, when event appears
 *         hardware will operate the [task].
 *
 * @param[in]  channel  The ETM channel.
 * @param[in]  event  The ETM event.
 * @param[in]  task  The ETM task.
 *
 */
void ieee802154_etm_set_event_task(uint32_t channel, uint32_t event, uint32_t task);

/**
 * @brief  Clear the ETM module [channel].
 *
 * @param[in]  channel  The ETM channel.
 *
 */
void ieee802154_etm_channel_clear(uint32_t channel);

#ifdef __cplusplus
}
#endif
