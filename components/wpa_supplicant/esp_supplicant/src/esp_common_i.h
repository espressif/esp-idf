/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_COMMON_I_H
#define ESP_COMMON_I_H

#include "utils/includes.h"

struct wpa_funcs;
extern struct wpa_supplicant g_wpa_supp;

#ifdef CONFIG_WPA_11KV_SUPPORT
struct ieee_mgmt_frame {
	u8 sender[ETH_ALEN];
	u8 channel;
	u32 rssi;
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
void esp_set_scan_ie(void);
void esp_set_assoc_ie(uint8_t *bssid, const u8 *ies, size_t ies_len, bool add_mdie);
#endif
