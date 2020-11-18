/**
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
	REASON_QOS_CAPACITY = 3,
	REASON_FIRST_ASSOC = 4,
	REASON_LOAD_BALALNCE = 5,
	REASON_BETTER_AP = 6,
	REASON_CURRENT_DEAUTH = 7,
};

/**
  * @brief  Send bss transition query to connected AP
  *
  * @param  query_reason: reason for sending query
  * @param  btm_candidates: btm candidates list if available
  * @param  cand_list: whether candidate list to be included from scan results available in supplicant's cache.
  *
  * @return
  *    - 0: success else failure
  */
int esp_wnm_send_bss_transition_mgmt_query(enum btm_query_reason query_reason,
					   const char *btm_candidates,
					   int cand_list);

#ifdef __cplusplus
}
#endif
#endif
