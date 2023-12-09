/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_MBO_H
#define _ESP_MBO_H

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of reasons for a channel being non-preferred in a wireless network.
 *
 * This enumeration defines various reasons why a specific channel might be considered non-preferred
 * in a wireless network configuration.
 */
enum non_pref_chan_reason {
    NON_PREF_CHAN_REASON_UNSPECIFIED = 0,      /**< Unspecified reason for non-preference */
    NON_PREF_CHAN_REASON_RSSI = 1,             /**< Non-preferred due to low RSSI (Received Signal Strength Indication) */
    NON_PREF_CHAN_REASON_EXT_INTERFERENCE = 2, /**< Non-preferred due to external interference */
    NON_PREF_CHAN_REASON_INT_INTERFERENCE = 3, /**< Non-preferred due to internal interference */
};

/**
 * @brief Structure representing a non-preferred channel in a wireless network.
 *
 * This structure encapsulates information about a non-preferred channel
 * including the reason for its non-preference, the operating class, channel number, and preference level.
 */
struct non_pref_chan {
    enum non_pref_chan_reason reason; /**< Reason for the channel being non-preferred */
    uint8_t oper_class;               /**< Operating class of the channel */
    uint8_t chan;                     /**< Channel number */
    uint8_t preference;               /**< Preference level of the channel */
};

/**
 * @brief Structure representing a list of non-preferred channels in a wireless network.
 *
 * This structure encapsulates information about a list of non-preferred channels
 * including the number of non-preferred channels and an array of structures
 * representing individual non-preferred channels.
 */
struct non_pref_chan_s {
    size_t non_pref_chan_num; /**< Number of non-preferred channels in the list */
    struct non_pref_chan chan[]; /**< Array of structures representing individual non-preferred channels */
};

/**
  * @brief  Update channel preference for MBO IE
  *
  * @param  non_pref_chan: Non preference channel list
  *
  * @return
  *    - 0: success else failure
  */
int esp_mbo_update_non_pref_chan(struct non_pref_chan_s *non_pref_chan);

#ifdef __cplusplus
}
#endif
#endif
