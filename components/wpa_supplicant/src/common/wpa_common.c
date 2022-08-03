/*
 * WPA/RSN - Shared functions for supplicant and authenticator
 * Copyright (c) 2002-2008, Jouni Malinen <j@w1.fi>
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
#include "common/defs.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_common.h"
#include "rsn_supp/wpa.h"
#include "crypto/sha1.h"
#include "crypto/sha256.h"
#include "crypto/md5.h"
#include "crypto/aes.h"
 
#define MD5_MAC_LEN 16

#ifndef CONFIG_NO_WPA2
static int rsn_selector_to_bitfield(const u8 *s)
{
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_NONE)
		return WPA_CIPHER_NONE;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_WEP40)
		return WPA_CIPHER_WEP40;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_TKIP)
		return WPA_CIPHER_TKIP;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_CCMP)
		return WPA_CIPHER_CCMP;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_WEP104)
		return WPA_CIPHER_WEP104;
#ifdef CONFIG_IEEE80211W
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_AES_128_CMAC)
		return WPA_CIPHER_AES_128_CMAC;
#endif /* CONFIG_IEEE80211W */
	return 0;
}

static int rsn_key_mgmt_to_bitfield(const u8 *s)
{
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_UNSPEC_802_1X)
		return WPA_KEY_MGMT_IEEE8021X;
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X)
		return WPA_KEY_MGMT_PSK;
#ifdef CONFIG_IEEE80211R
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_FT_802_1X)
		return WPA_KEY_MGMT_FT_IEEE8021X;
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_FT_PSK)
		return WPA_KEY_MGMT_FT_PSK;
#endif /* CONFIG_IEEE80211R */
#ifdef CONFIG_IEEE80211W
#ifdef CONFIG_WPA3_SAE
        if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_SAE)
                return WPA_KEY_MGMT_SAE;
#endif /* CONFIG_WPA3_SAE */
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_802_1X_SHA256)
		return WPA_KEY_MGMT_IEEE8021X_SHA256;
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_PSK_SHA256)
		return WPA_KEY_MGMT_PSK_SHA256;
#endif /* CONFIG_IEEE80211W */
	return 0;
}

static int wpa_selector_to_bitfield(const u8 *s)
{
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_NONE)
		return WPA_CIPHER_NONE;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_WEP40)
		return WPA_CIPHER_WEP40;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_TKIP)
		return WPA_CIPHER_TKIP;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_CCMP)
		return WPA_CIPHER_CCMP;
	if (RSN_SELECTOR_GET(s) == WPA_CIPHER_SUITE_WEP104)
		return WPA_CIPHER_WEP104;
	return 0;
}

static int wpa_key_mgmt_to_bitfield(const u8 *s)
{
	if (RSN_SELECTOR_GET(s) == WPA_AUTH_KEY_MGMT_UNSPEC_802_1X)
		return WPA_KEY_MGMT_IEEE8021X;
	if (RSN_SELECTOR_GET(s) == WPA_AUTH_KEY_MGMT_PSK_OVER_802_1X)
		return WPA_KEY_MGMT_PSK;
	if (RSN_SELECTOR_GET(s) == WPA_AUTH_KEY_MGMT_NONE)
		return WPA_KEY_MGMT_WPA_NONE;
	return 0;
}
#endif /* CONFIG_NO_WPA2 */
/**
 * wpa_parse_wpa_ie_rsn - Parse RSN IE
 * @rsn_ie: Buffer containing RSN IE
 * @rsn_ie_len: RSN IE buffer length (including IE number and length octets)
 * @data: Pointer to structure that will be filled in with parsed data
 * Returns: 0 on success, <0 on failure
 */
int wpa_parse_wpa_ie_rsn(const u8 *rsn_ie, size_t rsn_ie_len,
			 struct wpa_ie_data *data)
{
#ifndef CONFIG_NO_WPA2
	const struct rsn_ie_hdr *hdr;
	const u8 *pos;
	int left;
	int i, count;

