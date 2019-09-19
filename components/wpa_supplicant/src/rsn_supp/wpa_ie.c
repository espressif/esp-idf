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
	if (wpa_ie_len >= 1 && wpa_ie[0] == WLAN_EID_RSN)
		return wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, data);
	else
		return wpa_parse_wpa_ie_wpa(wpa_ie, wpa_ie_len, data);
}


static int  wpa_gen_wpa_ie_wpa(u8 *wpa_ie, size_t wpa_ie_len,
                  int pairwise_cipher, int group_cipher,
                  int key_mgmt)
{
    u8 *pos;
    struct wpa_ie_hdr *hdr;

    if (wpa_ie_len < sizeof(*hdr) + WPA_SELECTOR_LEN +
        2 + WPA_SELECTOR_LEN + 2 + WPA_SELECTOR_LEN)
        return -1;

    hdr = (struct wpa_ie_hdr *) wpa_ie;
    hdr->elem_id = WLAN_EID_VENDOR_SPECIFIC;
    RSN_SELECTOR_PUT(hdr->oui, WPA_OUI_TYPE);
    WPA_PUT_LE16(hdr->version, WPA_VERSION);
    pos = (u8 *) (hdr + 1);

    if (group_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_CCMP);
    } else if (group_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_TKIP);
    } else if (group_cipher == WPA_CIPHER_WEP104) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_WEP104);
    } else if (group_cipher == WPA_CIPHER_WEP40) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_WEP40);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid group cipher (%d).",
               group_cipher);
        return -1;
    }
    pos += WPA_SELECTOR_LEN;

    *pos++ = 1;
    *pos++ = 0;
    if (pairwise_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_CCMP);
    } else if (pairwise_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_TKIP);
    } else if (pairwise_cipher == WPA_CIPHER_NONE) {
        RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_NONE);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid pairwise cipher (%d).",
               pairwise_cipher);
        return -1;
    }
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


static int  wpa_gen_wpa_ie_rsn(u8 *rsn_ie, size_t rsn_ie_len,
                  int pairwise_cipher, int group_cipher,
                  int key_mgmt, int mgmt_group_cipher,
                  struct wpa_sm *sm)
{
#ifndef CONFIG_NO_WPA2
    u8 *pos;
    struct rsn_ie_hdr *hdr;
    u16 capab;
    u8 min_len = 0;

    if (rsn_ie_len < sizeof(*hdr) + RSN_SELECTOR_LEN +
            2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN + 2 +
            (sm->cur_pmksa ? 2 + PMKID_LEN : 0)) {
        wpa_printf(MSG_DEBUG, "RSN: Too short IE buffer (%lu bytes)",
                (unsigned long) rsn_ie_len);
        return -1;
    }

    /* For WPA2-PSK, if the RSNE in AP beacon/probe response doesn't specify the 
     * pairwise cipher or AKM suite, the RSNE IE in association request
     * should only contain group cihpher suite, otherwise the WPA2 improvements 
     * certification will fail.
     */
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

    if (group_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_CCMP);
    } else if (group_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_TKIP);
    } else if (group_cipher == WPA_CIPHER_WEP104) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_WEP104);
    } else if (group_cipher == WPA_CIPHER_WEP40) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_WEP40);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid group cipher (%d).",
               group_cipher);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    if ( (sm->ap_notify_completed_rsne == false) && (key_mgmt != WPA_KEY_MGMT_IEEE8021X) ) {
        hdr->len = (pos - rsn_ie) - 2;
        return (pos - rsn_ie);
    }

    *pos++ = 1;
    *pos++ = 0;
    if (pairwise_cipher == WPA_CIPHER_CCMP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_CCMP);
    } else if (pairwise_cipher == WPA_CIPHER_TKIP) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_TKIP);
    } else if (pairwise_cipher == WPA_CIPHER_NONE) {
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_NONE);
    } else {
        wpa_printf(MSG_DEBUG, "Invalid pairwise cipher (%d).",
               pairwise_cipher);
        return -1;
    }
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
#endif /* CONFIG_IEEE80211W */
    } else {
        wpa_printf(MSG_DEBUG, "Invalid key management type (%d).",
               key_mgmt);
        return -1;
    }
    pos += RSN_SELECTOR_LEN;

    /* RSN Capabilities */
    capab = 0;
#ifdef CONFIG_IEEE80211W
    if (sm->mfp)
        capab |= WPA_CAPABILITY_MFPC;
    if (sm->mfp == 2)
        capab |= WPA_CAPABILITY_MFPR;
#endif /* CONFIG_IEEE80211W */
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
    if (mgmt_group_cipher == WPA_CIPHER_AES_128_CMAC) {
        if (!sm->cur_pmksa) {
            /* PMKID Count */
            WPA_PUT_LE16(pos, 0);
            pos += 2;
        }

        /* Management Group Cipher Suite */
        RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_AES_128_CMAC);
        pos += RSN_SELECTOR_LEN;
    }
