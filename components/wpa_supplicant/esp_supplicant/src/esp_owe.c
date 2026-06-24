/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crypto/crypto.h"
#include "esp_owe_i.h"
#include "rsn_supp/wpa.h"

#ifdef CONFIG_OWE_SOFTAP
#include "ap/hostapd.h"
#include "ap/sta_info.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_i.h"
#include "common/ieee802_11_defs.h"
#endif

#ifdef CONFIG_OWE_STA
uint8_t *owe_build_dhie(uint16_t group)
{
    struct wpa_sm *sm = NULL;
    sm = get_wpa_sm();
    return (uint8_t *)(owe_build_assoc_req(sm, group));
}

void owe_deinit(void)
{
    struct wpa_sm *sm;
    sm = get_wpa_sm();
    if (sm->key_mgmt == WPA_KEY_MGMT_OWE) {
        if (sm->owe_ie) {
            wpabuf_free(sm->owe_ie);
            sm->owe_ie = NULL;
        }
        crypto_ecdh_deinit(sm->owe_ecdh);
        sm->owe_ecdh = NULL;
    }
}

void esp_wifi_register_owe_cb(struct wpa_funcs *wpa_cb)
{
    wpa_cb->owe_build_dhie = owe_build_dhie;
    wpa_cb->owe_process_assoc_resp = owe_process_assoc_resp;
}
#endif /* CONFIG_OWE_STA */

#ifdef CONFIG_OWE_SOFTAP
struct wpabuf *esp_owe_build_assoc_resp_dhie(struct hostapd_data *hapd, const u8 *bssid, int *owe_ie_len)
{

    if (!hapd || !hapd->wpa_auth || !hapd->wpa_auth->wpa_ie) {
        wpa_printf(MSG_ERROR, "Invalid hapd or WPA auth data");
        return NULL;
    }

    struct wpabuf *pub;
    struct sta_info *sta = ap_get_sta(hapd, bssid);
    if (!sta) {
        return NULL;
    }

    struct wpabuf *owe_buf = wpabuf_alloc(OWE_IE_INIT_LEN);
    if (!owe_buf) {
        wpa_printf(MSG_ERROR, "Memory allocation failed for OWE IE");
        return NULL;
    }

    // If PMKSA caching is used, write and return only RSN IE with PMKID
    if (sta->wpa_sm && sta->wpa_sm->pmksa) {
        u8 *pos, buf[257];
        pos = buf;

        wpa_printf(MSG_DEBUG, "OWE: Using PMKSA caching for Assoc Resp");
        pos = wpa_auth_write_assoc_resp_owe(hapd, sta->wpa_sm, pos,
                                            buf + sizeof(buf) - pos);

        if (wpabuf_resize(&owe_buf, pos - buf) < 0) {
            wpa_printf(MSG_ERROR, "OWE: wpabuf_resize failed for PMKSA assoc resp");
            wpabuf_free(owe_buf);
            *owe_ie_len = 0;
            return NULL;
        }
        wpabuf_put_data(owe_buf, buf, pos - buf);
        *owe_ie_len = pos - buf;
        return owe_buf;
    }

    if (sta->owe_ecdh) {
        if (!sta->wpa_sm) {
            wpa_printf(MSG_ERROR, "OWE: Missing WPA state machine for assoc resp");
            wpabuf_free(owe_buf);
            *owe_ie_len = 0;
            return NULL;
        }

        u8 buf[257];
        u8 *pos = buf;

        pos = wpa_auth_write_assoc_resp_owe(hapd, sta->wpa_sm, pos,
                                            buf + sizeof(buf) - pos);
        size_t rsne_len = (size_t)(pos - buf);

        if (rsne_len == 0 || pos > buf + sizeof(buf)) {
            wpa_printf(MSG_ERROR, "OWE: Failed to write RSN IE for assoc resp");
            wpabuf_free(owe_buf);
            *owe_ie_len = 0;
            return NULL;
        }

        pub = crypto_ecdh_get_pubkey(sta->owe_ecdh, 0);
        if (!pub) {
            wpabuf_free(owe_buf);
            *owe_ie_len = 0;
            return NULL;
        }

        wpa_hexdump_buf(MSG_DEBUG, "Own public key", pub);

        size_t dh_len = 5 + wpabuf_len(pub);

        if (wpabuf_resize(&owe_buf, rsne_len + dh_len) < 0) {
            wpa_printf(MSG_ERROR, "OWE: wpabuf_resize failed for assoc resp IEs");
            wpabuf_free(pub);
            wpabuf_free(owe_buf);
            *owe_ie_len = 0;
            return NULL;
        }
        wpabuf_put_data(owe_buf, buf, rsne_len);
        wpabuf_put_u8(owe_buf, WLAN_EID_EXTENSION);
        wpabuf_put_u8(owe_buf, 1 + 2 + wpabuf_len(pub));
        wpabuf_put_u8(owe_buf, WLAN_EID_EXT_OWE_DH_PARAM);
        wpabuf_put_le16(owe_buf, IANA_SECP256R1);
        wpabuf_put_buf(owe_buf, pub);
        wpabuf_free(pub);

        wpa_hexdump_buf(MSG_DEBUG, "OWE: Buffer", owe_buf);
    }
    *owe_ie_len = wpabuf_len(owe_buf);

    return owe_buf;
}
#endif /* CONFIG_OWE_SOFTAP */
