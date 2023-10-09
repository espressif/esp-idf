/*
 * WPA/RSN - Shared functions for supplicant and authenticator
 * Copyright (c) 2002-2018, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
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
#include "crypto/sha384.h"
#include "crypto/md5.h"
#include "crypto/aes.h"
#include "crypto/aes_wrap.h"
#include "crypto/crypto.h"

#define MD5_MAC_LEN 16
#ifdef CONFIG_IEEE80211R
int wpa_ft_mic(const u8 *kck, size_t kck_len, const u8 *sta_addr,
               const u8 *ap_addr, u8 transaction_seqnum,
               const u8 *mdie, size_t mdie_len,
	       const u8 *ftie, size_t ftie_len,
	       const u8 *rsnie, size_t rsnie_len,
	       const u8 *ric, size_t ric_len, u8 *mic)
{
	u8 *buf, *pos;
	size_t buf_len;

	if (kck_len != 16) {
		wpa_printf(MSG_WARNING, "FT: Unsupported KCK length %u",
				(unsigned int) kck_len);
		return -1;
	}

	buf_len = 2 * ETH_ALEN + 1 + mdie_len + ftie_len + rsnie_len + ric_len;
	buf = os_malloc(buf_len);
	if (buf == NULL)
		return -1;

	pos = buf;
	os_memcpy(pos, sta_addr, ETH_ALEN);
	pos += ETH_ALEN;
	os_memcpy(pos, ap_addr, ETH_ALEN);
	pos += ETH_ALEN;
	*pos++ = transaction_seqnum;
	if (rsnie) {
		os_memcpy(pos, rsnie, rsnie_len);
		pos += rsnie_len;
	}
	if (mdie) {
		os_memcpy(pos, mdie, mdie_len);
		pos += mdie_len;
	}
	if (ftie) {
		struct rsn_ftie *_ftie;
		os_memcpy(pos, ftie, ftie_len);
		if (ftie_len < 2 + sizeof(*_ftie)) {
			os_free(buf);
			return -1;
		}
		_ftie = (struct rsn_ftie *) (pos + 2);
		os_memset(_ftie->mic, 0, sizeof(_ftie->mic));
		pos += ftie_len;
	}
	if (ric) {
		os_memcpy(pos, ric, ric_len);
		pos += ric_len;
	}

	wpa_hexdump(MSG_MSGDUMP, "FT: MIC data", buf, pos - buf);
	if (omac1_aes_128(kck, buf, pos - buf, mic)) {
		os_free(buf);
		return -1;
	}

	os_free(buf);

	return 0;
}


static int wpa_ft_parse_ftie(const u8 *ie, size_t ie_len,
			     struct wpa_ft_ies *parse)
{
	const u8 *end, *pos;

	parse->ftie = ie;
	parse->ftie_len = ie_len;

	pos = ie + sizeof(struct rsn_ftie);
	end = ie + ie_len;

	while (pos + 2 <= end && pos + 2 + pos[1] <= end) {
		switch (pos[0]) {
		case FTIE_SUBELEM_R1KH_ID:
			if (pos[1] != FT_R1KH_ID_LEN) {
				wpa_printf(MSG_DEBUG, "FT: Invalid R1KH-ID "
					   "length in FTIE: %d", pos[1]);
				return -1;
			}
			parse->r1kh_id = pos + 2;
			break;
		case FTIE_SUBELEM_GTK:
			parse->gtk = pos + 2;
			parse->gtk_len = pos[1];
			break;
		case FTIE_SUBELEM_R0KH_ID:
			if (pos[1] < 1 || pos[1] > FT_R0KH_ID_MAX_LEN) {
				wpa_printf(MSG_DEBUG, "FT: Invalid R0KH-ID "
					   "length in FTIE: %d", pos[1]);
				return -1;
			}
			parse->r0kh_id = pos + 2;
			parse->r0kh_id_len = pos[1];
			break;
#ifdef CONFIG_IEEE80211W
		case FTIE_SUBELEM_IGTK:
			parse->igtk = pos + 2;
			parse->igtk_len = pos[1];
			break;
#endif /* CONFIG_IEEE80211W */
		}

		pos += 2 + pos[1];
	}

	return 0;
}


int wpa_ft_parse_ies(const u8 *ies, size_t ies_len,
		     struct wpa_ft_ies *parse)
{
	const u8 *end, *pos;
	struct wpa_ie_data data;
	int ret;
	const struct rsn_ftie *ftie;
	int prot_ie_count = 0;

	os_memset(parse, 0, sizeof(*parse));
	if (ies == NULL)
		return 0;

	pos = ies;
	end = ies + ies_len;
	while (pos + 2 <= end && pos + 2 + pos[1] <= end) {
		switch (pos[0]) {
		case WLAN_EID_RSN:
			parse->rsn = pos + 2;
			parse->rsn_len = pos[1];
			ret = wpa_parse_wpa_ie_rsn(parse->rsn - 2,
						   parse->rsn_len + 2,
						   &data);
			if (ret < 0) {
				wpa_printf(MSG_DEBUG, "FT: Failed to parse "
					   "RSN IE: %d", ret);
				return -1;
			}
			if (data.num_pmkid == 1 && data.pmkid)
				parse->rsn_pmkid = data.pmkid;
			break;
		case WLAN_EID_MOBILITY_DOMAIN:
			parse->mdie = pos + 2;
			parse->mdie_len = pos[1];
			break;
		case WLAN_EID_FAST_BSS_TRANSITION:
			if (pos[1] < sizeof(*ftie))
				return -1;
			ftie = (const struct rsn_ftie *) (pos + 2);
			prot_ie_count = ftie->mic_control[1];
			if (wpa_ft_parse_ftie(pos + 2, pos[1], parse) < 0)
				return -1;
			break;
		case WLAN_EID_TIMEOUT_INTERVAL:
			parse->tie = pos + 2;
			parse->tie_len = pos[1];
			break;
		case WLAN_EID_RIC_DATA:
			if (parse->ric == NULL)
				parse->ric = pos;
			break;
		}

		pos += 2 + pos[1];
	}

