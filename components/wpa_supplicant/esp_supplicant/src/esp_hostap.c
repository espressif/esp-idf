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

void *hostap_init(void)
{
    struct wifi_ssid *ssid = esp_wifi_ap_get_prof_ap_ssid_internal();
    struct hostapd_data *hapd = NULL;
    struct wpa_auth_config *auth_conf;
    u8 mac[6];
    u16 spp_attrubute = 0;
    u8 pairwise_cipher;

    hapd = (struct hostapd_data *)os_zalloc(sizeof(struct hostapd_data));

    if (hapd == NULL) {
        return NULL;
    }

    hapd->conf = (struct hostapd_bss_config *)os_zalloc(sizeof(struct hostapd_bss_config));

    if (hapd->conf == NULL) {
        os_free(hapd);
        return NULL;
    }

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

    esp_wifi_get_macaddr_internal(WIFI_IF_AP, mac);

    hapd->wpa_auth = wpa_init(mac, auth_conf, NULL);
    esp_wifi_set_appie_internal(WIFI_APPIE_WPA, hapd->wpa_auth->wpa_ie, (uint16_t)hapd->wpa_auth->wpa_ie_len, 0);
    os_free(auth_conf);

    return (void *)hapd;
}

bool hostap_deinit(void *data)
{
    struct hostapd_data *hapd = (struct hostapd_data *)data;

    if (hapd == NULL) {
        return true;
    }

    if (hapd->wpa_auth->wpa_ie != NULL) {
        os_free(hapd->wpa_auth->wpa_ie);
    }

    if (hapd->wpa_auth->group != NULL) {
        os_free(hapd->wpa_auth->group);
    }

    if (hapd->wpa_auth != NULL) {
        os_free(hapd->wpa_auth);
    }

    if (hapd->conf->ssid.wpa_psk != NULL) {
        os_free(hapd->conf->ssid.wpa_psk);
    }

    if (hapd->conf->ssid.wpa_passphrase != NULL) {
        os_free(hapd->conf->ssid.wpa_passphrase);
    }

    if (hapd->conf != NULL) {
        os_free(hapd->conf);
    }

    if (hapd != NULL) {
        os_free(hapd);
    }

    esp_wifi_unset_appie_internal(WIFI_APPIE_WPA);

    return true;
}