#endif /* CONFIG_IEEE80211W */

    hdr->len = (pos - rsn_ie) - 2;

    WPA_ASSERT((size_t) (pos - rsn_ie) <= rsn_ie_len);

    return pos - rsn_ie;
#else /* CONFIG_NO_WPA2 */
    return -1;
#endif /* CONFIG_NO_WPA2 */
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
    if (sm->proto == WPA_PROTO_RSN)
        return wpa_gen_wpa_ie_rsn(wpa_ie, wpa_ie_len,
                      sm->pairwise_cipher,
                      sm->group_cipher,
                      sm->key_mgmt, sm->mgmt_group_cipher,
                      sm);
    else
        return wpa_gen_wpa_ie_wpa(wpa_ie, wpa_ie_len,
                      sm->pairwise_cipher,
                      sm->group_cipher,
                      sm->key_mgmt);
}


/**
 * wpa_parse_generic - Parse EAPOL-Key Key Data Generic IEs
 * @pos: Pointer to the IE header
 * @end: Pointer to the end of the Key Data buffer
 * @ie: Pointer to parsed IE data
 * Returns: 0 on success, 1 if end mark is found, -1 on failure
 */
static int  wpa_parse_generic(const u8 *pos, const u8 *end,
			     struct wpa_eapol_ie_parse *ie)
{
	if (pos[1] == 0)
		return 1;

	if (pos[1] >= 6 &&
	    RSN_SELECTOR_GET(pos + 2) == WPA_OUI_TYPE &&
	    pos[2 + WPA_SELECTOR_LEN] == 1 &&
	    pos[2 + WPA_SELECTOR_LEN + 1] == 0) {
		ie->wpa_ie = pos;
		ie->wpa_ie_len = pos[1] + 2;
		wpa_hexdump(MSG_DEBUG, "WPA: WPA IE in EAPOL-Key",
			    ie->wpa_ie, ie->wpa_ie_len);
		return 0;
	}

	if (pos + 1 + RSN_SELECTOR_LEN < end &&
	    pos[1] >= RSN_SELECTOR_LEN + PMKID_LEN &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_PMKID) {
		ie->pmkid = pos + 2 + RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "WPA: PMKID in EAPOL-Key",
			    pos, pos[1] + 2);
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_GROUPKEY) {
		ie->gtk = pos + 2 + RSN_SELECTOR_LEN;
		ie->gtk_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "WPA: GTK in EAPOL-Key",
				pos, pos[1] + 2);
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_MAC_ADDR) {
		ie->mac_addr = pos + 2 + RSN_SELECTOR_LEN;
		ie->mac_addr_len = pos[1] - RSN_SELECTOR_LEN;
		wpa_hexdump(MSG_DEBUG, "WPA: MAC Address in EAPOL-Key",
			    pos, pos[1] + 2);
		return 0;
	}
	return 0;
}


/**
 * wpa_supplicant_parse_ies - Parse EAPOL-Key Key Data IEs
 * @buf: Pointer to the Key Data buffer
 * @len: Key Data Length
 * @ie: Pointer to parsed IE data
 * Returns: 0 on success, -1 on failure
 */
int  wpa_supplicant_parse_ies(const u8 *buf, size_t len,
			     struct wpa_eapol_ie_parse *ie)
{
	const u8 *pos, *end;
	int ret = 0;

	memset(ie, 0, sizeof(*ie));
	for (pos = buf, end = pos + len; pos + 1 < end; pos += 2 + pos[1]) {
		if (pos[0] == 0xdd &&
		    ((pos == buf + len - 1) || pos[1] == 0)) {
			/* Ignore padding */
			break;
		}
		if (pos + 2 + pos[1] > end) {
		    #ifdef DEBUG_PRINT	
			wpa_printf(MSG_DEBUG, "WPA: EAPOL-Key Key Data "
				   "underflow (ie=%d len=%d pos=%d)",
				   pos[0], pos[1], (int) (pos - buf));
		    #endif	
			wpa_hexdump(MSG_DEBUG, "WPA: Key Data",
					buf, len);
			ret = -1;
			break;
		}
		if (*pos == WLAN_EID_RSN) {
			ie->rsn_ie = pos;
			ie->rsn_ie_len = pos[1] + 2;
			wpa_hexdump(MSG_DEBUG, "WPA: RSN IE in EAPOL-Key",
				    ie->rsn_ie, ie->rsn_ie_len);
		} else if (*pos == WLAN_EID_VENDOR_SPECIFIC) {
			ret = wpa_parse_generic(pos, end, ie);
			if (ret < 0)
				break;
			if (ret > 0) {
				ret = 0;
				break;
			}
		} else {
			wpa_hexdump(MSG_DEBUG, "WPA: Unrecognized EAPOL-Key "
				    "Key Data IE", pos, 2 + pos[1]);
		}
	}

	return ret;
}


#endif // ESP_SUPPLICANT