	if (prot_ie_count == 0)
		return 0; /* no MIC */

	/*
	 * Check that the protected IE count matches with IEs included in the
	 * frame.
	 */
	if (parse->rsn)
		prot_ie_count--;
	if (parse->mdie)
		prot_ie_count--;
	if (parse->ftie)
		prot_ie_count--;
	if (prot_ie_count < 0) {
		wpa_printf(MSG_DEBUG, "FT: Some required IEs not included in "
			   "the protected IE count");
		return -1;
	}

	if (prot_ie_count == 0 && parse->ric) {
		wpa_printf(MSG_DEBUG, "FT: RIC IE(s) in the frame, but not "
			   "included in protected IE count");
		return -1;
	}

	/* Determine the end of the RIC IE(s) */
	pos = parse->ric;
	while (pos && pos + 2 <= end && pos + 2 + pos[1] <= end &&
	       prot_ie_count) {
		prot_ie_count--;
		pos += 2 + pos[1];
	}
	parse->ric_len = pos - parse->ric;
	if (prot_ie_count) {
		wpa_printf(MSG_DEBUG, "FT: %d protected IEs missing from "
			   "frame", (int) prot_ie_count);
		return -1;
	}

	return 0;
}
#endif /* CONFIG_IEEE80211R */


static unsigned int wpa_kck_len(int akmp, size_t pmk_len)
{
	switch (akmp) {
	case WPA_KEY_MGMT_IEEE8021X_SUITE_B_192:
		return 24;
	case WPA_KEY_MGMT_OWE:
		return pmk_len / 2;
	case WPA_KEY_MGMT_SAE_EXT_KEY:
		return pmk_len / 2;
	default:
		return 16;
	}
}

static unsigned int wpa_kek_len(int akmp, size_t pmk_len)
{
	switch (akmp) {
	case WPA_KEY_MGMT_IEEE8021X_SUITE_B_192:
		return 32;
	case WPA_KEY_MGMT_OWE:
		return pmk_len <= 32 ? 16 : 32;
	case WPA_KEY_MGMT_SAE_EXT_KEY:
		return pmk_len <= 32 ? 16 : 32;
	default:
		return 16;
	}
}


unsigned int wpa_mic_len(int akmp, size_t pmk_len)
{
	switch (akmp) {
	case WPA_KEY_MGMT_IEEE8021X_SUITE_B_192:
		return 24;
	case WPA_KEY_MGMT_SAE_EXT_KEY:
		return pmk_len / 2;
	default:
		return 16;
	}
}

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
#ifdef CONFIG_GCMP
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_GCMP)
		return WPA_CIPHER_GCMP;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_GCMP_256)
		return WPA_CIPHER_GCMP_256;
#endif
#ifdef CONFIG_IEEE80211W
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_AES_128_CMAC)
		return WPA_CIPHER_AES_128_CMAC;
#ifdef CONFIG_GMAC
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_BIP_GMAC_128)
		return WPA_CIPHER_BIP_GMAC_128;
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_BIP_GMAC_256)
		return WPA_CIPHER_BIP_GMAC_256;
#endif
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
#ifdef CONFIG_SUITEB
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_802_1X_SUITE_B)
		return WPA_KEY_MGMT_IEEE8021X_SUITE_B;
#endif
#ifdef CONFIG_SUITEB192
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_802_1X_SUITE_B_192)
		return WPA_KEY_MGMT_IEEE8021X_SUITE_B_192;
#endif
#ifdef CONFIG_WPA3_SAE
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_SAE)
		return WPA_KEY_MGMT_SAE;
	if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_SAE_EXT_KEY)
		return WPA_KEY_MGMT_SAE_EXT_KEY;
#endif /* CONFIG_WPA3_SAE */
#ifdef CONFIG_OWE_STA
	if(RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_OWE)
		return WPA_KEY_MGMT_OWE;
#endif /* CONFIG_OWE_STA */

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

int wpa_cipher_valid_mgmt_group(int cipher)
{
	return cipher == WPA_CIPHER_AES_128_CMAC ||
		cipher == WPA_CIPHER_BIP_GMAC_128 ||
		cipher == WPA_CIPHER_BIP_GMAC_256;
}

int wpa_parse_wpa_ie_rsnxe(const u8 *rsnxe_ie, size_t rsnxe_ie_len,
             struct wpa_ie_data *data)
{
	uint8_t rsnxe_capa = 0;
	uint8_t sae_pwe = esp_wifi_get_config_sae_pwe_h2e_internal(WIFI_IF_STA);
	memset(data, 0, sizeof(*data));

