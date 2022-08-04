/*
 * hostapd - WPA/RSN IE and KDE definitions
 * Copyright (c) 2004-2008, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
 
#include "utils/includes.h"
#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "ap/wpa_auth.h"
#include "ap/wpa_auth_ie.h"
#include "ap/wpa_auth_i.h"
#include "common/wpa_common.h"
#include "utils/wpa_debug.h"

#ifdef CONFIG_RSN_TESTING
int rsn_testing = 0;
#endif /* CONFIG_RSN_TESTING */


static int wpa_write_wpa_ie(struct wpa_auth_config *conf, u8 *buf, size_t len)
{
	struct wpa_ie_hdr *hdr;
	int num_suites;
	u8 *pos, *count;
	u32 suite;

	hdr = (struct wpa_ie_hdr *) buf;
	hdr->elem_id = WLAN_EID_VENDOR_SPECIFIC;
	RSN_SELECTOR_PUT(hdr->oui, WPA_OUI_TYPE);
	WPA_PUT_LE16(hdr->version, WPA_VERSION);
	pos = (u8 *) (hdr + 1);

	suite = wpa_cipher_to_suite(WPA_PROTO_WPA, conf->wpa_group);
	if (suite == 0) {
		wpa_printf( MSG_DEBUG, "Invalid group cipher (%d).",
			   conf->wpa_group);
		return -1;
	}
	RSN_SELECTOR_PUT(pos, suite);
	pos += WPA_SELECTOR_LEN;

	count = pos;
	pos += 2;

	num_suites = wpa_cipher_put_suites(pos, conf->wpa_pairwise);
	if (num_suites == 0) {
		wpa_printf( MSG_DEBUG, "Invalid pairwise cipher (%d).",
			   conf->wpa_pairwise);
		return -1;
	}
	pos += num_suites * WPA_SELECTOR_LEN;
	WPA_PUT_LE16(count, num_suites);

	num_suites = 0;
	count = pos;
	pos += 2;

	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
		RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_UNSPEC_802_1X);
		pos += WPA_SELECTOR_LEN;
		num_suites++;
	}
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK) {
		RSN_SELECTOR_PUT(pos, WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X);
		pos += WPA_SELECTOR_LEN;
		num_suites++;
	}

	if (num_suites == 0) {
		wpa_printf( MSG_DEBUG, "Invalid key management type (%d).",
			   conf->wpa_key_mgmt);
		return -1;
	}
	WPA_PUT_LE16(count, num_suites);

	/* WPA Capabilities; use defaults, so no need to include it */

	hdr->len = (pos - buf) - 2;

	return pos - buf;
}


int wpa_write_rsn_ie(struct wpa_auth_config *conf, u8 *buf, size_t len,
		     const u8 *pmkid)
{
	struct rsn_ie_hdr *hdr;
	int num_suites, res;
	u8 *pos, *count;
	u16 capab;
	u32 suite;

	hdr = (struct rsn_ie_hdr *) buf;
	hdr->elem_id = WLAN_EID_RSN;
	WPA_PUT_LE16(hdr->version, RSN_VERSION);
	pos = (u8 *) (hdr + 1);

	suite = wpa_cipher_to_suite(WPA_PROTO_RSN, conf->wpa_group);
	if (suite == 0) {
		wpa_printf( MSG_DEBUG, "Invalid group cipher (%d).",
			   conf->wpa_group);
		return -1;
	}
	RSN_SELECTOR_PUT(pos, suite);
	pos += RSN_SELECTOR_LEN;

	num_suites = 0;
	count = pos;
	pos += 2;

#ifdef CONFIG_RSN_TESTING
	if (rsn_testing) {
		RSN_SELECTOR_PUT(pos, RSN_SELECTOR(0x12, 0x34, 0x56, 1));
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_RSN_TESTING */

	res = rsn_cipher_put_suites(pos, conf->rsn_pairwise);
	num_suites += res;
	pos += res * RSN_SELECTOR_LEN;

#ifdef CONFIG_RSN_TESTING
	if (rsn_testing) {
		RSN_SELECTOR_PUT(pos, RSN_SELECTOR(0x12, 0x34, 0x56, 2));
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_RSN_TESTING */

	if (num_suites == 0) {
		wpa_printf( MSG_DEBUG, "Invalid pairwise cipher (%d).",
			   conf->rsn_pairwise);
		return -1;
	}
	WPA_PUT_LE16(count, num_suites);

	num_suites = 0;
	count = pos;
	pos += 2;

#ifdef CONFIG_RSN_TESTING
	if (rsn_testing) {
		RSN_SELECTOR_PUT(pos, RSN_SELECTOR(0x12, 0x34, 0x56, 1));
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_RSN_TESTING */

	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_UNSPEC_802_1X);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#ifdef CONFIG_IEEE80211R
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_IEEE8021X) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_802_1X);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_PSK) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_PSK);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_802_1X_SHA256);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_PSK_SHA256) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PSK_SHA256);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_SAE
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_SAE) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_SAE);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (conf->wpa_key_mgmt & WPA_KEY_MGMT_FT_SAE) {
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_SAE);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_SAE */