	memset(data, 0, sizeof(*data));
	data->proto = WPA_PROTO_RSN;
	data->pairwise_cipher = WPA_CIPHER_CCMP;
	data->group_cipher = WPA_CIPHER_CCMP;
	data->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
	data->capabilities = 0;
	data->pmkid = NULL;
	data->num_pmkid = 0;
	data->mgmt_group_cipher = 0;

	if (rsn_ie_len == 0) {
		/* No RSN IE - fail silently */
		return -1;
	}

	if (rsn_ie_len < sizeof(struct rsn_ie_hdr)) {
	    #ifdef DEBUG_PRINT	
		wpa_printf(MSG_DEBUG, "%s: ie len too short %lu",
			   __func__, (unsigned long) rsn_ie_len);
	    #endif	
		return -1;
	}

	hdr = (const struct rsn_ie_hdr *) rsn_ie;

	if (hdr->elem_id != WLAN_EID_RSN ||
	    hdr->len != rsn_ie_len - 2 ||
	    WPA_GET_LE16(hdr->version) != RSN_VERSION) {
    	    #ifdef DEBUG_PRINT	
		wpa_printf(MSG_DEBUG, "%s: malformed ie or unknown version",
			   __func__);
	     #endif		
		return -2;
	}

	pos = (const u8 *) (hdr + 1);
	left = rsn_ie_len - sizeof(*hdr);

	if (left >= RSN_SELECTOR_LEN) {
		data->group_cipher = rsn_selector_to_bitfield(pos);
		pos += RSN_SELECTOR_LEN;
		left -= RSN_SELECTOR_LEN;
	} else if (left > 0) {
	    #ifdef DEBUG_PRINT	
		wpa_printf(MSG_DEBUG, "%s: ie length mismatch, %u too much",
			   __func__, left);
	     #endif	
		return -3;
	}

	if (left >= 2) {
		data->pairwise_cipher = 0;
		count = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
		if (count == 0 || left < count * RSN_SELECTOR_LEN) {
		    #ifdef DEBUG_PRINT		
			wpa_printf(MSG_DEBUG, "%s: ie count botch (pairwise), "
				   "count %u left %u", __func__, count, left);
		    #endif	
			return -4;
		}
		for (i = 0; i < count; i++) {
			data->pairwise_cipher |= rsn_selector_to_bitfield(pos);
			pos += RSN_SELECTOR_LEN;
			left -= RSN_SELECTOR_LEN;
		}
	} else if (left == 1) {
	    #ifdef DEBUG_PRINT	
		wpa_printf(MSG_DEBUG, "%s: ie too short (for key mgmt)",
			   __func__);
	    #endif	
		return -5;
	}

	if (left >= 2) {
		data->key_mgmt = 0;
		count = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
		if (count == 0 || left < count * RSN_SELECTOR_LEN) {
		    #ifdef DEBUG_PRINT		
			wpa_printf(MSG_DEBUG, "%s: ie count botch (key mgmt), "
				   "count %u left %u", __func__, count, left);
		    #endif	
			return -6;
		}
		for (i = 0; i < count; i++) {
			data->key_mgmt |= rsn_key_mgmt_to_bitfield(pos);
			pos += RSN_SELECTOR_LEN;
			left -= RSN_SELECTOR_LEN;
		}
	} else if (left == 1) {
	    #ifdef DEBUG_PRINT	
		wpa_printf(MSG_DEBUG, "%s: ie too short (for capabilities)",
			   __func__);
	    #endif	
		return -7;
	}

	if (left >= 2) {
		data->capabilities = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
	}

	if (left >= 2) {
		data->num_pmkid = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
		if (left < (int) data->num_pmkid * PMKID_LEN) {
		    #ifdef DEBUG_PRINT	
			wpa_printf(MSG_DEBUG, "%s: PMKID underflow "
				   "(num_pmkid=%lu left=%d)",
				   __func__, (unsigned long) data->num_pmkid,
				   left);
		    #endif	
			data->num_pmkid = 0;
			return -9;
		} else {
			data->pmkid = pos;
			pos += data->num_pmkid * PMKID_LEN;
			left -= data->num_pmkid * PMKID_LEN;
		}
	}