	if (rsnxe_ie_len < 1) {
		return -1;
	}
	rsnxe_capa = rsnxe_ie[2];
	if (sae_pwe == 1 && !(rsnxe_capa & BIT(WLAN_RSNX_CAPAB_SAE_H2E))){
		wpa_printf(MSG_ERROR, "SAE H2E required, but not supported by the AP");
		return -1;
	}
	data->rsnxe_capa = rsnxe_capa;
	return 0;
}

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
	const struct rsn_ie_hdr *hdr;
	const u8 *pos;
	int left;
	int i, count;

	os_memset(data, 0, sizeof(*data));
	data->proto = WPA_PROTO_RSN;
	data->pairwise_cipher = WPA_CIPHER_CCMP;
	data->group_cipher = WPA_CIPHER_CCMP;
	data->key_mgmt = WPA_KEY_MGMT_IEEE8021X;
	data->capabilities = 0;
	data->pmkid = NULL;
	data->num_pmkid = 0;
	data->mgmt_group_cipher = WPA_CIPHER_AES_128_CMAC;

	wpa_hexdump(MSG_DEBUG, "rsn_ie", rsn_ie, rsn_ie_len);
	if (rsn_ie_len == 0) {
		/* No RSN IE - fail silently */
		return -1;
	}

	if (rsn_ie_len < sizeof(struct rsn_ie_hdr)) {
		wpa_printf(MSG_DEBUG, "%s: ie len too short %lu",
			   __func__, (unsigned long) rsn_ie_len);
		return -1;
	}

	hdr = (const struct rsn_ie_hdr *) rsn_ie;

	if (hdr->elem_id != WLAN_EID_RSN ||
	    hdr->len != rsn_ie_len - 2 ||
	    WPA_GET_LE16(hdr->version) != RSN_VERSION) {
		wpa_printf(MSG_DEBUG, "%s: malformed ie or unknown version",
			   __func__);
		return -2;
	}

	pos = (const u8 *) (hdr + 1);
	left = rsn_ie_len - sizeof(*hdr);

	if (left >= RSN_SELECTOR_LEN) {
		data->group_cipher = rsn_selector_to_bitfield(pos);
		pos += RSN_SELECTOR_LEN;
		left -= RSN_SELECTOR_LEN;
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
		if (count == 0 || left < count * RSN_SELECTOR_LEN) {
			wpa_printf(MSG_DEBUG, "%s: ie count botch (pairwise), "
				   "count %u left %u", __func__, count, left);
			return -4;
		}
		for (i = 0; i < count; i++) {
			data->pairwise_cipher |= rsn_selector_to_bitfield(pos);
			pos += RSN_SELECTOR_LEN;
			left -= RSN_SELECTOR_LEN;
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
		if (count == 0 || left < count * RSN_SELECTOR_LEN) {
			wpa_printf(MSG_DEBUG, "%s: ie count botch (key mgmt), "
				   "count %u left %u", __func__, count, left);
			return -6;
		}
		for (i = 0; i < count; i++) {
			data->key_mgmt |= rsn_key_mgmt_to_bitfield(pos);
			pos += RSN_SELECTOR_LEN;
			left -= RSN_SELECTOR_LEN;
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

	if (left >= 2) {
		u16 num_pmkid = WPA_GET_LE16(pos);
		pos += 2;
		left -= 2;
		if (num_pmkid > (unsigned int) left / PMKID_LEN) {
			wpa_printf(MSG_DEBUG, "%s: PMKID underflow "
				   "(num_pmkid=%u left=%d)",
				   __func__, num_pmkid, left);
			data->num_pmkid = 0;
			return -9;
		} else {
			data->num_pmkid = num_pmkid;
			data->pmkid = pos;
			pos += data->num_pmkid * PMKID_LEN;
			left -= data->num_pmkid * PMKID_LEN;
		}
	}

	if (left >= 4) {
		data->mgmt_group_cipher = rsn_selector_to_bitfield(pos);
		if (!wpa_cipher_valid_mgmt_group(data->mgmt_group_cipher)) {
			wpa_printf(MSG_DEBUG,
				   "%s: Unsupported management group cipher 0x%x (%08x)",
				   __func__, data->mgmt_group_cipher,
				   WPA_GET_BE32(pos));
			return -10;
		}
		pos += RSN_SELECTOR_LEN;
		left -= RSN_SELECTOR_LEN;
	}

	if (left > 0) {
		wpa_printf(MSG_DEBUG, "%s: ie has %u trailing bytes - ignored",
			   __func__, left);
	}

	return 0;
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

#ifdef CONFIG_IEEE80211R

/**
 * wpa_derive_pmk_r0 - Derive PMK-R0 and PMKR0Name
 *
 * IEEE Std 802.11r-2008 - 8.5.1.5.3
 */
void wpa_derive_pmk_r0(const u8 *xxkey, size_t xxkey_len,
		       const u8 *ssid, size_t ssid_len,
		       const u8 *mdid, const u8 *r0kh_id, size_t r0kh_id_len,
		       const u8 *s0kh_id, u8 *pmk_r0, u8 *pmk_r0_name)
{
	u8 buf[1 + WPA_MAX_SSID_LEN + MOBILITY_DOMAIN_ID_LEN + 1 +
	       FT_R0KH_ID_MAX_LEN + ETH_ALEN];
	u8 *pos, r0_key_data[48], hash[32];
	const u8 *addr[2];
	size_t len[2];

	/*
	 * R0-Key-Data = KDF-384(XXKey, "FT-R0",
	 *                       SSIDlength || SSID || MDID || R0KHlength ||
	 *                       R0KH-ID || S0KH-ID)
	 * XXKey is either the second 256 bits of MSK or PSK.
	 * PMK-R0 = L(R0-Key-Data, 0, 256)
	 * PMK-R0Name-Salt = L(R0-Key-Data, 256, 128)
	 */
	if (ssid_len > WPA_MAX_SSID_LEN || r0kh_id_len > FT_R0KH_ID_MAX_LEN)
		return;
	pos = buf;
	*pos++ = ssid_len;
	os_memcpy(pos, ssid, ssid_len);
	pos += ssid_len;
	os_memcpy(pos, mdid, MOBILITY_DOMAIN_ID_LEN);
	pos += MOBILITY_DOMAIN_ID_LEN;
	*pos++ = r0kh_id_len;
	os_memcpy(pos, r0kh_id, r0kh_id_len);
	pos += r0kh_id_len;
	os_memcpy(pos, s0kh_id, ETH_ALEN);
	pos += ETH_ALEN;

	sha256_prf(xxkey, xxkey_len, "FT-R0", buf, pos - buf,
		   r0_key_data, sizeof(r0_key_data));
	os_memcpy(pmk_r0, r0_key_data, PMK_LEN);

	/*
	 * PMKR0Name = Truncate-128(SHA-256("FT-R0N" || PMK-R0Name-Salt)
	 */
	addr[0] = (const u8 *) "FT-R0N";
	len[0] = 6;
	addr[1] = r0_key_data + PMK_LEN;
	len[1] = 16;

	sha256_vector(2, addr, len, hash);
	os_memcpy(pmk_r0_name, hash, WPA_PMK_NAME_LEN);
}


/**
 * wpa_derive_pmk_r1_name - Derive PMKR1Name
 *
 * IEEE Std 802.11r-2008 - 8.5.1.5.4
 */
void wpa_derive_pmk_r1_name(const u8 *pmk_r0_name, const u8 *r1kh_id,
			    const u8 *s1kh_id, u8 *pmk_r1_name)
{
	u8 hash[32];
	const u8 *addr[4];
	size_t len[4];

	/*
	 * PMKR1Name = Truncate-128(SHA-256("FT-R1N" || PMKR0Name ||
	 *                               R1KH-ID || S1KH-ID))
	 */
	addr[0] = (const u8 *) "FT-R1N";
	len[0] = 6;
	addr[1] = pmk_r0_name;
	len[1] = WPA_PMK_NAME_LEN;
	addr[2] = r1kh_id;
	len[2] = FT_R1KH_ID_LEN;
	addr[3] = s1kh_id;
	len[3] = ETH_ALEN;

	sha256_vector(4, addr, len, hash);
	os_memcpy(pmk_r1_name, hash, WPA_PMK_NAME_LEN);
}


/**
 * wpa_derive_pmk_r1 - Derive PMK-R1 and PMKR1Name from PMK-R0
 *
 * IEEE Std 802.11r-2008 - 8.5.1.5.4
 */
void wpa_derive_pmk_r1(const u8 *pmk_r0, const u8 *pmk_r0_name,
		      const u8 *r1kh_id, const u8 *s1kh_id,
		      u8 *pmk_r1, u8 *pmk_r1_name)
{
	u8 buf[FT_R1KH_ID_LEN + ETH_ALEN];
	u8 *pos;

	/* PMK-R1 = KDF-256(PMK-R0, "FT-R1", R1KH-ID || S1KH-ID) */
	pos = buf;
	os_memcpy(pos, r1kh_id, FT_R1KH_ID_LEN);
	pos += FT_R1KH_ID_LEN;
	os_memcpy(pos, s1kh_id, ETH_ALEN);
	pos += ETH_ALEN;

	sha256_prf(pmk_r0, PMK_LEN, "FT-R1", buf, pos - buf, pmk_r1, PMK_LEN);

	wpa_derive_pmk_r1_name(pmk_r0_name, r1kh_id, s1kh_id, pmk_r1_name);
}


/**
 * wpa_pmk_r1_to_ptk - Derive PTK and PTKName from PMK-R1
 *
 * IEEE Std 802.11r-2008 - 8.5.1.5.5
 */
int wpa_pmk_r1_to_ptk(const u8 *pmk_r1, const u8 *snonce, const u8 *anonce,
                     const u8 *sta_addr, const u8 *bssid,
                     const u8 *pmk_r1_name,
                     struct wpa_ptk *ptk, u8 *ptk_name, int akmp, int cipher)
{
	u8 buf[2 * WPA_NONCE_LEN + 2 * ETH_ALEN];
	u8 *pos, hash[32];
	const u8 *addr[6];
	size_t len[6];
	u8 tmp[WPA_KCK_MAX_LEN + WPA_KEK_MAX_LEN + WPA_TK_MAX_LEN];
	size_t ptk_len;

	/*
	 * PTK = KDF-PTKLen(PMK-R1, "FT-PTK", SNonce || ANonce ||
	 *                  BSSID || STA-ADDR)
	 */
	pos = buf;
	os_memcpy(pos, snonce, WPA_NONCE_LEN);
	pos += WPA_NONCE_LEN;
	os_memcpy(pos, anonce, WPA_NONCE_LEN);
	pos += WPA_NONCE_LEN;
	os_memcpy(pos, bssid, ETH_ALEN);
	pos += ETH_ALEN;
	os_memcpy(pos, sta_addr, ETH_ALEN);
	pos += ETH_ALEN;

	ptk->kck_len = wpa_kck_len(akmp, PMK_LEN);
	ptk->kek_len = wpa_kek_len(akmp, PMK_LEN);
	ptk->tk_len = wpa_cipher_key_len(cipher);
	ptk_len = ptk->kck_len + ptk->kek_len + ptk->tk_len;

	sha256_prf(pmk_r1, PMK_LEN, "FT-PTK", buf, pos - buf, tmp, ptk_len);

	/*
	 * PTKName = Truncate-128(SHA-256(PMKR1Name || "FT-PTKN" || SNonce ||
	 *                                ANonce || BSSID || STA-ADDR))
	 */
	addr[0] = pmk_r1_name;
	len[0] = WPA_PMK_NAME_LEN;
	addr[1] = (const u8 *) "FT-PTKN";
	len[1] = 7;
	addr[2] = snonce;
	len[2] = WPA_NONCE_LEN;
	addr[3] = anonce;
	len[3] = WPA_NONCE_LEN;
	addr[4] = bssid;
	len[4] = ETH_ALEN;
	addr[5] = sta_addr;
	len[5] = ETH_ALEN;

	sha256_vector(6, addr, len, hash);
	os_memcpy(ptk_name, hash, WPA_PMK_NAME_LEN);

	os_memcpy(ptk->kck, tmp, ptk->kck_len);
	os_memcpy(ptk->kek, tmp + ptk->kck_len, ptk->kek_len);
	os_memcpy(ptk->tk, tmp + ptk->kck_len + ptk->kek_len, ptk->tk_len);

	wpa_hexdump_key(MSG_DEBUG, "FT: KCK", ptk->kck, ptk->kck_len);
	wpa_hexdump_key(MSG_DEBUG, "FT: KEK", ptk->kek, ptk->kek_len);
	wpa_hexdump_key(MSG_DEBUG, "FT: TK", ptk->tk, ptk->tk_len);
	wpa_hexdump(MSG_DEBUG, "FT: PTKName", ptk_name, WPA_PMK_NAME_LEN);

	os_memset(tmp, 0, sizeof(tmp));

	return 0;
}

#endif /* CONFIG_IEEE80211R */



/**
* wpa_use_akm_defined - Is AKM-defined Key Descriptor Version used
* @akmp: WPA_KEY_MGMT_* used in key derivation
* Returns: 1 if AKM-defined Key Descriptor Version is used; 0 otherwise
*/

int wpa_use_akm_defined(int akmp){
	return akmp == WPA_KEY_MGMT_OSEN ||
		akmp == WPA_KEY_MGMT_OWE ||
		wpa_key_mgmt_sae(akmp) ||
		wpa_key_mgmt_suite_b(akmp);
}

int wpa_use_aes_key_wrap(int akmp)
{
	return akmp == WPA_KEY_MGMT_OSEN ||
		wpa_key_mgmt_ft(akmp) ||
		wpa_key_mgmt_sha256(akmp) ||
		wpa_key_mgmt_sae(akmp) ||
		wpa_key_mgmt_suite_b(akmp);
}

/**
 * wpa_eapol_key_mic - Calculate EAPOL-Key MIC
 * @key: EAPOL-Key Key Confirmation Key (KCK)
 * @key_len: KCK length in octets
 * @akmp: WPA_KEY_MGMT_* used in key derivation
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
int wpa_eapol_key_mic(const u8 *key, size_t key_len, int akmp, int ver,
		      const u8 *buf, size_t len, u8 *mic)
{
	u8 hash[SHA384_MAC_LEN];

	switch (ver) {
	case WPA_KEY_INFO_TYPE_HMAC_MD5_RC4:
		return hmac_md5(key, key_len, buf, len, mic);
	case WPA_KEY_INFO_TYPE_HMAC_SHA1_AES:
		if (hmac_sha1(key, key_len, buf, len, hash))
			return -1;
		os_memcpy(mic, hash, MD5_MAC_LEN);
		break;
	case WPA_KEY_INFO_TYPE_AES_128_CMAC:
		return omac1_aes_128(key, buf, len, mic);
#ifdef CONFIG_IEEE80211W
       case WPA_KEY_INFO_TYPE_AKM_DEFINED:
		switch (akmp) {
#ifdef CONFIG_WPA3_SAE
		case WPA_KEY_MGMT_SAE:
			return omac1_aes_128(key, buf, len, mic);
		case WPA_KEY_MGMT_SAE_EXT_KEY:
			wpa_printf(MSG_DEBUG,
				   "WPA: EAPOL-Key MIC using HMAC-SHA%u (AKM-defined - SAE-EXT-KEY)",
				   (unsigned int) key_len * 8 * 2);
			if (key_len == 128 / 8) {
				if (hmac_sha256(key, key_len, buf, len, hash))
					return -1;
			} else {
				wpa_printf(MSG_INFO,
					   "SAE: Unsupported KCK length: %u",
					   (unsigned int) key_len);
				return -1;
			}
			os_memcpy(mic, hash, key_len);
			break;
#endif /* CONFIG_WPA3_SAE */
#ifdef CONFIG_SUITEB
		case WPA_KEY_MGMT_IEEE8021X_SUITE_B:
			if (hmac_sha256(key, key_len, buf, len, hash))
				return -1;
			os_memcpy(mic, hash, MD5_MAC_LEN);
			break;
#endif /* CONFIG_SUITEB */
#ifdef CONFIG_SUITEB192
		case WPA_KEY_MGMT_IEEE8021X_SUITE_B_192:
			if (hmac_sha384(key, key_len, buf, len, hash))
				return -1;
			os_memcpy(mic, hash, 24);
			break;
#endif /* CONFIG_SUITEB192 */
#ifdef CONFIG_OWE_STA
		case WPA_KEY_MGMT_OWE:
			wpa_printf(MSG_DEBUG,
			"WPA: EAPOL-Key MIC using HMAC-SHA%u (AKM-defined - OWE)",
			(unsigned int) key_len * 8 * 2);
			if (key_len == 128 / 8) {
				if (hmac_sha256(key, key_len, buf, len, hash))
					return -1;
			} else {
				wpa_printf(MSG_INFO,"OWE: Unsupported KCK length: %u",
				(unsigned int) key_len);
				return -1;
                        }
                        os_memcpy(mic, hash, key_len);
                        break;

#endif /* CONFIG_OWE_STA */
#endif /* CONFIG_IEEE80211W */
		default:
			return -1;
		}
		break;

	default:
		return -1;
	}

	return 0;
}

