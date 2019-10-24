// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "utils/includes.h"
#include "utils/common.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/wpa_i.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "rsn_supp/wpa_ie.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_i.h"
#include "ap/ap_config.h"
#include "ap/hostapd.h"
#include "esp_wpas_glue.h"
#include "esp_hostap.h"

#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/aes_wrap.h"

#include "esp_wifi_driver.h"
#include "esp_private/wifi.h"

void  wpa_install_key(enum wpa_alg alg, u8 *addr, int key_idx, int set_tx,
                      u8 *seq, size_t seq_len, u8 *key, size_t key_len, int key_entry_valid)
{
    esp_wifi_set_sta_key_internal(alg, addr, key_idx, set_tx, seq, seq_len, key, key_len, key_entry_valid);
}

int  wpa_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx,
                 u8 *key, size_t key_len, int key_entry_valid)
{
    return esp_wifi_get_sta_key_internal(ifx, alg, addr, key_idx, key, key_len, key_entry_valid);
}

/**
 * eapol_sm_notify_eap_success - Notification of external EAP success trigger
 * @sm: Pointer to EAPOL state machine allocated with eapol_sm_init()
 * @success: %TRUE = set success, %FALSE = clear success
 *
 * Notify the EAPOL state machine that external event has forced EAP state to
 * success (success = %TRUE). This can be cleared by setting success = %FALSE.
 *
 * This function is called to update EAP state when WPA-PSK key handshake has
 * been completed successfully since WPA-PSK does not use EAP state machine.
 */

/* fix buf for tx for now */
#define WPA_TX_MSG_BUFF_MAXLEN 200

void  wpa_sendto_wrapper(void *buffer, u16 len)
{
    esp_wifi_internal_tx(0, buffer, len);
}

void  wpa_deauthenticate(u8 reason_code)
{
    esp_wifi_deauthenticate_internal(reason_code);
}

void  wpa_config_profile()
{
    if (esp_wifi_sta_prof_is_wpa_internal()) {
        wpa_set_profile(WPA_PROTO_WPA, esp_wifi_sta_get_prof_authmode_internal());
    } else if (esp_wifi_sta_prof_is_wpa2_internal()) {
        wpa_set_profile(WPA_PROTO_RSN, esp_wifi_sta_get_prof_authmode_internal());
    } else {
        WPA_ASSERT(0);
    }
}

int wpa_config_bss(uint8_t *bssid)
{
    struct wifi_ssid *ssid = esp_wifi_sta_get_prof_ssid_internal();
    u8 mac[6];

    esp_wifi_get_macaddr_internal(0, mac);
    wpa_set_bss((char *)mac, (char *)bssid, esp_wifi_sta_get_pairwise_cipher_internal(), esp_wifi_sta_get_group_cipher_internal(),
                (char *)esp_wifi_sta_get_prof_password_internal(), ssid->ssid, ssid->len);
    return ESP_OK;
}

void  wpa_config_assoc_ie(u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len)
{
    if (proto == BIT(0)) {
        esp_wifi_set_appie_internal(WIFI_APPIE_WPA, assoc_buf, assoc_wpa_ie_len, 1);
    } else {
        esp_wifi_set_appie_internal(WIFI_APPIE_RSN, assoc_buf, assoc_wpa_ie_len, 1);
    }
}

void  wpa_neg_complete()
{
    esp_wifi_auth_done_internal();
}

void  wpa_attach(void)
{
#ifndef IOT_SIP_MODE
    wpa_register(NULL, wpa_sendto_wrapper,
                 wpa_config_assoc_ie, wpa_install_key, wpa_get_key, wpa_deauthenticate, wpa_neg_complete);
#else
    u8 *payload = (u8 *)os_malloc(WPA_TX_MSG_BUFF_MAXLEN);
    wpa_register(payload, wpa_sendto_wrapper,
                 wpa_config_assoc_ie, wpa_install_key, wpa_get_key, wpa_deauthenticate, wpa_neg_complete);
#endif

    esp_wifi_register_tx_cb_internal(eapol_txcb, WIFI_TXCB_EAPOL_ID);
}