#ifdef CONFIG_RSN_TESTING
	if (rsn_testing) {
		RSN_SELECTOR_PUT(pos, RSN_SELECTOR(0x12, 0x34, 0x56, 2));
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif /* CONFIG_RSN_TESTING */

	if (num_suites == 0) {
		wpa_printf( MSG_DEBUG, "Invalid key management type (%d).",
			   conf->wpa_key_mgmt);
		return -1;
	}
	WPA_PUT_LE16(count, num_suites);

	/* RSN Capabilities */
	capab = 0;
	if (conf->rsn_preauth)
		capab |= WPA_CAPABILITY_PREAUTH;
	if (conf->wmm_enabled) {
		/* 4 PTKSA replay counters when using WMM */
		capab |= (RSN_NUM_REPLAY_COUNTERS_16 << 2);
	}
#ifdef CONFIG_IEEE80211W
	if (conf->ieee80211w != NO_MGMT_FRAME_PROTECTION) {
		capab |= WPA_CAPABILITY_MFPC;
		if (conf->ieee80211w == MGMT_FRAME_PROTECTION_REQUIRED)
			capab |= WPA_CAPABILITY_MFPR;
	}
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_RSN_TESTING
	if (rsn_testing)
		capab |= BIT(8) | BIT(14) | BIT(15);
#endif /* CONFIG_RSN_TESTING */
	WPA_PUT_LE16(pos, capab);
	pos += 2;

	if (pmkid) {
		if (pos + 2 + PMKID_LEN > buf + len)
			return -1;
		/* PMKID Count */
		WPA_PUT_LE16(pos, 1);
		pos += 2;
		memcpy(pos, pmkid, PMKID_LEN);
		pos += PMKID_LEN;
	}

#ifdef CONFIG_IEEE80211W
	if (conf->ieee80211w != NO_MGMT_FRAME_PROTECTION) {
		if (pos + 2 + 4 > buf + len)
			return -1;
		if (pmkid == NULL) {
			/* PMKID Count */
			WPA_PUT_LE16(pos, 0);
			pos += 2;
		}

		/* Management Group Cipher Suite */
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_AES_128_CMAC);
		pos += RSN_SELECTOR_LEN;
	}
#endif /* CONFIG_IEEE80211W */

#ifdef CONFIG_RSN_TESTING
	if (rsn_testing) {
		/*
		 * Fill in any defined fields and add extra data to the end of
		 * the element.
		 */
		int pmkid_count_set = pmkid != NULL;
		if (conf->ieee80211w != NO_MGMT_FRAME_PROTECTION)
			pmkid_count_set = 1;
		/* PMKID Count */
		WPA_PUT_LE16(pos, 0);
		pos += 2;
		if (conf->ieee80211w == NO_MGMT_FRAME_PROTECTION) {
			/* Management Group Cipher Suite */
			RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_AES_128_CMAC);
			pos += RSN_SELECTOR_LEN;
		}

		memset(pos, 0x12, 17);
		pos += 17;
	}
#endif /* CONFIG_RSN_TESTING */

	hdr->len = (pos - buf) - 2;

	return pos - buf;
}


int wpa_auth_gen_wpa_ie(struct wpa_authenticator *wpa_auth)
{
	u8 *pos, buf[128];
	int res;

	pos = buf;

	if (wpa_auth->conf.wpa & WPA_PROTO_RSN) {
		res = wpa_write_rsn_ie(&wpa_auth->conf,
				       pos, buf + sizeof(buf) - pos, NULL);
		if (res < 0)
			return res;
		pos += res;
	}
#ifdef CONFIG_IEEE80211R
	if (wpa_key_mgmt_ft(wpa_auth->conf.wpa_key_mgmt)) {
		res = wpa_write_mdie(&wpa_auth->conf, pos,
				     buf + sizeof(buf) - pos);
		if (res < 0)
			return res;
		pos += res;
	}
#endif /* CONFIG_IEEE80211R */
	if (wpa_auth->conf.wpa & WPA_PROTO_WPA) {
		res = wpa_write_wpa_ie(&wpa_auth->conf,
				       pos, buf + sizeof(buf) - pos);
		if (res < 0)
			return res;
		pos += res;
	}

	os_free(wpa_auth->wpa_ie);
	wpa_auth->wpa_ie = os_malloc(pos - buf);
	if (wpa_auth->wpa_ie == NULL)
		return -1;
	memcpy(wpa_auth->wpa_ie, buf, pos - buf);
	wpa_auth->wpa_ie_len = pos - buf;

	return 0;
}

