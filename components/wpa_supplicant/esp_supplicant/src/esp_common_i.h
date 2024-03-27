/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_COMMON_I_H
#define ESP_COMMON_I_H

#include "utils/includes.h"

struct wpa_funcs;
extern struct wpa_supplicant g_wpa_supp;

#ifdef CONFIG_IEEE80211KV
struct ieee_mgmt_frame {
	u8 sender[ETH_ALEN];
	u8 channel;
	int8_t rssi;
	size_t len;
	u8 payload[0];
};

int esp_supplicant_post_evt(uint32_t evt_id, uint32_t data);

typedef struct {
    uint32_t id;
    uint32_t data;
} supplicant_event_t;

enum SIG_SUPPLICANT {
	SIG_SUPPLICANT_RX_ACTION,
	SIG_SUPPLICANT_SCAN_DONE,
	SIG_SUPPLICANT_DEL_TASK,
	SIG_SUPPLICANT_MAX,
};

void esp_get_tx_power(uint8_t *tx_power);
#ifdef CONFIG_MBO
bool mbo_bss_profile_match(u8 *bssid);
#endif
#endif
int esp_supplicant_common_init(struct wpa_funcs *wpa_cb);
void esp_supplicant_common_deinit(void);
void esp_supplicant_unset_all_appie(void);
void esp_set_scan_ie(void);
void esp_set_assoc_ie(uint8_t *bssid, const u8 *ies, size_t ies_len, bool add_mdie);
void supplicant_sta_conn_handler(uint8_t* bssid);
void supplicant_sta_disconn_handler(void);
#endif