u32 wpa_akm_to_suite(int akm)
{
#ifdef CONFIG_IEEE80211R
	if (akm & WPA_KEY_MGMT_FT_IEEE8021X)
		return RSN_AUTH_KEY_MGMT_FT_802_1X;
	if (akm & WPA_KEY_MGMT_FT_PSK)
		return RSN_AUTH_KEY_MGMT_FT_PSK;
#endif /* CONFIG_IEEE80211R */
	if (akm & WPA_KEY_MGMT_IEEE8021X_SHA256)
		return RSN_AUTH_KEY_MGMT_802_1X_SHA256;
	if (akm & WPA_KEY_MGMT_IEEE8021X)
		return RSN_AUTH_KEY_MGMT_UNSPEC_802_1X;
	if (akm & WPA_KEY_MGMT_PSK_SHA256)
		return RSN_AUTH_KEY_MGMT_PSK_SHA256;
	if (akm & WPA_KEY_MGMT_PSK)
		return RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X;
	if (akm & WPA_KEY_MGMT_IEEE8021X_SUITE_B)
		return RSN_AUTH_KEY_MGMT_802_1X_SUITE_B;
	if (akm & WPA_KEY_MGMT_IEEE8021X_SUITE_B_192)
		return RSN_AUTH_KEY_MGMT_802_1X_SUITE_B_192;
	if (akm & WPA_KEY_MGMT_SAE)
		return RSN_AUTH_KEY_MGMT_SAE;
	if (akm & WPA_KEY_MGMT_SAE_EXT_KEY)
		return RSN_AUTH_KEY_MGMT_SAE_EXT_KEY;
	if (akm & WPA_KEY_MGMT_FT_SAE)
		return RSN_AUTH_KEY_MGMT_FT_SAE;
	if (akm & WPA_KEY_MGMT_OWE)
		return RSN_AUTH_KEY_MGMT_OWE;
	return 0;
}