u8 * wpa_add_kde(u8 *pos, u32 kde, const u8 *data, size_t data_len,
		 const u8 *data2, size_t data2_len)
{
	*pos++ = WLAN_EID_VENDOR_SPECIFIC;
	*pos++ = RSN_SELECTOR_LEN + data_len + data2_len;
	RSN_SELECTOR_PUT(pos, kde);
	pos += RSN_SELECTOR_LEN;
	memcpy(pos, data, data_len);
	pos += data_len;
	if (data2) {
		memcpy(pos, data2, data2_len);
		pos += data2_len;
	}
	return pos;
}

int wpa_validate_wpa_ie(struct wpa_authenticator *wpa_auth,
			struct wpa_state_machine *sm,
			const u8 *wpa_ie, size_t wpa_ie_len/*,
			const u8 *mdie, size_t mdie_len*/)
{
	struct wpa_ie_data data = {0};
	int ciphers, key_mgmt, res, version;
	u32 selector;

	if (wpa_auth == NULL || sm == NULL)
		return WPA_NOT_ENABLED;

	if (wpa_ie == NULL || wpa_ie_len < 1)
		return WPA_INVALID_IE;

	if (wpa_ie[0] == WLAN_EID_RSN)
		version = WPA_PROTO_RSN;
	else
		version = WPA_PROTO_WPA;

	if (!(wpa_auth->conf.wpa & version)) {
		wpa_printf( MSG_DEBUG, "Invalid WPA proto (%d) from " MACSTR,
			   version, MAC2STR(sm->addr));
		return WPA_INVALID_PROTO;
	}

	if (version == WPA_PROTO_RSN) {
		res = wpa_parse_wpa_ie_rsn(wpa_ie, wpa_ie_len, &data);

		selector = RSN_AUTH_KEY_MGMT_UNSPEC_802_1X;
		if (0) {
		}
#ifdef CONFIG_IEEE80211R
		else if (data.key_mgmt & WPA_KEY_MGMT_FT_IEEE8021X)
			selector = RSN_AUTH_KEY_MGMT_FT_802_1X;
		else if (data.key_mgmt & WPA_KEY_MGMT_FT_PSK)
			selector = RSN_AUTH_KEY_MGMT_FT_PSK;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
		else if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256)
			selector = RSN_AUTH_KEY_MGMT_802_1X_SHA256;
		else if (data.key_mgmt & WPA_KEY_MGMT_PSK_SHA256)
			selector = RSN_AUTH_KEY_MGMT_PSK_SHA256;
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_SAE
		else if (data.key_mgmt & WPA_KEY_MGMT_SAE)
			selector = RSN_AUTH_KEY_MGMT_SAE;
		else if (data.key_mgmt & WPA_KEY_MGMT_FT_SAE)
			selector = RSN_AUTH_KEY_MGMT_FT_SAE;
#endif /* CONFIG_SAE */
		else if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X)
			selector = RSN_AUTH_KEY_MGMT_UNSPEC_802_1X;
		else if (data.key_mgmt & WPA_KEY_MGMT_PSK)
			selector = RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X;

		selector = wpa_cipher_to_suite(WPA_PROTO_RSN,
					       data.pairwise_cipher);
		if (!selector)
			selector = RSN_CIPHER_SUITE_CCMP;

		selector = wpa_cipher_to_suite(WPA_PROTO_RSN,
					       data.group_cipher);
		if (!selector)
			selector = RSN_CIPHER_SUITE_CCMP;
	} else {
		res = wpa_parse_wpa_ie_wpa(wpa_ie, wpa_ie_len, &data);

		selector = WPA_AUTH_KEY_MGMT_UNSPEC_802_1X;
		if (data.key_mgmt & WPA_KEY_MGMT_IEEE8021X)
			selector = WPA_AUTH_KEY_MGMT_UNSPEC_802_1X;
		else if (data.key_mgmt & WPA_KEY_MGMT_PSK)
			selector = WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X;

		selector = wpa_cipher_to_suite(WPA_PROTO_WPA,
					       data.pairwise_cipher);
		if (!selector)
			selector = RSN_CIPHER_SUITE_TKIP;

		selector = wpa_cipher_to_suite(WPA_PROTO_WPA,
					       data.group_cipher);
		if (!selector)
			selector = WPA_CIPHER_SUITE_TKIP;
	}
	if (res) {
		wpa_printf( MSG_DEBUG, "Failed to parse WPA/RSN IE from "
			   MACSTR " (res=%d)", MAC2STR(sm->addr), res);
		wpa_hexdump(MSG_DEBUG, "WPA/RSN IE", wpa_ie, wpa_ie_len);
		return WPA_INVALID_IE;
	}

	if (data.group_cipher != wpa_auth->conf.wpa_group) {
		wpa_printf( MSG_DEBUG, "Invalid WPA group cipher (0x%x) from "
			   MACSTR, data.group_cipher, MAC2STR(sm->addr));
		return WPA_INVALID_GROUP;
	}

	key_mgmt = data.key_mgmt & wpa_auth->conf.wpa_key_mgmt;
	if (!key_mgmt) {
		wpa_printf( MSG_DEBUG, "Invalid WPA key mgmt (0x%x) from "
			   MACSTR, data.key_mgmt, MAC2STR(sm->addr));
		return WPA_INVALID_AKMP;
	}
	if (0) {
	}
