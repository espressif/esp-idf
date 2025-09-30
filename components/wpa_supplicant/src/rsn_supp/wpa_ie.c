/*
 * wpa_supplicant - WPA/RSN IE and KDE processing
 * Copyright (c) 2003-2008, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */
 #ifdef ESP_SUPPLICANT

#include "utils/includes.h"

#include "utils/common.h"
#include "rsn_supp/wpa.h"
#include "common/ieee802_11_defs.h"
#include "rsn_supp/wpa_ie.h"
#include "rsn_supp/pmksa_cache.h"

/**
 * wpa_parse_wpa_ie - Parse WPA/RSN IE
 * @wpa_ie: Pointer to WPA or RSN IE
 * @wpa_ie_len: Length of the WPA/RSN IE
 * @data: Pointer to data area for parsing results
 * Returns: 0 on success, -1 on failure
 *
 * Parse the contents of WPA or RSN IE and write the parsed data into data.
 */
int  wpa_parse_wpa_ie(const u8 *wpa_ie, size_t wpa_ie_len,
		     struct wpa_ie_data *data)
{
    if (wpa_ie_len >= 1 && wpa_ie[0] == WLAN_EID_RSN) {
        return wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
    } else if (wpa_ie_len >=1 && wpa_ie[0] == WLAN_EID_RSNX){
        return wpa_parse_wpa_ie_rsnxe(wpa_ie, wpa_ie_len, data);
    } else if (wpa_ie[0] == WLAN_EID_WAPI) {
        return 0;
#ifdef CONFIG_WPA3_COMPAT
    } else if (wpa_ie_len >= 6 && wpa_ie[0] == WLAN_EID_VENDOR_SPECIFIC &&
               wpa_ie[1] >= 4 &&
               WPA_GET_BE32(&wpa_ie[2]) == RSNE_OVERRIDE_IE_VENDOR_TYPE) {
        return wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
    } else if (wpa_ie_len >= 6 && wpa_ie[0] == WLAN_EID_VENDOR_SPECIFIC &&
               wpa_ie[1] >= 4 &&
               WPA_GET_BE32(&wpa_ie[2]) == RSNXE_OVERRIDE_IE_VENDOR_TYPE) {
        return wpa_parse_wpa_ie_rsnxe(wpa_ie, wpa_ie_len, data);
#endif
    }

    return wpa_parse_wpa_ie_wpa(wpa_ie, wpa_ie_len, data);
}


static int  wpa_gen_wpa_ie_wpa(u8 *wpa_ie, size_t wpa_ie_len,
                  int pairwise_cipher, int group_cipher,
                  int key_mgmt)
{
    u8 *pos;
    struct wpa_ie_hdr *hdr;
    u32 suite;

    if (wpa_ie_len < sizeof(*hdr) + WPA_SELECTOR_LEN +
        2 + WPA_SELECTOR_LEN + 2 + WPA_SELECTOR_LEN)
        return -1;

    hdr = (struct wpa_ie_hdr *) wpa_ie;
    hdr->elem_id = WLAN_EID_VENDOR_SPECIFIC;
    RSN_SELECTOR_PUT(hdr->oui, WPA_OUI_TYPE);
    WPA_PUT_LE16(hdr->version, WPA_VERSION);
    pos = (u8 *) (hdr + 1);

    suite = wpa_cipher_to_suite(WPA_PROTO_WPA, group_cipher);
    if (suite == 0) {
        wpa_printf(MSG_WARNING, "Invalid group cipher (%d).",
               group_cipher);
        return -1;
    }
    RSN_SELECTOR_PUT(pos, suite);
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    suite = wpa_cipher_to_suite(WPA_PROTO_WPA, pairwise_cipher);
    if (suite == 0 ||
        (!wpa_cipher_valid_pairwise(pairwise_cipher) &&
         pairwise_cipher != WPA_CIPHER_NONE)) {
        wpa_printf(MSG_WARNING, "Invalid pairwise cipher (%d).",
               pairwise_cipher);
        return -1;
    }
    RSN_SELECTOR_PUT(pos, suite);
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_UNSPEC_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_PSK) {
        RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_WPA_NONE) {
        RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_NONE);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid key management type (%d).",
               key_mgmt);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    /* WPA Capabilities; use defaults, so no need to include it */

    hdr->len = (pos - wpa_ie) - 2;

    WPA_ASSERT((size_t) (pos - wpa_ie) <= wpa_ie_len);

    return pos - wpa_ie;
}


