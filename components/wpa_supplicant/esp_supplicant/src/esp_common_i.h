/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
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
void esp_get_tx_power(uint8_t *tx_power);
int esp_supplicant_common_init(struct wpa_funcs *wpa_cb);
void esp_supplicant_common_deinit(void);
void esp_set_scan_ie(void);
void esp_set_assoc_ie(void);
#else

#include "esp_rrm.h"
#include "esp_wnm.h"
#include "esp_mbo.h"

static inline void esp_set_scan_ie(void) { }
static inline void esp_set_assoc_ie(void) { }

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

int esp_mbo_update_non_pref_chan(struct non_pref_chan_s *non_pref_chan)
{
	return -1;
}
#endif
#endif