	if (left > 0) {
	    #ifdef DEBUG_PRINT	
		wpa_printf(MSG_DEBUG, "%s: ie has %u trailing bytes - ignored",
			   __func__, left);
	    #endif	
	}

	return 0;
#else /* CONFIG_NO_WPA2 */
	return -1;
#endif /* CONFIG_NO_WPA2 */
}

int wpa_parse_wpa_ie_wpa(const u8 *wpa_ie, size_t wpa_ie_len,
			 struct wpa_ie_data *data)
{
	const struct wpa_ie_hdr *hdr;
	const u8 *pos;
	int left;
	int i, count;

	memset(data, 0, sizeof(*data));
	data->proto = WPA_PROTO_WPA;
	data->pairwise_cipher = WPA_CIPHER_TKIP;
	data->group_cipher = WPA_CIPHER_TKIP;
	data->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
	data->capabilities = 0;
	data->pmkid = NULL;
	data->num_pmkid = 0;
	data->mgmt_group_cipher = 0;

	if (wpa_ie_len == 0) {
		/* No WPA IE - fail silently */
		return -1;
	}

	if (wpa_ie_len < sizeof(struct wpa_ie_hdr)) {
		wpa_printf(MSG_DEBUG, "%s: ie len too short %lu",
			   __func__, (unsigned long) wpa_ie_len);
		return -1;
	}

	hdr = (const struct wpa_ie_hdr *) wpa_ie;

	if (hdr->elem_id != WLAN_EID_VENDOR_SPECIFIC ||
	    hdr->len != wpa_ie_len - 2 ||
	    RSN_SELECTOR_GET(hdr->oui) != WPA_OUI_TYPE ||
	    WPA_GET_LE16(hdr->version) != WPA_VERSION) {
		wpa_printf(MSG_DEBUG, "%s: malformed ie or unknown version",
			   __func__);
		return -2;
	}

	pos = (const u8 *) (hdr + 1);
	left = wpa_ie_len - sizeof(*hdr);

	if (left >= WPA_SELECTOR_LEN) {
		data->group_cipher = wpa_selector_to_bitfield(pos);
		pos += WPA_SELECTOR_LEN;
		left -= WPA_SELECTOR_LEN;
	} else if (left > 0) {
		wpa_printf(MSG_DEBUG, "%s: ie length mismatch, %u too much",
			   __func__, left);
		return -3;
	}

	if (left >= 2) {
		data->pairwise_cipher = 0;
		count = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
		if (count == 0 || left < count * WPA_SELECTOR_LEN) {
			wpa_printf(MSG_DEBUG, "%s: ie count botch (pairwise), "
				   "count %u left %u", __func__, count, left);
			return -4;
		}
		for (i = 0; i < count; i++) {
			data->pairwise_cipher |= wpa_selector_to_bitfield(pos);
			pos += WPA_SELECTOR_LEN;
			left -= WPA_SELECTOR_LEN;
		}
	} else if (left == 1) {
		wpa_printf(MSG_DEBUG, "%s: ie too short (for key mgmt)",
			   __func__);
		return -5;
	}

	if (left >= 2) {
		data->key_mgmt = 0;
		count = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
		if (count == 0 || left < count * WPA_SELECTOR_LEN) {
			wpa_printf(MSG_DEBUG, "%s: ie count botch (key mgmt), "
				   "count %u left %u", __func__, count, left);
			return -6;
		}
		for (i = 0; i < count; i++) {
			data->key_mgmt |= wpa_key_mgmt_to_bitfield(pos);
			pos += WPA_SELECTOR_LEN;
			left -= WPA_SELECTOR_LEN;
		}
	} else if (left == 1) {
		wpa_printf(MSG_DEBUG, "%s: ie too short (for capabilities)",
			   __func__);
		return -7;
	}

	if (left >= 2) {
		data->capabilities = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
	}

	if (left > 0) {
		wpa_printf(MSG_DEBUG, "%s: ie has %u trailing bytes - ignored",
			   __func__, left);
	}

	return 0;
}


