/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_WPA3_H
#define ESP_WPA3_H

#include "esp_wifi_driver.h"

#ifdef CONFIG_WPA3_SAE

void esp_wifi_register_wpa3_cb(struct wpa_funcs *wpa_cb);
void esp_wpa3_free_sae_data(void);

#else /* CONFIG_WPA3_SAE */

static inline void esp_wifi_register_wpa3_cb(struct wpa_funcs *wpa_cb)
{
    wpa_cb->wpa3_build_sae_msg = NULL;
    wpa_cb->wpa3_parse_sae_msg = NULL;
}

static inline void esp_wpa3_free_sae_data(void)
{
}

#endif /* CONFIG_WPA3_SAE */

#ifdef CONFIG_SAE
enum SIG_WPA3_TASK {
    SIG_WPA3_RX_COMMIT,
    SIG_WPA3_RX_CONFIRM,
    SIG_TASK_DEL,
    SIG_TASK_MAX
};

typedef struct {
    uint32_t id;
    uint32_t data;
} wpa3_hostap_auth_event_t;

struct sae_hostap_confirm_data {
    size_t len;
    u8 bssid[ETH_ALEN];
    u32 auth_transaction;
    u16 status;
    u8 msg[];
};

#define WPA3_HOSTAP_HANDLE_AUTH_TASK_STACK_SIZE  (6144)
#define WPA3_HOSTAP_HANDLE_AUTH_TASK_PRIORITY    (19)
#define WPA3_HOSTAP_AUTH_API_LOCK() os_semphr_take(g_wpa3_hostap_auth_api_lock, OS_BLOCK)
#define WPA3_HOSTAP_AUTH_API_UNLOCK() os_semphr_give(g_wpa3_hostap_auth_api_lock)

int wpa3_hostap_post_evt(uint32_t evt_id, uint32_t data);
void esp_wifi_register_wpa3_ap_cb(struct wpa_funcs *wpa_cb);
int wpa3_hostap_auth_init(void *data);
bool wpa3_hostap_auth_deinit(void);
int esp_send_sae_auth_reply(struct hostapd_data *hapd,
               const u8 *dst, const u8 *bssid,
               u16 auth_alg, u16 auth_transaction, u16 resp,
               const u8 *ies, size_t ies_len);

#else /* CONFIG_SAE */

static inline void esp_wifi_register_wpa3_ap_cb(struct wpa_funcs *wpa_cb)
{
	wpa_cb->wpa3_hostap_handle_auth = NULL;
}

#endif /* CONFIG_SAE */
#endif /* ESP_WPA3_H */
