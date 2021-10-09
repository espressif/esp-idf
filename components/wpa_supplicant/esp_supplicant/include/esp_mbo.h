/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
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
  * enum non_pref_chan_reason: Reason for non preference of channel
  */
enum non_pref_chan_reason {
	NON_PREF_CHAN_REASON_UNSPECIFIED = 0,
	NON_PREF_CHAN_REASON_RSSI = 1,
	NON_PREF_CHAN_REASON_EXT_INTERFERENCE = 2,
	NON_PREF_CHAN_REASON_INT_INTERFERENCE = 3,
};

/**
  * @brief  Channel structure for non preferred channel
  *
  * @param  reason: enum non_pref_chan_reason
  * @param  oper_class: operating class for the channel
  * @param  chan: channel number
  * @param  preference: channel preference
  */
struct non_pref_chan {
	enum non_pref_chan_reason reason;
	uint8_t oper_class;
	uint8_t chan;
	uint8_t preference;
};

/**
  * @brief  Array structure for non preferred channel struct
  *
  * @param  non_pref_chan_num: channel count
  * @param  chan: array of non_pref_chan type
  */
struct non_pref_chan_s {
	size_t non_pref_chan_num;
	struct non_pref_chan chan[];
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