#ifdef CONFIG_IEEE80211R
	else if (key_mgmt & WPA_KEY_MGMT_FT_IEEE8021X)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_FT_IEEE8021X;
	else if (key_mgmt & WPA_KEY_MGMT_FT_PSK)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_FT_PSK;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
	else if (key_mgmt & WPA_KEY_MGMT_IEEE8021X_SHA256)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_IEEE8021X_SHA256;
	else if (key_mgmt & WPA_KEY_MGMT_PSK_SHA256)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_PSK_SHA256;
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_SAE
	else if (key_mgmt & WPA_KEY_MGMT_SAE)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_SAE;
	else if (key_mgmt & WPA_KEY_MGMT_FT_SAE)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_FT_SAE;
#endif /* CONFIG_SAE */
	else if (key_mgmt & WPA_KEY_MGMT_IEEE8021X)
		sm->wpa_key_mgmt = WPA_KEY_MGMT_IEEE8021X;
	else
		sm->wpa_key_mgmt = WPA_KEY_MGMT_PSK;

	if (version == WPA_PROTO_RSN)
		ciphers = data.pairwise_cipher & wpa_auth->conf.rsn_pairwise;
	else
		ciphers = data.pairwise_cipher & wpa_auth->conf.wpa_pairwise;
	if (!ciphers) {
		wpa_printf( MSG_DEBUG, "Invalid %s pairwise cipher (0x%x) "
			   "from " MACSTR,
			   version == WPA_PROTO_RSN ? "RSN" : "WPA",
			   data.pairwise_cipher, MAC2STR(sm->addr));
		return WPA_INVALID_PAIRWISE;
	}

#ifdef CONFIG_IEEE80211W
	if (wpa_auth->conf.ieee80211w == MGMT_FRAME_PROTECTION_REQUIRED) {
		if (!(data.capabilities & WPA_CAPABILITY_MFPC)) {
			wpa_printf( MSG_DEBUG, "Management frame protection "
				   "required, but client did not enable it");
			return WPA_MGMT_FRAME_PROTECTION_VIOLATION;
		}

		if (ciphers & WPA_CIPHER_TKIP) {
			wpa_printf( MSG_DEBUG, "Management frame protection "
				   "cannot use TKIP");
			return WPA_MGMT_FRAME_PROTECTION_VIOLATION;
		}

		if (data.mgmt_group_cipher != WPA_CIPHER_AES_128_CMAC) {
			wpa_printf( MSG_DEBUG, "Unsupported management group "
				   "cipher %d", data.mgmt_group_cipher);
			return WPA_INVALID_MGMT_GROUP_CIPHER;
		}
	}

	if (wpa_auth->conf.ieee80211w == NO_MGMT_FRAME_PROTECTION ||
	    !(data.capabilities & WPA_CAPABILITY_MFPC))
		sm->mgmt_frame_prot = 0;
	else
		sm->mgmt_frame_prot = 1;
#endif /* CONFIG_IEEE80211W */