/**
 * wpa_eapol_key_mic - Calculate EAPOL-Key MIC
 * @key: EAPOL-Key Key Confirmation Key (KCK)
 * @ver: Key descriptor version (WPA_KEY_INFO_TYPE_*)
 * @buf: Pointer to the beginning of the EAPOL header (version field)
 * @len: Length of the EAPOL frame (from EAPOL header to the end of the frame)
 * @mic: Pointer to the buffer to which the EAPOL-Key MIC is written
 * Returns: 0 on success, -1 on failure
 *
 * Calculate EAPOL-Key MIC for an EAPOL-Key packet. The EAPOL-Key MIC field has
 * to be cleared (all zeroes) when calling this function.
 *
 * Note: 'IEEE Std 802.11i-2004 - 8.5.2 EAPOL-Key frames' has an error in the
 * description of the Key MIC calculation. It includes packet data from the
 * beginning of the EAPOL-Key header, not EAPOL header. This incorrect change
 * happened during final editing of the standard and the correct behavior is
 * defined in the last draft (IEEE 802.11i/D10).
 */
int wpa_eapol_key_mic(const u8 *key, int ver, const u8 *buf, size_t len,
		      u8 *mic)
{
	u8 hash[SHA1_MAC_LEN];

	switch (ver) {
	case WPA_KEY_INFO_TYPE_HMAC_MD5_RC4:
		    return hmac_md5(key, 16, buf, len, mic);
	case WPA_KEY_INFO_TYPE_HMAC_SHA1_AES:
		if (hmac_sha1(key, 16, buf, len, hash))
			return -1;
		memcpy(mic, hash, MD5_MAC_LEN);
		break;
#ifdef CONFIG_IEEE80211W
#ifdef CONFIG_WPA3_SAE
       case WPA_KEY_INFO_TYPE_AKM_DEFINED:
#endif /* CONFIG_WPA3_SAE */
	case WPA_KEY_INFO_TYPE_AES_128_CMAC:
		return omac1_aes_128(key, buf, len, mic);
#endif /* CONFIG_IEEE80211W */
	default:
		return -1;
	}

	return 0;
}

int wpa_compare_rsn_ie(int ft_initial_assoc,
		       const u8 *ie1, size_t ie1len,
		       const u8 *ie2, size_t ie2len)
{
	if (ie1 == NULL || ie2 == NULL)
		return -1;

	if (ie1len == ie2len && memcmp(ie1, ie2, ie1len) == 0)
		return 0; /* identical IEs */

#ifdef CONFIG_IEEE80211R
	if (ft_initial_assoc) {
		struct wpa_ie_data ie1d, ie2d;
		/*
		 * The PMKID-List in RSN IE is different between Beacon/Probe
		 * Response/(Re)Association Request frames and EAPOL-Key
		 * messages in FT initial mobility domain association. Allow
		 * for this, but verify that other parts of the RSN IEs are
		 * identical.
		 */
		if (wpa_parse_wpa_ie_rsn(ie1, ie1len, &ie1d) < 0 ||
		    wpa_parse_wpa_ie_rsn(ie2, ie2len, &ie2d) < 0)
			return -1;
		if (ie1d.proto == ie2d.proto &&
		    ie1d.pairwise_cipher == ie2d.pairwise_cipher &&
		    ie1d.group_cipher == ie2d.group_cipher &&
		    ie1d.key_mgmt == ie2d.key_mgmt &&
		    ie1d.capabilities == ie2d.capabilities &&
		    ie1d.mgmt_group_cipher == ie2d.mgmt_group_cipher)
			return 0;
	}
#endif /* CONFIG_IEEE80211R */

	return -1;
}

#ifdef DEBUG_PRINT
/**
 * wpa_cipher_txt - Convert cipher suite to a text string
 * @cipher: Cipher suite (WPA_CIPHER_* enum)
 * Returns: Pointer to a text string of the cipher suite name
 */
const char * wpa_cipher_txt(int cipher)
{
	switch (cipher) {
	case WPA_CIPHER_NONE:
		return "NONE";
	case WPA_CIPHER_WEP40:
		return "WEP-40";
	case WPA_CIPHER_WEP104:
		return "WEP-104";
	case WPA_CIPHER_TKIP:
		return "TKIP";
	case WPA_CIPHER_CCMP:
		return "CCMP";
	case WPA_CIPHER_CCMP | WPA_CIPHER_TKIP:
		return "CCMP+TKIP";
	default:
		return "UNKNOWN";
	}
}
#endif

