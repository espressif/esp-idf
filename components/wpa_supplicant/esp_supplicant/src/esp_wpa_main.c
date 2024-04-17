/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/wpa_i.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "rsn_supp/wpa_ie.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_i.h"
#include "ap/ap_config.h"
#include "ap/hostapd.h"
#include "esp_wpas_glue.h"
#include "esp_hostap.h"

#include "esp_system.h"
#include "crypto/crypto.h"
#include "crypto/sha1.h"
#include "crypto/aes_wrap.h"

#include "esp_wifi_driver.h"
#include "esp_private/wifi.h"
#include "esp_wpa3_i.h"
#include "esp_eap_client.h"
#include "esp_common_i.h"
#include "esp_owe_i.h"

#include "esp_wps.h"
#include "eap_server/eap.h"
#include "eapol_auth/eapol_auth_sm.h"
#include "ap/ieee802_1x.h"
#include "ap/sta_info.h"
#include "wps/wps_defs.h"
#include "wps/wps.h"
#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
#include "esp_roaming.h"
#endif

#ifdef CONFIG_DPP
#include "common/dpp.h"
#include "esp_dpp_i.h"
#endif

bool g_wpa_pmk_caching_disabled = 0;
const wifi_osi_funcs_t *wifi_funcs;
struct wpa_funcs *wpa_cb;

void  wpa_install_key(enum wpa_alg alg, u8 *addr, int key_idx, int set_tx,
                      u8 *seq, size_t seq_len, u8 *key, size_t key_len, enum key_flag key_flag)
{
    esp_wifi_set_sta_key_internal(alg, addr, key_idx, set_tx, seq, seq_len, key, key_len, key_flag);
}