int wpa_compare_rsn_ie(int ft_initial_assoc,
		       const u8 *ie1, size_t ie1len,
		       const u8 *ie2, size_t ie2len)
{
	if (ie1 == NULL || ie2 == NULL)
		return -1;

	if (ie1len == ie2len && os_memcmp(ie1, ie2, ie1len) == 0)
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
		    ie1d.mgmt_group_cipher == ie2d.mgmt_group_cipher &&
		    ie1d.rsnxe_capa == ie2d.rsnxe_capa)
			return 0;
	}
#endif /* CONFIG_IEEE80211R */

	return -1;
}

#ifdef CONFIG_SUITEB
/**
 * rsn_pmkid_suite_b - Calculate PMK identifier for Suite B AKM
 * @kck: Key confirmation key
 * @kck_len: Length of kck in bytes
 * @aa: Authenticator address
 * @spa: Supplicant address
 * @pmkid: Buffer for PMKID
 * Returns: 0 on success, -1 on failure
 *
 * IEEE Std 802.11ac-2013 - 11.6.1.3 Pairwise key hierarchy
 * PMKID = Truncate(HMAC-SHA-256(KCK, "PMK Name" || AA || SPA))
 */
int rsn_pmkid_suite_b(const u8 *kck, size_t kck_len, const u8 *aa,
		      const u8 *spa, u8 *pmkid)
{
	char *title = "PMK Name";
	const u8 *addr[3];
	const size_t len[3] = { 8, ETH_ALEN, ETH_ALEN };
	unsigned char hash[SHA256_MAC_LEN];

	addr[0] = (u8 *) title;
	addr[1] = aa;
	addr[2] = spa;

	if (hmac_sha256_vector(kck, kck_len, 3, addr, len, hash) < 0)
		return -1;
	os_memcpy(pmkid, hash, PMKID_LEN);
	return 0;
}
#endif /* CONFIG_SUITEB */