/**
 * wpa_pmk_to_ptk - Calculate PTK from PMK, addresses, and nonces
 * @pmk: Pairwise master key
 * @pmk_len: Length of PMK
 * @label: Label to use in derivation
 * @addr1: AA or SA
 * @addr2: SA or AA
 * @nonce1: ANonce or SNonce
 * @nonce2: SNonce or ANonce
 * @ptk: Buffer for pairwise transient key
 * @ptk_len: Length of PTK
 * @use_sha256: Whether to use SHA256-based KDF
 *
 * IEEE Std 802.11i-2004 - 8.5.1.2 Pairwise key hierarchy
 * PTK = PRF-X(PMK, "Pairwise key expansion",
 *             Min(AA, SA) || Max(AA, SA) ||
 *             Min(ANonce, SNonce) || Max(ANonce, SNonce))
 */
void wpa_pmk_to_ptk(const u8 *pmk, size_t pmk_len, const char *label,
		    const u8 *addr1, const u8 *addr2,
		    const u8 *nonce1, const u8 *nonce2,
		    u8 *ptk, size_t ptk_len, int use_sha256)
{
	u8 data[2 * ETH_ALEN + 2 * WPA_NONCE_LEN];

	if (memcmp(addr1, addr2, ETH_ALEN) < 0) {
		memcpy(data, addr1, ETH_ALEN);
		memcpy(data + ETH_ALEN, addr2, ETH_ALEN);
	} else {
		memcpy(data, addr2, ETH_ALEN);
		memcpy(data + ETH_ALEN, addr1, ETH_ALEN);
	}

	if (memcmp(nonce1, nonce2, WPA_NONCE_LEN) < 0) {
		memcpy(data + 2 * ETH_ALEN, nonce1, WPA_NONCE_LEN);
		memcpy(data + 2 * ETH_ALEN + WPA_NONCE_LEN, nonce2,
			  WPA_NONCE_LEN);
	} else {
		memcpy(data + 2 * ETH_ALEN, nonce2, WPA_NONCE_LEN);
		memcpy(data + 2 * ETH_ALEN + WPA_NONCE_LEN, nonce1,
			  WPA_NONCE_LEN);
	}

	if (use_sha256) {
		sha256_prf(pmk, pmk_len, label, data, sizeof(data),
			ptk, ptk_len);
	}
	else
	{
	    sha1_prf(pmk, pmk_len, label, data, sizeof(data), ptk, ptk_len);
	}
	wpa_printf(MSG_DEBUG, "WPA: PTK derivation - A1=" MACSTR " A2=" MACSTR"\n",
		   MAC2STR(addr1), MAC2STR(addr2));

	wpa_hexdump(MSG_MSGDUMP, "WPA: PMK", pmk, pmk_len);
	wpa_hexdump(MSG_MSGDUMP, "WPA: PTK", ptk, ptk_len);
}

/**
 * rsn_pmkid - Calculate PMK identifier
 * @pmk: Pairwise master key
 * @pmk_len: Length of pmk in bytes
 * @aa: Authenticator address
 * @spa: Supplicant address
 * @pmkid: Buffer for PMKID
 * @use_sha256: Whether to use SHA256-based KDF
 *
 * IEEE Std 802.11i-2004 - 8.5.1.2 Pairwise key hierarchy
 * PMKID = HMAC-SHA1-128(PMK, "PMK Name" || AA || SPA)
 */