uint8_t  *wpa_ap_get_wpa_ie(uint8_t *ie_len)
{
    struct hostapd_data *hapd = (struct hostapd_data *)esp_wifi_get_hostap_private_internal();

    if (!hapd || !hapd->wpa_auth || !hapd->wpa_auth->wpa_ie) {
        return NULL;
    }

    *ie_len = hapd->wpa_auth->wpa_ie_len;
    return hapd->wpa_auth->wpa_ie;
}

bool  wpa_ap_rx_eapol(void *hapd_data, void *sm_data, u8 *data, size_t data_len)
{
    struct hostapd_data *hapd = (struct hostapd_data *)hapd_data;
    struct wpa_state_machine *sm = (struct wpa_state_machine *)sm_data;

    if (!hapd || !sm) {
        return false;
    }

    wpa_receive(hapd->wpa_auth, sm, data, data_len);

    return true;
}

bool  wpa_deattach(void)
{
    return true;
}

void  wpa_sta_connect(uint8_t *bssid)
{
    wpa_config_profile();
    wpa_config_bss(bssid);
}

int cipher_type_map(int wpa_cipher)
{
    switch (wpa_cipher) {
    case WPA_CIPHER_NONE:
        return WIFI_CIPHER_TYPE_NONE;

    case WPA_CIPHER_WEP40:
        return WIFI_CIPHER_TYPE_WEP40;

    case WPA_CIPHER_WEP104:
        return WIFI_CIPHER_TYPE_WEP104;

    case WPA_CIPHER_TKIP:
        return WIFI_CIPHER_TYPE_TKIP;

    case WPA_CIPHER_CCMP:
        return WIFI_CIPHER_TYPE_CCMP;

    case WPA_CIPHER_CCMP|WPA_CIPHER_TKIP:
        return WIFI_CIPHER_TYPE_TKIP_CCMP;

    default:
        return WIFI_CIPHER_TYPE_UNKNOWN;
    }
}

int wpa_parse_wpa_ie_wrapper(const u8 *wpa_ie, size_t wpa_ie_len, wifi_wpa_ie_t *data)
{
    struct wpa_ie_data ie;
    int ret = 0;

    ret = wpa_parse_wpa_ie(wpa_ie, wpa_ie_len, &ie);
    data->proto = ie.proto;
    data->pairwise_cipher = cipher_type_map(ie.pairwise_cipher);
    data->group_cipher = cipher_type_map(ie.group_cipher);
    data->key_mgmt = ie.key_mgmt;
    data->capabilities = ie.capabilities;
    data->pmkid = ie.pmkid;
    data->mgmt_group_cipher = cipher_type_map(ie.mgmt_group_cipher);

    return ret;
}

int esp_supplicant_init(void)
{
    struct wpa_funcs *wpa_cb;

    wpa_cb = (struct wpa_funcs *)os_malloc(sizeof(struct wpa_funcs));
    if (!wpa_cb) {
        return ESP_ERR_NO_MEM;
    }

    wpa_cb->wpa_sta_init       = wpa_attach;
    wpa_cb->wpa_sta_deinit     = wpa_deattach;
    wpa_cb->wpa_sta_rx_eapol   = wpa_sm_rx_eapol;
    wpa_cb->wpa_sta_connect    = wpa_sta_connect;
    wpa_cb->wpa_sta_in_4way_handshake = wpa_sta_in_4way_handshake;

    wpa_cb->wpa_ap_join       = wpa_ap_join;
    wpa_cb->wpa_ap_remove     = wpa_ap_remove;
    wpa_cb->wpa_ap_get_wpa_ie = wpa_ap_get_wpa_ie;
    wpa_cb->wpa_ap_rx_eapol   = wpa_ap_rx_eapol;
    wpa_cb->wpa_ap_init       = hostap_init;
    wpa_cb->wpa_ap_deinit     = hostap_deinit;

    wpa_cb->wpa_config_parse_string  = wpa_config_parse_string;
    wpa_cb->wpa_parse_wpa_ie  = wpa_parse_wpa_ie_wrapper;
    wpa_cb->wpa_config_bss = wpa_config_bss;
    wpa_cb->wpa_michael_mic_failure = wpa_michael_mic_failure;

    esp_wifi_register_wpa_cb_internal(wpa_cb);

    return ESP_OK;
}

int esp_supplicant_deinit(void)
{
    return esp_wifi_unregister_wpa_cb_internal();
}