#ifdef CONFIG_IEEE80211R
	if (wpa_key_mgmt_ft(sm->wpa_key_mgmt)) {
		if (mdie == NULL || mdie_len < MOBILITY_DOMAIN_ID_LEN + 1) {
			wpa_printf( MSG_DEBUG, "RSN: Trying to use FT, but "
				   "MDIE not included");
			return WPA_INVALID_MDIE;
		}
		if (memcmp(mdie, wpa_auth->conf.mobility_domain,
			      MOBILITY_DOMAIN_ID_LEN) != 0) {
			wpa_hexdump(MSG_DEBUG, "RSN: Attempted to use unknown "
				    "MDIE", mdie, MOBILITY_DOMAIN_ID_LEN);
			return WPA_INVALID_MDIE;
		}
	}
#endif /* CONFIG_IEEE80211R */

	if (ciphers & WPA_CIPHER_CCMP)
		sm->pairwise = WPA_CIPHER_CCMP;
	else if (ciphers & WPA_CIPHER_GCMP)
		sm->pairwise = WPA_CIPHER_GCMP;
	else
		sm->pairwise = WPA_CIPHER_TKIP;

	/* TODO: clear WPA/WPA2 state if STA changes from one to another */
	if (wpa_ie[0] == WLAN_EID_RSN)
		sm->wpa = WPA_VERSION_WPA2;
	else
		sm->wpa = WPA_VERSION_WPA;

	if (sm->wpa_ie == NULL || sm->wpa_ie_len < wpa_ie_len) {
		os_free(sm->wpa_ie);
		sm->wpa_ie = os_malloc(wpa_ie_len);
		if (sm->wpa_ie == NULL)
			return WPA_ALLOC_FAIL;
	}
	memcpy(sm->wpa_ie, wpa_ie, wpa_ie_len);
	sm->wpa_ie_len = wpa_ie_len;

	return WPA_IE_OK;
}

/**
 * wpa_parse_generic - Parse EAPOL-Key Key Data Generic IEs
 * @pos: Pointer to the IE header
 * @end: Pointer to the end of the Key Data buffer
 * @ie: Pointer to parsed IE data
 * Returns: 0 on success, 1 if end mark is found, -1 on failure
 */
static int wpa_parse_generic(const u8 *pos, const u8 *end,
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
		return 0;
	}

	if (pos + 1 + RSN_SELECTOR_LEN < end &&
	    pos[1] >= RSN_SELECTOR_LEN + PMKID_LEN &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_PMKID) {
		ie->pmkid = pos + 2 + RSN_SELECTOR_LEN;
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_GROUPKEY) {
		ie->gtk = pos + 2 + RSN_SELECTOR_LEN;
		ie->gtk_len = pos[1] - RSN_SELECTOR_LEN;
		return 0;
	}

	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_MAC_ADDR) {
		ie->mac_addr = pos + 2 + RSN_SELECTOR_LEN;
		ie->mac_addr_len = pos[1] - RSN_SELECTOR_LEN;
		return 0;
	}

#ifdef CONFIG_IEEE80211W
	if (pos[1] > RSN_SELECTOR_LEN + 2 &&
	    RSN_SELECTOR_GET(pos + 2) == RSN_KEY_DATA_IGTK) {
		ie->igtk = pos + 2 + RSN_SELECTOR_LEN;
		ie->igtk_len = pos[1] - RSN_SELECTOR_LEN;
		return 0;
	}
#endif /* CONFIG_IEEE80211W */

	return 0;
}


/**
 * wpa_parse_kde_ies - Parse EAPOL-Key Key Data IEs
 * @buf: Pointer to the Key Data buffer
 * @len: Key Data Length
 * @ie: Pointer to parsed IE data
 * Returns: 0 on success, -1 on failure
 */
int wpa_parse_kde_ies(const u8 *buf, size_t len, struct wpa_eapol_ie_parse *ie)
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
			wpa_printf( MSG_DEBUG, "WPA: EAPOL-Key Key Data "
				   "underflow (ie=%d len=%d pos=%d)",
				   pos[0], pos[1], (int) (pos - buf));
			wpa_hexdump_key(MSG_DEBUG, "WPA: Key Data",
					buf, len);
			ret = -1;
			break;
		}
		if (*pos == WLAN_EID_RSN) {
			ie->rsn_ie = pos;
			ie->rsn_ie_len = pos[1] + 2;
#ifdef CONFIG_IEEE80211R
		} else if (*pos == WLAN_EID_MOBILITY_DOMAIN) {
			ie->mdie = pos;
			ie->mdie_len = pos[1] + 2;
		} else if (*pos == WLAN_EID_FAST_BSS_TRANSITION) {
			ie->ftie = pos;
			ie->ftie_len = pos[1] + 2;
#endif /* CONFIG_IEEE80211R */
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


int wpa_auth_uses_mfp(struct wpa_state_machine *sm)
{
	return sm ? sm->mgmt_frame_prot : 0;
}