void rsn_pmkid(const u8 *pmk, size_t pmk_len, const u8 *aa, const u8 *spa,
	       u8 *pmkid, int use_sha256)
{
	char title[9];
	const u8 *addr[3];
	const size_t len[3] = { 8, ETH_ALEN, ETH_ALEN };
	unsigned char hash[SHA256_MAC_LEN];

    os_memcpy(title, "PMK Name", sizeof("PMK Name"));
	addr[0] = (u8 *) title;
	addr[1] = aa;
	addr[2] = spa;

#ifdef CONFIG_IEEE80211W
	if (use_sha256) {
		hmac_sha256_vector(pmk, pmk_len, 3, addr, len, hash);
	}
	else
#endif /* CONFIG_IEEE80211W */
	hmac_sha1_vector(pmk, pmk_len, 3, addr, len, hash);
	memcpy(pmkid, hash, PMKID_LEN);
}

int wpa_cipher_key_len(int cipher)
{
	switch (cipher) {
	case WPA_CIPHER_CCMP:
	case WPA_CIPHER_GCMP:
		return 16;
	case WPA_CIPHER_TKIP:
		return 32;
	case WPA_CIPHER_WEP104:
		return 13;
	case WPA_CIPHER_WEP40:
		return 5;
	}

	return 0;
}

int wpa_cipher_to_alg(int cipher)
{
	switch (cipher) {
	case WPA_CIPHER_CCMP:
		return WPA_ALG_CCMP;
	case WPA_CIPHER_GCMP:
		return WPA_ALG_GCMP;
	case WPA_CIPHER_TKIP:
		return WPA_ALG_TKIP;
	case WPA_CIPHER_WEP104:
	case WPA_CIPHER_WEP40:
		return WPA_ALG_WEP;
	}
	return WPA_ALG_NONE;
}

u32 wpa_cipher_to_suite(int proto, int cipher)
{
	if (cipher & WPA_CIPHER_CCMP)
		return (proto == WPA_PROTO_RSN ?
			RSN_CIPHER_SUITE_CCMP : WPA_CIPHER_SUITE_CCMP);
	if (cipher & WPA_CIPHER_GCMP)
		return RSN_CIPHER_SUITE_GCMP;
	if (cipher & WPA_CIPHER_TKIP)
		return (proto == WPA_PROTO_RSN ?
			RSN_CIPHER_SUITE_TKIP : WPA_CIPHER_SUITE_TKIP);
	if (cipher & WPA_CIPHER_WEP104)
		return (proto == WPA_PROTO_RSN ?
			RSN_CIPHER_SUITE_WEP104 : WPA_CIPHER_SUITE_WEP104);
	if (cipher & WPA_CIPHER_WEP40)
		return (proto == WPA_PROTO_RSN ?
			RSN_CIPHER_SUITE_WEP40 : WPA_CIPHER_SUITE_WEP40);
	if (cipher & WPA_CIPHER_NONE)
		return (proto == WPA_PROTO_RSN ?
			RSN_CIPHER_SUITE_NONE : WPA_CIPHER_SUITE_NONE);
	return 0;
}

int rsn_cipher_put_suites(u8 *pos, int ciphers)
{
	int num_suites = 0;

	if (ciphers & WPA_CIPHER_CCMP) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_CCMP);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (ciphers & WPA_CIPHER_GCMP) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_GCMP);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (ciphers & WPA_CIPHER_TKIP) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_TKIP);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (ciphers & WPA_CIPHER_NONE) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_NONE);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}

	return num_suites;
}

int wpa_cipher_put_suites(u8 *pos, int ciphers)
{
	int num_suites = 0;

	if (ciphers & WPA_CIPHER_CCMP) {
		RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_CCMP);
		pos += WPA_SELECTOR_LEN;
		num_suites++;
	}
	if (ciphers & WPA_CIPHER_TKIP) {
		RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_TKIP);
		pos += WPA_SELECTOR_LEN;
		num_suites++;
	}
	if (ciphers & WPA_CIPHER_NONE) {
		RSN_SELECTOR_PUT(pos, WPA_CIPHER_SUITE_NONE);
		pos += WPA_SELECTOR_LEN;
		num_suites++;
	}

	return num_suites;
}

unsigned int wpa_mic_len(int akmp)
{
    /* The following code is supposed to be used for 192 bit encryption support only
	if (akmp == WPA_KEY_MGMT_IEEE8021X_SUITE_B_192)
		return 24;
    */

	return 16;
}

#endif // ESP_SUPPLICANT


