/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_WNM_H
#define _ESP_WNM_H

#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
  * enum btm_query_reason: Reason code for sending btm query
  */
enum btm_query_reason {
	REASON_UNSPECIFIED = 0,
	REASON_FRAME_LOSS = 1,
	REASON_DELAY = 2,
	REASON_BANDWIDTH = 3,
	REASON_LOAD_BALANCE = 4,
	REASON_RSSI = 5,
	REASON_RETRANSMISSIONS = 6,
	REASON_INTERFERENCE = 7,
	REASON_GRAY_ZONE = 8,
	REASON_PREMIUM_AP = 9,
};

/**
  * @brief  Send bss transition query to connected AP
  *
  * @param  query_reason: reason for sending query
  * @param  btm_candidates: btm candidates list if available
  * @param  cand_list: whether candidate list to be included from scan results available in supplicant's cache.
  *
  * @return
  *    - 0: success
  *    - -1: AP does not support BTM
  *    - -2: station not connected to AP
  */
int esp_wnm_send_bss_transition_mgmt_query(enum btm_query_reason query_reason,
					   const char *btm_candidates,
					   int cand_list);

/**
  * @brief  Check bss trasition capability of connected AP
  *
  * @return
  *    - true: AP supports BTM
  *    - false: AP does not support BTM or station not connected to AP
  */
bool esp_wnm_is_btm_supported_connection(void);

#ifdef __cplusplus
}
#endif
#endif