#ifdef CONFIG_SUITEB192
/**
 * rsn_pmkid_suite_b_192 - Calculate PMK identifier for Suite B AKM
 * @kck: Key confirmation key
 * @kck_len: Length of kck in bytes
 * @aa: Authenticator address
 * @spa: Supplicant address
 * @pmkid: Buffer for PMKID
 * Returns: 0 on success, -1 on failure
 *
 * IEEE Std 802.11ac-2013 - 11.6.1.3 Pairwise key hierarchy
 * PMKID = Truncate(HMAC-SHA-384(KCK, "PMK Name" || AA || SPA))
 */
int rsn_pmkid_suite_b_192(const u8 *kck, size_t kck_len, const u8 *aa,
			  const u8 *spa, u8 *pmkid)
{
	char *title = "PMK Name";
	const u8 *addr[3];
	const size_t len[3] = { 8, ETH_ALEN, ETH_ALEN };
	unsigned char hash[SHA384_MAC_LEN];

	addr[0] = (u8 *) title;
	addr[1] = aa;
	addr[2] = spa;

	if (hmac_sha384_vector(kck, kck_len, 3, addr, len, hash) < 0)
		return -1;
	os_memcpy(pmkid, hash, PMKID_LEN);
	return 0;
}
#endif /* CONFIG_SUITEB192 */

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
	case WPA_CIPHER_GCMP:
		return "GCMP";
	case WPA_CIPHER_GCMP_256:
		return "GCMP-256";
	case WPA_KEY_MGMT_IEEE8021X_SUITE_B:
		return "WPA2-EAP-SUITE-B";
	case WPA_KEY_MGMT_IEEE8021X_SUITE_B_192:
		return "WPA2-EAP-SUITE-B-192";
	default:
		return "UNKNOWN";
	}
}

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
 * @akmp: Negotiated AKM
 * @cipher: Negotiated pairwise cipher
 * Returns: 0 on success, -1 on failure
 *
 * IEEE Std 802.11i-2004 - 8.5.1.2 Pairwise key hierarchy
 * PTK = PRF-X(PMK, "Pairwise key expansion",
 *             Min(AA, SA) || Max(AA, SA) ||
 *             Min(ANonce, SNonce) || Max(ANonce, SNonce))
 */
