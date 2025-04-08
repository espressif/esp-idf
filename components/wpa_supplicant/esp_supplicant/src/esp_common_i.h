/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_COMMON_I_H
#define ESP_COMMON_I_H

#include "utils/includes.h"

struct wpa_funcs;
extern struct wpa_supplicant g_wpa_supp;

#if defined(CONFIG_RRM) || defined(CONFIG_WNM)
struct ieee_mgmt_frame {
	u8 sender[ETH_ALEN];
	u8 channel;
	int8_t rssi;
	size_t len;
	u8 payload[0];
};

void esp_get_tx_power(uint8_t *tx_power);
#ifdef CONFIG_MBO
bool mbo_bss_profile_match(u8 *bssid);
#endif
#endif /* defined(CONFIG_RRM) || defined(CONFIG_WNM) */
int esp_supplicant_common_init(struct wpa_funcs *wpa_cb);
void esp_supplicant_common_deinit(void);
void esp_supplicant_unset_all_appie(void);
void esp_set_scan_ie(void);
void esp_set_assoc_ie(uint8_t *bssid, const u8 *ies, size_t ies_len, bool add_mdie);
void supplicant_sta_conn_handler(uint8_t* bssid);
void supplicant_sta_disconn_handler(uint8_t reason_code);
#endif
void wpa_sta_clear_ft_auth_ie(void);