static int wpa_gen_wpa_ie_rsn(u8 *rsn_ie, size_t rsn_ie_len,
                  int pairwise_cipher, int group_cipher,
                  int key_mgmt, int mgmt_group_cipher,
                  struct wpa_sm *sm)
{
    u8 *pos;
    struct rsn_ie_hdr *hdr;
    u16 capab;
    u8 min_len = 0;
    u32 suite;

    if (rsn_ie_len < sizeof(*hdr) + RSN_SELECTOR_LEN +
            2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2 +
            (sm->cur_pmksa ? 2 + PMKID_LEN : 0)) {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)",
                (unsigned long) rsn_ie_len);
        return -1;
    }

    if ( (sm->ap_notify_completed_rsne == true) || (key_mgmt == WPA_KEY_MGMT_IEEE8021X) ) {
        min_len = sizeof(*hdr) + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2;
    } else {
        min_len = sizeof(*hdr) + RSN_SELECTOR_LEN;
    }

    if (rsn_ie_len < min_len) {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)", (unsigned long) rsn_ie_len);
    }

    hdr = (struct rsn_ie_hdr *) rsn_ie;
    hdr->elem_id = WLAN_EID_RSN;
    WPA_PUT_LE16(hdr->version, RSN_VERSION);
    pos = (u8 *) (hdr + 1);

    suite = wpa_cipher_to_suite(WPA_PROTO_RSN, group_cipher);
    if (suite == 0) {
            wpa_printf(MSG_WARNING, "Invalid group cipher (%d).",
                       group_cipher);
            return -1;
    }
    RSN_SELECTOR_PUT(pos, suite);
    pos += RSN_SELECTOR_LEN;

    if ( (sm->ap_notify_completed_rsne == false) && (key_mgmt != WPA_KEY_MGMT_IEEE8021X) ) {
        hdr->len = (pos - rsn_ie) - 2;
        return (pos - rsn_ie);
    }

    *pos++ = 1;
    *pos++ = 0;
    suite = wpa_cipher_to_suite(WPA_PROTO_RSN, pairwise_cipher);
    if (suite == 0 ||
         (pairwise_cipher == WPA_CIPHER_NONE)) {
            wpa_printf(MSG_WARNING, "Invalid pairwise cipher (%d).",
                       pairwise_cipher);
            return -1;
    }
    RSN_SELECTOR_PUT(pos, suite);
    pos += RSN_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;

    if (key_mgmt == WPA_KEY_MGMT_IEEE8021X) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_UNSPEC_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_PSK) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X);
#ifdef CONFIG_IEEE80211R
    } else if (key_mgmt == WPA_KEY_MGMT_FT_IEEE8021X) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_802_1X);
    } else if (key_mgmt == WPA_KEY_MGMT_FT_PSK) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_PSK);
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
    } else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SHA256) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SHA256);
    } else if (key_mgmt == WPA_KEY_MGMT_PSK_SHA256) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PSK_SHA256);
#ifdef CONFIG_WPA3_SAE
    } else if (key_mgmt == WPA_KEY_MGMT_SAE) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_SAE);
    } else if (key_mgmt == WPA_KEY_MGMT_SAE_EXT_KEY) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_SAE_EXT_KEY);
    } else if (key_mgmt == WPA_KEY_MGMT_FT_SAE) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_SAE);
#endif /* CONFIG_WPA3_SAE */
#ifdef CONFIG_OWE_STA
    } else if (key_mgmt == WPA_KEY_MGMT_OWE) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_OWE);
#endif /* CONFIG_OWE_STA */
#ifdef CONFIG_DPP
    } else if (key_mgmt & WPA_KEY_MGMT_DPP) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_DPP);
#endif /* CONFIG_DPP */
#endif /* CONFIG_IEEE80211W */
    } else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SUITE_B_192) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SUITE_B_192);
    } else if (key_mgmt == WPA_KEY_MGMT_IEEE8021X_SUITE_B) {
        RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SUITE_B);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid key management type (%d).",
               key_mgmt);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    /* RSN Capabilities */
    capab = 0;