int  wpa_get_key(uint8_t *ifx, int *alg, u8 *addr, int *key_idx,
                 u8 *key, size_t key_len, enum key_flag key_flag)
{
    return esp_wifi_get_sta_key_internal(ifx, alg, addr, key_idx, key, key_len, key_flag);
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

int  wpa_config_profile(uint8_t *bssid)
{
    int ret = 0;

    if (esp_wifi_sta_prof_is_wpa_internal()) {
        wpa_set_profile(WPA_PROTO_WPA, esp_wifi_sta_get_prof_authmode_internal());
    } else if (esp_wifi_sta_prof_is_rsn_internal()) {
        wpa_set_profile(WPA_PROTO_RSN, esp_wifi_sta_get_prof_authmode_internal());
    } else if (esp_wifi_sta_prof_is_wapi_internal()) {
        wpa_set_profile(WPA_PROTO_WAPI, esp_wifi_sta_get_prof_authmode_internal());
    } else {
        ret = -1;
    }

    return ret;
}

int wpa_config_bss(uint8_t *bssid)
{
    int ret = 0;
    struct wifi_ssid *ssid = esp_wifi_sta_get_prof_ssid_internal();
    u8 mac[6];

    esp_wifi_get_macaddr_internal(0, mac);
    ret = wpa_set_bss((char *)mac, (char *)bssid, esp_wifi_sta_get_pairwise_cipher_internal(), esp_wifi_sta_get_group_cipher_internal(),
                      (char *)esp_wifi_sta_get_prof_password_internal(), ssid->ssid, ssid->len);
    return ret;
}

void  wpa_config_assoc_ie(u8 proto, u8 *assoc_buf, u32 assoc_wpa_ie_len)
{
    if (proto == BIT(0)) {
        esp_wifi_set_appie_internal(WIFI_APPIE_WPA, assoc_buf, assoc_wpa_ie_len, 1);
    } else {
        esp_wifi_set_appie_internal(WIFI_APPIE_RSN, assoc_buf, assoc_wpa_ie_len, 1);
    }
}

void  wpa_neg_complete(void)
{
    esp_wifi_auth_done_internal();
}

bool  wpa_attach(void)
{
    bool ret = true;
    ret = wpa_sm_init();
    if (ret) {
        ret = (esp_wifi_register_eapol_txdonecb_internal(eapol_txcb) == ESP_OK);
    }
    esp_set_scan_ie();
    return ret;
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

bool wpa_ap_rx_eapol(void *hapd_data, void *sm_data, u8 *data, size_t data_len)
{
    struct hostapd_data *hapd = (struct hostapd_data *)hapd_data;
    struct sta_info *sta = (struct sta_info *)sm_data;
    if (!hapd || !sta) {
        wpa_printf(MSG_DEBUG, "hapd=%p sta=%p", hapd, sta);
        return false;
    }
#ifdef CONFIG_WPS_REGISTRAR
    int wps_type = esp_wifi_get_wps_type_internal();

    if ((wps_type == WPS_TYPE_PBC) ||
            (wps_type == WPS_TYPE_PIN)) {
        ieee802_1x_receive(hapd, sta->addr, data, data_len);
        return true;
    }
#endif
    wpa_receive(hapd->wpa_auth, sta->wpa_sm, data, data_len);

    return true;
}

void wpa_ap_get_peer_spp_msg(void *sm_data, bool *spp_cap, bool *spp_req)
{
    struct wpa_state_machine *sm = (struct wpa_state_machine *)sm_data;

    if (!sm) {
        return;
    }

    *spp_cap = sm->spp_sup.capable;
    *spp_req = sm->spp_sup.require;
}

bool wpa_deattach(void)
{
    struct wpa_sm *sm = &gWpaSm;
    esp_wpa3_free_sae_data();
#ifdef CONFIG_ESP_WIFI_ENTERPRISE_SUPPORT
    if (sm->wpa_sm_eap_disable) {
        sm->wpa_sm_eap_disable();
    }
#endif
    if (sm->wpa_sm_wps_disable) {
        sm->wpa_sm_wps_disable();
    }
    esp_wifi_register_eapol_txdonecb_internal(NULL);

    wpa_sm_deinit();
    return true;
}

#ifdef CONFIG_DPP
int dpp_connect(uint8_t *bssid, bool pdr_done)
{
    int res = 0;
    if (!pdr_done) {
        if (esp_wifi_sta_get_prof_authmode_internal() == WPA3_AUTH_DPP) {
            esp_dpp_post_evt(SIG_DPP_START_NET_INTRO, (u32)bssid);
        }
    } else {
        res = wpa_config_bss(bssid);
        if (res) {
            wpa_printf(MSG_DEBUG, "Rejecting bss, validation failed");
            return res;
        }
        res = esp_wifi_sta_connect_internal(bssid);
    }
    return res;
}
#endif

int wpa_sta_connect(uint8_t *bssid)
{
    /* use this API to set AP specific IEs during connection */
    int ret = 0;
    ret = wpa_config_profile(bssid);
    if (ret == 0) {
        ret = wpa_config_bss(bssid);
        if (ret) {
            wpa_printf(MSG_DEBUG, "Rejecting bss, validation failed");
            return ret;
        }
    } else if (esp_wifi_sta_get_prof_authmode_internal() == NONE_AUTH) {
        esp_set_assoc_ie((uint8_t *)bssid, NULL, 0, false);
    }

#ifdef CONFIG_DPP
    struct wpa_sm *sm = &gWpaSm;
    if (sm->key_mgmt == WPA_KEY_MGMT_DPP) {
        ret = dpp_connect(bssid, false);
    } else
#endif
    {
        ret = esp_wifi_sta_connect_internal(bssid);
    }
    return ret;
}

void wpa_config_done(void)
{
    esp_set_scan_ie();
}

int wpa_parse_wpa_ie_wrapper(const u8 *wpa_ie, size_t wpa_ie_len, wifi_wpa_ie_t *data)
{
    struct wpa_ie_data ie;
    int ret = 0;

    ret = wpa_parse_wpa_ie(wpa_ie, wpa_ie_len, &ie);
    data->proto = ie.proto;
    data->pairwise_cipher = cipher_type_map_supp_to_public(ie.pairwise_cipher);
    data->group_cipher = cipher_type_map_supp_to_public(ie.group_cipher);
    data->key_mgmt = ie.key_mgmt;
    data->capabilities = ie.capabilities;
    data->pmkid = ie.pmkid;
    data->mgmt_group_cipher = cipher_type_map_supp_to_public(ie.mgmt_group_cipher);
    data->rsnxe_capa = ie.rsnxe_capa;

    return ret;
}

static void wpa_sta_connected_cb(uint8_t *bssid)
{
    supplicant_sta_conn_handler(bssid);
}

static void wpa_sta_disconnected_cb(uint8_t reason_code)
{
    switch (reason_code) {
    case WIFI_REASON_AUTH_EXPIRE:
    case WIFI_REASON_NOT_AUTHED:
    case WIFI_REASON_NOT_ASSOCED:
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    case WIFI_REASON_INVALID_PMKID:
    case WIFI_REASON_AUTH_FAIL:
    case WIFI_REASON_ASSOC_FAIL:
    case WIFI_REASON_CONNECTION_FAIL:
    case WIFI_REASON_HANDSHAKE_TIMEOUT:
        esp_wpa3_free_sae_data();
        wpa_sta_clear_curr_pmksa();
        wpa_sm_notify_disassoc(&gWpaSm);
        break;
    default:
        if (g_wpa_pmk_caching_disabled) {
            wpa_sta_clear_curr_pmksa();
        }
        break;
    }
#ifdef CONFIG_OWE_STA
    owe_deinit();
#endif /* CONFIG_OWE_STA */

    supplicant_sta_disconn_handler(reason_code);
}

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT

#ifdef CONFIG_WPS_REGISTRAR
static int check_n_add_wps_sta(struct hostapd_data *hapd, struct sta_info *sta_info, u8 *ies, u8 ies_len, bool *pmf_enable, int subtype)
{
    struct wpabuf *wps_ie = ieee802_11_vendor_ie_concat(ies, ies_len, WPS_DEV_OUI_WFA);
    int wps_type = esp_wifi_get_wps_type_internal();

    /* Condition for this, WPS is running and WPS IEs are part of assoc req */
    if (!wps_ie || (wps_type == WPS_TYPE_DISABLE)) {
        return 0;
    }

    if (wps_type == WPS_TYPE_PBC) {
        if (esp_wps_registrar_check_pbc_overlap(hapd->wps)) {
            wpa_printf(MSG_DEBUG, "WPS: PBC session overlap detected");
            return -1;
        }
    }

    sta_info->wps_ie = wps_ie;
    sta_info->eapol_sm = ieee802_1x_alloc_eapol_sm(hapd, sta_info);

    if (sta_info->eapol_sm) {
        wpa_printf(MSG_DEBUG, "considering station " MACSTR " for WPS", MAC2STR(sta_info->addr));
        if (esp_send_assoc_resp(hapd, sta_info->addr, WLAN_STATUS_SUCCESS, true, subtype) != WLAN_STATUS_SUCCESS) {
            wpa_printf(MSG_ERROR, "failed to send assoc response " MACSTR, MAC2STR(sta_info->addr));
            return -1;
        }
    }

    return 0;
}
#endif

static bool hostap_sta_join(void **sta, u8 *bssid, u8 *wpa_ie, u8 wpa_ie_len, u8 *rsnxe, u8 rsnxe_len, bool *pmf_enable, int subtype, uint8_t *pairwise_cipher)
{
    struct sta_info *sta_info = NULL;
    struct hostapd_data *hapd = hostapd_get_hapd_data();

    if (!hapd) {
        goto fail;
    }

    if (*sta) {
        struct sta_info *old_sta = *sta;
#ifdef CONFIG_SAE
        if (old_sta->lock && os_semphr_take(old_sta->lock, 0) != TRUE) {
            wpa_printf(MSG_INFO, "Ignore assoc request as softap is busy with sae calculation for station "MACSTR, MAC2STR(bssid));
            if (esp_send_assoc_resp(hapd, bssid, WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY, rsnxe ? false : true, subtype) != WLAN_STATUS_SUCCESS) {
                goto fail;
            }
            return false;
        }
#endif /* CONFIG_SAE */
        if (!esp_wifi_ap_is_sta_sae_reauth_node(bssid)) {
            ap_free_sta(hapd, old_sta);
            *sta = NULL;
        }
#ifdef CONFIG_SAE
        else if (old_sta && old_sta->lock) {
            sta_info = old_sta;
            goto process_old_sta;
        }
#endif /* CONFIG_SAE */
    }

    sta_info = ap_get_sta(hapd, bssid);
    if (!sta_info) {
        sta_info = ap_sta_add(hapd, bssid);
        if (!sta_info) {
            wpa_printf(MSG_ERROR, "failed to add station " MACSTR, MAC2STR(bssid));
            goto fail;
        }
    }
#ifdef CONFIG_SAE
    if (sta_info->lock && os_semphr_take(sta_info->lock, 0) != TRUE) {
        wpa_printf(MSG_INFO, "Ignore assoc request as softap is busy with sae calculation for station "MACSTR, MAC2STR(bssid));
        if (esp_send_assoc_resp(hapd, bssid, WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY, rsnxe ? false : true, subtype) != WLAN_STATUS_SUCCESS) {
            goto fail;
        }
        return false;
    }

process_old_sta:
#endif /* CONFIG_SAE */

#ifdef CONFIG_WPS_REGISTRAR
    if (check_n_add_wps_sta(hapd, sta_info, wpa_ie, wpa_ie_len, pmf_enable, subtype) == 0) {
        if (sta_info->eapol_sm) {
            goto done;
        }
    } else {
        goto fail;
    }
#endif
    if (wpa_ap_join(sta_info, bssid, wpa_ie, wpa_ie_len, rsnxe, rsnxe_len, pmf_enable, subtype, pairwise_cipher)) {
        goto done;
    } else {
        goto fail;
    }
done:
    *sta = sta_info;
#ifdef CONFIG_SAE
    if (sta_info->lock) {
        os_semphr_give(sta_info->lock);
    }
#endif /* CONFIG_SAE */
    return true;

fail:

#ifdef CONFIG_SAE
    if (sta_info && sta_info->lock) {
        os_semphr_give(sta_info->lock);
    }
#endif /* CONFIG_SAE */
    esp_wifi_ap_deauth_internal(bssid, WLAN_REASON_PREV_AUTH_NOT_VALID);
    return false;
}
#endif

int esp_supplicant_init(void)
{
    int ret = ESP_OK;

    wifi_funcs = WIFI_OSI_FUNCS_INITIALIZER();
    if (!wifi_funcs) {
        return ESP_ERR_NOT_FOUND;
    }
    wpa_cb = (struct wpa_funcs *)os_zalloc(sizeof(struct wpa_funcs));
    if (!wpa_cb) {
        return ESP_ERR_NO_MEM;
    }

    wpa_cb->wpa_sta_init       = wpa_attach;
    wpa_cb->wpa_sta_deinit     = wpa_deattach;
    wpa_cb->wpa_sta_rx_eapol   = wpa_sm_rx_eapol;
    wpa_cb->wpa_sta_connect    = wpa_sta_connect;
    wpa_cb->wpa_sta_connected_cb    = wpa_sta_connected_cb;
    wpa_cb->wpa_sta_disconnected_cb = wpa_sta_disconnected_cb;
    wpa_cb->wpa_sta_in_4way_handshake = wpa_sta_in_4way_handshake;

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    wpa_cb->wpa_ap_join       = hostap_sta_join;
    wpa_cb->wpa_ap_remove     = wpa_ap_remove;
    wpa_cb->wpa_ap_get_wpa_ie = wpa_ap_get_wpa_ie;
    wpa_cb->wpa_ap_rx_eapol   = wpa_ap_rx_eapol;
    wpa_cb->wpa_ap_get_peer_spp_msg  = wpa_ap_get_peer_spp_msg;
    wpa_cb->wpa_ap_init       = hostap_init;
    wpa_cb->wpa_ap_deinit     = hostap_deinit;
#endif

    wpa_cb->wpa_config_parse_string  = wpa_config_parse_string;
    wpa_cb->wpa_parse_wpa_ie  = wpa_parse_wpa_ie_wrapper;
    wpa_cb->wpa_config_bss = NULL;//wpa_config_bss;
    wpa_cb->wpa_michael_mic_failure = wpa_michael_mic_failure;
    wpa_cb->wpa_config_done = wpa_config_done;
    wpa_cb->wpa_sta_set_ap_rsnxe = wpa_sm_set_ap_rsnxe;

    esp_wifi_register_wpa3_ap_cb(wpa_cb);
    esp_wifi_register_wpa3_cb(wpa_cb);
#ifdef CONFIG_OWE_STA
    esp_wifi_register_owe_cb(wpa_cb);
#endif /* CONFIG_OWE_STA */
    eloop_init();
    ret = esp_supplicant_common_init(wpa_cb);

    if (ret != 0) {
        return ret;
    }

    esp_wifi_register_wpa_cb_internal(wpa_cb);

#if CONFIG_ESP_WIFI_WAPI_PSK
    ret =  esp_wifi_internal_wapi_init();
#endif

#if CONFIG_ESP_WIFI_ENABLE_ROAMING_APP
    init_roaming_app();
#endif

    return ret;
}

int esp_supplicant_deinit(void)
{
    esp_supplicant_common_deinit();
    esp_supplicant_unset_all_appie();
    eloop_destroy();
    wpa_cb = NULL;
    return esp_wifi_unregister_wpa_cb_internal();
}

esp_err_t esp_supplicant_disable_pmk_caching(bool disable)
{
    g_wpa_pmk_caching_disabled = disable;
    return ESP_OK;
}
