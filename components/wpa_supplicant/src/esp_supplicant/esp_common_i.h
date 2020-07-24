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

#ifndef ESP_COMMON_I_H
#define ESP_COMMON_I_H

#include "utils/includes.h"

struct wpa_funcs;

#ifdef ROAMING_SUPPORT
struct ieee_mgmt_frame {
	u8 sender[ETH_ALEN];
	u8 channel;
	u32 rssi;
	size_t len;
	u8 payload[0];
};

typedef struct {
    uint32_t id;
    uint32_t data;
} supplicant_event_t;

#define SUPPLICANT_API_LOCK() xSemaphoreTakeRecursive(s_supplicant_api_lock, portMAX_DELAY)
#define SUPPLICANT_API_UNLOCK() xSemaphoreGiveRecursive(s_supplicant_api_lock)

#define SUPPLICANT_TASK_STACK_SIZE (6144 + TASK_STACK_SIZE_ADD)
enum SIG_SUPPLICANT {
	SIG_SUPPLICANT_RX_ACTION,
	SIG_SUPPLICANT_SCAN_DONE,
	SIG_SUPPLICANT_DEL_TASK,
	SIG_SUPPLICANT_MAX,
};

int esp_supplicant_post_evt(uint32_t evt_id, uint32_t data);
int esp_ieee80211_handle_rx_frm(u8 type, u8 *frame, size_t len, u8 *sender,
			    u32 rssi, u8 channel, u64 current_tsf);
void esp_set_rm_enabled_ie(void);
void esp_get_tx_power(uint8_t *tx_power);
void esp_supplicant_common_init(struct wpa_funcs *wpa_cb);
#else

#include "esp_rrm.h"
#include "esp_wnm.h"

static inline void esp_set_rm_enabled_ie(void) {}
static inline int esp_ieee80211_handle_rx_frm(u8 type, u8 *frame, size_t len, u8 *sender,
			    u32 rssi, u8 channel, u64 current_tsf)
{
	return -1;
}
int esp_rrm_send_neighbor_rep_request(neighbor_rep_request_cb cb,
				      void *cb_ctx)
{
	return -1;
}

int esp_wnm_send_bss_transition_mgmt_query(enum btm_query_reason query_reason,
					   const char *btm_candidates,
					   int cand_list)
{
	return -1;
}

#endif
#endif