int wpa_pmk_to_ptk(const u8 *pmk, size_t pmk_len, const char *label,
		   const u8 *addr1, const u8 *addr2,
		   const u8 *nonce1, const u8 *nonce2,
		   struct wpa_ptk *ptk, int akmp, int cipher)
{
	if (pmk_len == 0) {
		wpa_printf(MSG_ERROR, "WPA: No PMK set for PTK derivation");
		return -1;
	}
	u8 data[2 * ETH_ALEN + 2 * WPA_NONCE_LEN];
	size_t data_len = 2 * ETH_ALEN + 2 * WPA_NONCE_LEN;
	u8 tmp[WPA_KCK_MAX_LEN + WPA_KEK_MAX_LEN + WPA_TK_MAX_LEN];
	size_t ptk_len;

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

	ptk->kck_len = wpa_kck_len(akmp, pmk_len);
	ptk->kek_len = wpa_kek_len(akmp, pmk_len);
	ptk->tk_len = wpa_cipher_key_len(cipher);
	ptk_len = ptk->kck_len + ptk->kek_len + ptk->tk_len;

#if defined(CONFIG_SUITEB192)
	if (wpa_key_mgmt_sha384(akmp)) {
		wpa_printf(MSG_DEBUG, "WPA: PTK derivation using PRF(SHA384)");
		if (sha384_prf(pmk, pmk_len, label, data, data_len,
					tmp, ptk_len) < 0)
			return -1;
	} else
#endif
	if (wpa_key_mgmt_sha256(akmp)) {
		sha256_prf(pmk, pmk_len, label, data, data_len,
			   tmp, ptk_len);
#ifdef CONFIG_WPA3_SAE
	} else if (wpa_key_mgmt_sae_ext_key(akmp)) {
		if (pmk_len == 32) {
			wpa_printf(MSG_DEBUG,
				   "SAE: PTK derivation using PRF(SHA256)");
			if (sha256_prf(pmk, pmk_len, label, data, data_len,
				       tmp, ptk_len) < 0)
				return -1;
		} else {
			wpa_printf(MSG_INFO, "SAE: Unknown PMK length %u",
				   (unsigned int) pmk_len);
			return -1;
		}
#endif /* CONFIG_WPA3_SAE */
	} else {
		sha1_prf(pmk, pmk_len, label, data, data_len, tmp, ptk_len);
	}
	wpa_printf(MSG_DEBUG, "WPA: PTK derivation - A1=" MACSTR " A2=" MACSTR"\n",
		   MAC2STR(addr1), MAC2STR(addr2));

	wpa_hexdump(MSG_MSGDUMP, "WPA: PMK", pmk, pmk_len);
	wpa_hexdump_key(MSG_DEBUG, "WPA: PTK", tmp, ptk_len);

	os_memcpy(ptk->kck, tmp, ptk->kck_len);
	wpa_hexdump_key(MSG_DEBUG, "WPA: KCK", ptk->kck, ptk->kck_len);

	os_memcpy(ptk->kek, tmp + ptk->kck_len, ptk->kek_len);
	wpa_hexdump_key(MSG_DEBUG, "WPA: KEK", ptk->kek, ptk->kek_len);

	os_memcpy(ptk->tk, tmp + ptk->kck_len + ptk->kek_len, ptk->tk_len);
	wpa_hexdump_key(MSG_DEBUG, "WPA: TK", ptk->tk, ptk->tk_len);

	os_memset(tmp, 0, sizeof(tmp));
	return 0;
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
	       u8 *pmkid, int akmp)
{
	char *title = "PMK Name";
	const u8 *addr[3];
	const size_t len[3] = { 8, ETH_ALEN, ETH_ALEN };
	unsigned char hash[SHA256_MAC_LEN];

	addr[0] = (u8 *) title;
	addr[1] = aa;
	addr[2] = spa;

#ifdef CONFIG_IEEE80211W
	if (wpa_key_mgmt_sha256(akmp)) {
		wpa_printf(MSG_DEBUG, "RSN: Derive PMKID using HMAC-SHA-256");
		hmac_sha256_vector(pmk, pmk_len, 3, addr, len, hash);
	} else
#endif /* CONFIG_IEEE80211W */
	{
		wpa_printf(MSG_DEBUG, "RSN: Derive PMKID using HMAC-SHA-1");
		hmac_sha1_vector(pmk, pmk_len, 3, addr, len, hash);
	}
	os_memcpy(pmkid, hash, PMKID_LEN);
}