#ifdef CONFIG_IEEE80211W
    if (sm->pmf_cfg.capable) {
        capab |= WPA_CAPABILITY_MFPC;
        if (sm->pmf_cfg.required || key_mgmt == WPA_KEY_MGMT_SAE || key_mgmt == WPA_KEY_MGMT_OWE) {
            capab |= WPA_CAPABILITY_MFPR;
        }
    }
#endif /* CONFIG_IEEE80211W */

    if (sm->spp_sup.capable) {
        capab |= WPA_CAPABILITY_SPP_CAPABLE;
    }

    if (sm->spp_sup.require) {
        capab |= WPA_CAPABILITY_SPP_REQUIRED;
    }

    WPA_PUT_LE16(pos, capab);
    pos += 2;

    if (sm->cur_pmksa) {
        /* PMKID Count (2 octets, little endian) */
        *pos++ = 1;
        *pos++ = 0;
        /* PMKID */
        os_memcpy(pos, sm->cur_pmksa->pmkid, PMKID_LEN);
        pos += PMKID_LEN;
    }

#ifdef CONFIG_IEEE80211W
    if (wpa_cipher_valid_mgmt_group(mgmt_group_cipher)) {
        if (!sm->cur_pmksa) {
            /* 0 PMKID Count */
            WPA_PUT_LE16(pos, 0);
            pos += 2;
        }

        /* Management Group Cipher Suite */
        RSN_SELECTOR_PUT(pos, wpa_cipher_to_suite(WPA_PROTO_RSN,
                                                  mgmt_group_cipher));;
        pos += RSN_SELECTOR_LEN;
    }
#endif /* CONFIG_IEEE80211W */

    hdr->len = (pos - rsn_ie) - 2;

    WPA_ASSERT((size_t) (pos - rsn_ie) <= rsn_ie_len);

    return pos - rsn_ie;
}


/**
 * wpa_gen_wpa_ie - Generate WPA/RSN IE based on current security policy
 * @sm: Pointer to WPA state machine data from wpa_sm_init()
 * @wpa_ie: Pointer to memory area for the generated WPA/RSN IE
 * @wpa_ie_len: Maximum length of the generated WPA/RSN IE
 * Returns: Length of the generated WPA/RSN IE or -1 on failure
 */
int  wpa_gen_wpa_ie(struct wpa_sm *sm, u8 *wpa_ie, size_t wpa_ie_len)
{
    if (sm->proto == WPA_PROTO_RSN) {
        return wpa_gen_wpa_ie_rsn(wpa_ie, wpa_ie_len,
                      sm->pairwise_cipher,
                      sm->group_cipher,
                      sm->key_mgmt, sm->mgmt_group_cipher,
                      sm);
    } else if (sm->proto == WPA_PROTO_WAPI) {
        return 0;
    }

    return wpa_gen_wpa_ie_wpa(wpa_ie, wpa_ie_len,
                    sm->pairwise_cipher,
                    sm->group_cipher,
                    sm->key_mgmt);
}


int wpa_gen_rsnxe(struct wpa_sm *sm, u8 *rsnxe, size_t rsnxe_len)
{
    u8 *pos = rsnxe;
    u32 capab = 0, tmp;
    size_t flen;

    if (wpa_key_mgmt_sae(sm->key_mgmt) &&
        (sm->sae_pwe == SAE_PWE_HASH_TO_ELEMENT ||
         sm->sae_pwe == SAE_PWE_BOTH)) {
        capab |= BIT(WLAN_RSNX_CAPAB_SAE_H2E);
#ifdef CONFIG_SAE_PK
        if (sm->sae_pk) {
            capab |= BIT(WLAN_RSNX_CAPAB_SAE_PK);
        }
#endif /* CONFIG_SAE_PK */
    }

    if (!capab)
        return 0; /* no supported extended RSN capabilities */

    tmp = capab;
    flen = 0;
    while (tmp) {
        flen++;
        tmp >>= 8;
    }
    if (rsnxe_len < 2 + flen)
        return -1;
    capab |= flen - 1; /* bit 0-3 = Field length (n - 1) */

    *pos++ = WLAN_EID_RSNX;
    *pos++ = flen;
    while (capab) {
        *pos++ = capab & 0xff;
        capab >>= 8;
    }

    return pos - rsnxe;
}
#endif // ESP_SUPPLICANT
