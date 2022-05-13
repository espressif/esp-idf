/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "crypto/sha1.h"
#include "common/ieee802_11_defs.h"
#include "common/eapol_common.h"
#include "ap/wpa_auth.h"
#include "ap/ap_config.h"
#include "utils/wpa_debug.h"
#include "ap/hostapd.h"
#include "ap/wpa_auth_i.h"
#include "esp_wifi_driver.h"
#include "esp_wifi_types.h"

struct hostapd_data *global_hapd;

struct hostapd_data *hostapd_get_hapd_data(void)
{
    return global_hapd;
}

void *hostap_init(void)
{
    struct wifi_ssid *ssid = esp_wifi_ap_get_prof_ap_ssid_internal();
    struct hostapd_data *hapd = NULL;
    struct wpa_auth_config *auth_conf;
    u16 spp_attrubute = 0;
    u8 pairwise_cipher;
    wifi_pmf_config_t pmf_cfg;

    hapd = (struct hostapd_data *)os_zalloc(sizeof(struct hostapd_data));

    if (hapd == NULL) {
        return NULL;
    }

    hapd->conf = (struct hostapd_bss_config *)os_zalloc(sizeof(struct hostapd_bss_config));

    if (hapd->conf == NULL) {
        os_free(hapd);
        return NULL;
    }
    hapd->conf->max_num_sta = MAX_STA_COUNT;

    auth_conf = (struct wpa_auth_config *)os_zalloc(sizeof(struct  wpa_auth_config));

    if (auth_conf == NULL) {
        os_free(hapd->conf);
        os_free(hapd);
        hapd = NULL;
        return NULL;
    }
    if (esp_wifi_ap_get_prof_authmode_internal() == WIFI_AUTH_WPA_PSK) {
        auth_conf->wpa = WPA_PROTO_WPA;
    }
    if (esp_wifi_ap_get_prof_authmode_internal() == WIFI_AUTH_WPA2_PSK) {
        auth_conf->wpa = WPA_PROTO_RSN;
    }
    if (esp_wifi_ap_get_prof_authmode_internal() == WIFI_AUTH_WPA_WPA2_PSK) {
        auth_conf->wpa = WPA_PROTO_RSN | WPA_PROTO_WPA;
    }

    pairwise_cipher = esp_wifi_ap_get_prof_pairwise_cipher_internal();

#ifdef CONFIG_IEEE80211W

    esp_wifi_get_pmf_config_internal(&pmf_cfg, WIFI_IF_AP);

    if (pmf_cfg.required) {
	pairwise_cipher = WIFI_CIPHER_TYPE_CCMP;
    }
#endif /* CONFIG_IEEE80211W */

    /* TKIP is compulsory in WPA Mode */
    if (auth_conf->wpa == WPA_PROTO_WPA && pairwise_cipher == WIFI_CIPHER_TYPE_CCMP) {
        pairwise_cipher = WIFI_CIPHER_TYPE_TKIP_CCMP;
    }
    if (pairwise_cipher == WIFI_CIPHER_TYPE_TKIP) {
        auth_conf->wpa_group = WPA_CIPHER_TKIP;
        auth_conf->wpa_pairwise = WPA_CIPHER_TKIP;
        auth_conf->rsn_pairwise = WPA_CIPHER_TKIP;
    } else if (pairwise_cipher == WIFI_CIPHER_TYPE_CCMP) {
        auth_conf->wpa_group = WPA_CIPHER_CCMP;
        auth_conf->wpa_pairwise = WPA_CIPHER_CCMP;
        auth_conf->rsn_pairwise = WPA_CIPHER_CCMP;
    } else {
        auth_conf->wpa_group = WPA_CIPHER_TKIP;
        auth_conf->wpa_pairwise = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP;
        auth_conf->rsn_pairwise = WPA_CIPHER_CCMP | WPA_CIPHER_TKIP;
    }

    auth_conf->wpa_key_mgmt = WPA_KEY_MGMT_PSK;
    auth_conf->eapol_version = EAPOL_VERSION;

#ifdef CONFIG_IEEE80211W
    if (pmf_cfg.required && pmf_cfg.capable) {
	auth_conf->ieee80211w = MGMT_FRAME_PROTECTION_REQUIRED;
	auth_conf->wpa_key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
	wpa_printf(MSG_DEBUG, "%s :pmf required", __func__);
    } else if (pmf_cfg.capable && !pmf_cfg.required) {
	auth_conf->ieee80211w = MGMT_FRAME_PROTECTION_OPTIONAL;
	auth_conf->wpa_key_mgmt = WPA_KEY_MGMT_PSK;
	wpa_printf(MSG_DEBUG, "%s : pmf optional", __func__);
    }
#endif /* CONFIG_IEEE80211W */

    spp_attrubute = esp_wifi_get_spp_attrubute_internal(WIFI_IF_AP);
    auth_conf->spp_sup.capable = ((spp_attrubute & WPA_CAPABILITY_SPP_CAPABLE) ? SPP_AMSDU_CAP_ENABLE : SPP_AMSDU_CAP_DISABLE);
    auth_conf->spp_sup.require = ((spp_attrubute & WPA_CAPABILITY_SPP_REQUIRED) ? SPP_AMSDU_REQ_ENABLE : SPP_AMSDU_REQ_DISABLE);

    memcpy(hapd->conf->ssid.ssid, ssid->ssid, ssid->len);
    hapd->conf->ssid.ssid_len = ssid->len;
    hapd->conf->ssid.wpa_passphrase = (char *)os_zalloc(64);
    if (hapd->conf->ssid.wpa_passphrase == NULL) {
        os_free(auth_conf);
        os_free(hapd->conf);
        os_free(hapd);
        hapd = NULL;
        return NULL;
    }
    memcpy(hapd->conf->ssid.wpa_passphrase, esp_wifi_ap_get_prof_password_internal(), strlen((char *)esp_wifi_ap_get_prof_password_internal()));

    hapd->conf->ap_max_inactivity = 5 * 60;
    hostapd_setup_wpa_psk(hapd->conf);

    esp_wifi_get_macaddr_internal(WIFI_IF_AP, hapd->own_addr);

    hapd->wpa_auth = wpa_init(hapd->own_addr, auth_conf, NULL);
    esp_wifi_set_appie_internal(WIFI_APPIE_WPA, hapd->wpa_auth->wpa_ie, (uint16_t)hapd->wpa_auth->wpa_ie_len, 0);
    os_free(auth_conf);
    global_hapd = hapd;

    return (void *)hapd;
}

bool hostap_deinit(void *data)
{
    struct hostapd_data *hapd = (struct hostapd_data *)data;

    if (hapd == NULL) {
        return true;
    }

    if (hapd->wpa_auth != NULL) {
        if (hapd->wpa_auth->wpa_ie != NULL) {
            os_free(hapd->wpa_auth->wpa_ie);
        }
        if (hapd->wpa_auth->group != NULL) {
            os_free(hapd->wpa_auth->group);
        }
        os_free(hapd->wpa_auth);
    }

    if (hapd->conf != NULL) {
        if (hapd->conf->ssid.wpa_psk != NULL) {
            os_free(hapd->conf->ssid.wpa_psk);
        }
        if (hapd->conf->ssid.wpa_passphrase != NULL) {
            os_free(hapd->conf->ssid.wpa_passphrase);
        }
        os_free(hapd->conf);
    }

    os_free(hapd);
    esp_wifi_unset_appie_internal(WIFI_APPIE_WPA);
    global_hapd = NULL;

    return true;
}