int wpa_insert_pmkid(u8 *ies, size_t *ies_len, const u8 *pmkid)
{
	u8 *start, *end, *rpos, *rend;
	int added = 0;

	start = ies;
	end = ies + *ies_len;

	while (start < end) {
		if (*start == WLAN_EID_RSN)
			break;
		start += 2 + start[1];
	}
	if (start >= end) {
		wpa_printf(MSG_ERROR, "RSN: Could not find RSNE in IEs data");
		return -1;
	}
	wpa_hexdump(MSG_DEBUG, "RSN: RSNE before modification",
		    start, 2 + start[1]);

	/* Find start of PMKID-Count */
	rpos = start + 2;
	rend = rpos + start[1];

	/* Skip Version and Group Data Cipher Suite */
	rpos += 2 + 4;
	/* Skip Pairwise Cipher Suite Count and List */
	rpos += 2 + WPA_GET_LE16(rpos) * RSN_SELECTOR_LEN;
	/* Skip AKM Suite Count and List */
	rpos += 2 + WPA_GET_LE16(rpos) * RSN_SELECTOR_LEN;

	if (rpos == rend) {
		/* Add RSN Capabilities */
		os_memmove(rpos + 2, rpos, end - rpos);
		*rpos++ = 0;
		*rpos++ = 0;
		added += 2;
		start[1] += 2;
		rend = rpos;
	} else {
		/* Skip RSN Capabilities */
		rpos += 2;
		if (rpos > rend) {
			wpa_printf(MSG_ERROR,
				   "RSN: Could not parse RSNE in IEs data");
			return -1;
		}
	}

	if (rpos == rend) {
		/* No PMKID-Count field included; add it */
		os_memmove(rpos + 2 + PMKID_LEN, rpos, end + added - rpos);
		WPA_PUT_LE16(rpos, 1);
		rpos += 2;
		os_memcpy(rpos, pmkid, PMKID_LEN);
		added += 2 + PMKID_LEN;
		start[1] += 2 + PMKID_LEN;
	} else {
		u16 num_pmkid;

		if (rend - rpos < 2)
			return -1;
		num_pmkid = WPA_GET_LE16(rpos);
		/* PMKID-Count was included; use it */
		if (num_pmkid != 0) {
			u8 *after;

			if (num_pmkid * PMKID_LEN > rend - rpos - 2)
				return -1;
			/*
			 * PMKID may have been included in RSN IE in
			 * (Re)Association Request frame, so remove the old
			 * PMKID(s) first before adding the new one.
			 */
			wpa_printf(MSG_DEBUG,
				   "RSN: Remove %u old PMKID(s) from RSNE",
				   num_pmkid);
			after = rpos + 2 + num_pmkid * PMKID_LEN;
			os_memmove(rpos + 2, after, end - after);
			start[1] -= num_pmkid * PMKID_LEN;
			added -= num_pmkid * PMKID_LEN;
		}
		WPA_PUT_LE16(rpos, 1);
		rpos += 2;
		os_memmove(rpos + PMKID_LEN, rpos, end + added - rpos);
		os_memcpy(rpos, pmkid, PMKID_LEN);
		added += PMKID_LEN;
		start[1] += PMKID_LEN;
	}

	wpa_hexdump(MSG_DEBUG, "RSN: RSNE after modification (PMKID inserted)",
		    start, 2 + start[1]);

	*ies_len += added;

	return 0;
}




int wpa_cipher_key_len(int cipher)
{
	switch (cipher) {
	case WPA_CIPHER_TKIP:
#ifdef CONFIG_GCMP
	case WPA_CIPHER_GCMP_256:
#endif
#ifdef CONFIG_GMAC
	case WPA_CIPHER_BIP_GMAC_256:
#endif
		return 32;
	case WPA_CIPHER_CCMP:
#ifdef CONFIG_GCMP
	case WPA_CIPHER_GCMP:
#endif
#ifdef CONFIG_GMAC
	case WPA_CIPHER_BIP_GMAC_128:
#endif
	case WPA_CIPHER_AES_128_CMAC:
		return 16;
	case WPA_CIPHER_WEP104:
		return 13;
	case WPA_CIPHER_WEP40:
		return 5;
	}

	return 0;
}

int wpa_cipher_rsc_len(int cipher)
{
	switch (cipher) {
	case WPA_CIPHER_GCMP_256:
	case WPA_CIPHER_CCMP:
	case WPA_CIPHER_GCMP:
	case WPA_CIPHER_TKIP:
		return 6;
	}

	return 0;
}

int wpa_cipher_to_alg(int cipher)
{
	switch (cipher) {
	case WPA_CIPHER_CCMP:
		return WIFI_WPA_ALG_CCMP;
#ifdef CONFIG_GCMP
	case WPA_CIPHER_GCMP_256:
	case WPA_CIPHER_GCMP:
		return WIFI_WPA_ALG_GCMP;
#endif
	case WPA_CIPHER_TKIP:
		return WIFI_WPA_ALG_TKIP;
	case WPA_CIPHER_WEP104:
		return WIFI_WPA_ALG_WEP104;
	case WPA_CIPHER_WEP40:
		return WIFI_WPA_ALG_WEP40;
	}
	return WIFI_WPA_ALG_NONE;
}


int wpa_cipher_valid_pairwise(int cipher)
{
	return cipher == WPA_CIPHER_GCMP_256 ||
		cipher == WPA_CIPHER_CCMP ||
		cipher == WPA_CIPHER_GCMP ||
		cipher == WPA_CIPHER_TKIP;
}



u32 wpa_cipher_to_suite(int proto, int cipher)
{
	if (cipher & WPA_CIPHER_CCMP)
		return (proto == WPA_PROTO_RSN ?
			RSN_CIPHER_SUITE_CCMP : WPA_CIPHER_SUITE_CCMP);
#ifdef CONFIG_GCMP
	if (cipher & WPA_CIPHER_GCMP_256)
		return RSN_CIPHER_SUITE_GCMP_256;
	if (cipher & WPA_CIPHER_GCMP)
		return RSN_CIPHER_SUITE_GCMP;
#endif
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
	if (cipher & WPA_CIPHER_AES_128_CMAC)
		return RSN_CIPHER_SUITE_AES_128_CMAC;
#if CONFIG_GMAC
	if (cipher & WPA_CIPHER_BIP_GMAC_128)
		return RSN_CIPHER_SUITE_BIP_GMAC_128;
	if (cipher & WPA_CIPHER_BIP_GMAC_256)
		return RSN_CIPHER_SUITE_BIP_GMAC_256;
#endif
	return 0;
}

int rsn_cipher_put_suites(u8 *pos, int ciphers)
{
	int num_suites = 0;

#ifdef CONFIG_GCMP
	if (ciphers & WPA_CIPHER_GCMP_256) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_GCMP_256);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
	if (ciphers & WPA_CIPHER_GCMP) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_GCMP);
		pos += RSN_SELECTOR_LEN;
		num_suites++;
	}
#endif
	if (ciphers & WPA_CIPHER_CCMP) {
		RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_CCMP);
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

#endif // ESP_SUPPLICANT
