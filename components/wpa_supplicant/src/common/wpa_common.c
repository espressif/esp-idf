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
               const u8 *rsnxe, size_t rsnxe_len,
	       const u8 *ric, size_t ric_len, u8 *mic)
{
	u8 *buf, *pos;
	size_t buf_len;

	if (kck_len != 16) {
		wpa_printf(MSG_WARNING, "FT: Unsupported KCK length %u",
				(unsigned int) kck_len);
		return -1;
	}

	buf_len = 2 * ETH_ALEN + 1 + mdie_len + ftie_len + rsnie_len + ric_len + rsnxe_len;
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
        if (rsnxe) {
            os_memcpy(pos, rsnxe, rsnxe_len);
            pos += rsnxe_len;
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
                u8 len;

                len = pos[1];
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
                case WLAN_EID_RSNX:
                        wpa_hexdump(MSG_DEBUG, "FT: RSNXE", pos, len);
                        if (len < 1)
                                break;
                        parse->rsnxe = pos + 2;
                        parse->rsnxe_len = pos[1];
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
        if (parse->rsnxe)
            prot_ie_count--;
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
	if (RSN_SELECTOR_GET(s) == RSN_CIPHER_SUITE_NO_GROUP_ADDRESSED)
		return WPA_CIPHER_GTK_NOT_USED;

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
        if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_SAE_EXT_KEY)
                return WPA_KEY_MGMT_SAE_EXT_KEY;
        if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_FT_SAE)
                return WPA_KEY_MGMT_FT_SAE;
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
#if defined(CONFIG_OWE_STA) || defined(CONFIG_OWE_SOFTAP)
	if(RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_OWE)
		return WPA_KEY_MGMT_OWE;
#endif /* CONFIG_OWE_STA || CONFIG_OWE_SOFTAP */
#ifdef CONFIG_DPP
        if (RSN_SELECTOR_GET(s) == RSN_AUTH_KEY_MGMT_DPP)
                return WPA_KEY_MGMT_DPP;
#endif /* CONFIG_DPP */


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
    return cipher == WPA_CIPHER_GTK_NOT_USED ||
        cipher == WPA_CIPHER_AES_128_CMAC ||
        cipher == WPA_CIPHER_BIP_GMAC_128 ||
		cipher == WPA_CIPHER_BIP_GMAC_256;
}

int wpa_parse_wpa_ie_rsnxe(const u8 *rsnxe_ie, size_t rsnxe_ie_len,
             struct wpa_ie_data *data)
{
	uint8_t rsnxe_capa = 0;
	uint8_t sae_pwe = esp_wifi_get_config_sae_pwe_h2e_internal(WIFI_IF_STA);
	memset(data, 0, sizeof(*data));

	if (rsnxe_ie_len < 3 || !rsnxe_ie) {
		return -1;
	}
	if (rsnxe_ie[0] == WLAN_EID_VENDOR_SPECIFIC &&
		rsnxe_ie[1] >= 1 + 4) {
		rsnxe_capa = rsnxe_ie[2 + 4];
	} else {
		rsnxe_capa = rsnxe_ie[2];
	}
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

	wpa_hexdump(MSG_MSGDUMP, "rsn_ie", rsn_ie, rsn_ie_len);
	if (rsn_ie_len == 0) {
		/* No RSN IE - fail silently */
		return -1;
	}

	if (rsn_ie_len < sizeof(struct rsn_ie_hdr)) {
		wpa_printf(MSG_DEBUG, "%s: ie len too short %lu",
			   __func__, (unsigned long) rsn_ie_len);
		return -1;
	}

	if (rsn_ie_len >= 2 + 4 + 2 && rsn_ie[1] >= 4 + 2 &&
		   rsn_ie[1] == rsn_ie_len - 2 &&
		   (WPA_GET_BE32(&rsn_ie[2]) == RSNE_OVERRIDE_IE_VENDOR_TYPE) &&
		   WPA_GET_LE16(&rsn_ie[2 + 4]) == RSN_VERSION) {
		pos = rsn_ie + 2 + 4 + 2;
		left = rsn_ie_len - 2 - 4 - 2;
	} else {
		const struct rsn_ie_hdr *hdr;

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
	}

	if (left >= RSN_SELECTOR_LEN) {
		data->group_cipher = rsn_selector_to_bitfield(pos);
		data->has_group = 1;
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
		if (count)
			data->has_pairwise = 1;
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
               akmp == WPA_KEY_MGMT_DPP ||
               wpa_key_mgmt_sae(akmp) ||
               wpa_key_mgmt_suite_b(akmp);
}

/**
 * wpa_use_aes_key_wrap - Is AES Keywrap algorithm used for EAPOL-Key Key Data
 * @akmp: WPA_KEY_MGMT_* used in key derivation
 * Returns: 1 if AES Keywrap is used; 0 otherwise
 *
 * Note: AKM 00-0F-AC:1 and 00-0F-AC:2 have special rules for selecting whether
 * to use AES Keywrap based on the negotiated pairwise cipher. This function
 * does not cover those special cases.
 */
int wpa_use_aes_key_wrap(int akmp)
{
        return akmp == WPA_KEY_MGMT_OSEN ||
               akmp == WPA_KEY_MGMT_OWE ||
               akmp == WPA_KEY_MGMT_DPP ||
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
                case WPA_KEY_MGMT_FT_SAE:
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
#if defined(CONFIG_OWE_STA) || defined(CONFIG_OWE_SOFTAP)
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

#endif /* CONFIG_OWE_STA || CONFIG_OWE_SOFTAP */
#ifdef CONFIG_DPP
                case WPA_KEY_MGMT_DPP:
                        wpa_printf(MSG_DEBUG,
                                   "WPA: EAPOL-Key MIC using HMAC-SHA%u (AKM-defined - DPP)",
                                   (unsigned int) key_len * 8 * 2);
                        if (key_len == 128 / 8) {
                                if (hmac_sha256(key, key_len, buf, len, hash))
                                        return -1;
                        } else {
                                wpa_printf(MSG_INFO,
                                           "DPP: Unsupported KCK length: %u",
                                           (unsigned int) key_len);
                                return -1;
                        }
                        os_memcpy(mic, hash, key_len);
                        break;
#endif /* CONFIG_DPP */

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
	const char *title = "PMK Name";
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
	u8 len = pos[1];
	size_t dlen = 2 + len;
	u32 selector;
	const u8 *p;
	size_t left;

	if (len == 0)
		return 1;

	if (len < RSN_SELECTOR_LEN)
		return 2;

	p = pos + 2;
	selector = RSN_SELECTOR_GET(p);
	p += RSN_SELECTOR_LEN;
	left = len - RSN_SELECTOR_LEN;

	if (left >= 2 && selector == WPA_OUI_TYPE && p[0] == 1 && p[1] == 0) {
		ie->wpa_ie = pos;
		ie->wpa_ie_len = dlen;
		wpa_hexdump(MSG_DEBUG, "WPA: WPA IE in EAPOL-Key",
			    ie->wpa_ie, ie->wpa_ie_len);
		return 0;
	}

	if (left >= PMKID_LEN && selector == RSN_KEY_DATA_PMKID) {
		ie->pmkid = p;
		wpa_hexdump(MSG_DEBUG, "WPA: PMKID in EAPOL-Key", pos, dlen);
		return 0;
	}

	if (left > 2 && selector == RSN_KEY_DATA_GROUPKEY) {
		ie->gtk = p;
		ie->gtk_len = left;
		wpa_hexdump_key(MSG_DEBUG, "WPA: GTK in EAPOL-Key", pos, dlen);
		return 0;
	}

	if (left >= ETH_ALEN && selector == RSN_KEY_DATA_MAC_ADDR) {
		ie->mac_addr = p;
		wpa_printf(MSG_DEBUG, "WPA: MAC Address in EAPOL-Key: " MACSTR,
			   MAC2STR(ie->mac_addr));
		return 0;
	}

#ifdef CONFIG_IEEE80211W
	if (left > 2 && selector == RSN_KEY_DATA_IGTK) {
		ie->igtk = p;
		ie->igtk_len = left;
		wpa_hexdump_key(MSG_DEBUG, "WPA: IGTK in EAPOL-Key",
				pos, dlen);
		return 0;
	}
#endif /* CONFIG_IEEE80211W */

	if (left >= 1 && selector == WFA_KEY_DATA_TRANSITION_DISABLE) {
		ie->transition_disable = p;
		ie->transition_disable_len = left;
		wpa_hexdump(MSG_DEBUG,
				"WPA: Transition Disable KDE in EAPOL-Key",
					pos, dlen);
		return 0;
	}

#ifdef CONFIG_WPA3_COMPAT
	if (selector == RSNE_OVERRIDE_IE_VENDOR_TYPE) {
		ie->rsne_override = pos;
		ie->rsne_override_len = dlen;
		wpa_hexdump(MSG_DEBUG,
			    "RSN: RSNE Override element in EAPOL-Key",
			    ie->rsne_override, ie->rsne_override_len);
		return 0;
	}

	if (selector == RSNXE_OVERRIDE_IE_VENDOR_TYPE) {
		ie->rsnxe_override = pos;
		ie->rsnxe_override_len = dlen;
		wpa_hexdump(MSG_DEBUG,
			    "RSN: RSNXE Override element in EAPOL-Key",
			    ie->rsnxe_override, ie->rsnxe_override_len);
		return 0;
	}

	if (selector == RSN_SELECTION_IE_VENDOR_TYPE) {
		ie->rsn_selection = p;
		ie->rsn_selection_len = left;
		wpa_hexdump(MSG_DEBUG,
			    "RSN: RSN Selection element in EAPOL-Key",
			    ie->rsn_selection, ie->rsn_selection_len);
		return 0;
	}
#endif
	return 2;
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

	os_memset(ie, 0, sizeof(*ie));
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
		} else if (*pos == WLAN_EID_RSNX) {
			ie->rsnxe = pos;
			ie->rsnxe_len = pos[1] + 2;
			wpa_hexdump(MSG_DEBUG, "WPA: RSNXE in EAPOL-Key",
				    ie->rsnxe, ie->rsnxe_len);
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

#ifdef CONFIG_PASN

/*
 * wpa_pasn_build_auth_header - Add the MAC header and initialize Authentication
 * frame for PASN
 *
 * @buf: Buffer in which the header will be added
 * @bssid: The BSSID of the AP
 * @src: Source address
 * @dst: Destination address
 * @trans_seq: Authentication transaction sequence number
 * @status: Authentication status
 */
void wpa_pasn_build_auth_header(struct wpabuf *buf, const u8 *bssid,
				const u8 *src, const u8 *dst,
				u8 trans_seq, u16 status)
{
	struct ieee80211_auth *auth;

	wpa_printf(MSG_DEBUG, "PASN: Add authentication header. trans_seq=%u",
		   trans_seq);

	auth = wpabuf_put(buf, offsetof(struct ieee80211_auth, auth.variable));

	auth->frame_control = host_to_le16((WLAN_FC_TYPE_MGMT << 2) |
					   (WLAN_FC_STYPE_AUTH << 4));

	os_memcpy(auth->da, dst, ETH_ALEN);
	os_memcpy(auth->sa, src, ETH_ALEN);
	os_memcpy(auth->bssid, bssid, ETH_ALEN);
	auth->seq_ctrl = 0;

	auth->auth.auth_alg = host_to_le16(WLAN_AUTH_PASN);
	auth->auth.auth_transaction = host_to_le16(trans_seq);
	auth->auth.status_code = host_to_le16(status);
}


/*
 * wpa_pasn_add_rsne - Add an RSNE for PASN authentication
 * @buf: Buffer in which the IE will be added
 * @pmkid: Optional PMKID. Can be NULL.
 * @akmp: Authentication and key management protocol
 * @cipher: The cipher suite
 */
int wpa_pasn_add_rsne(struct wpabuf *buf, const u8 *pmkid, int akmp, int cipher)
{
	struct rsn_ie_hdr *hdr;
	u32 suite;
	u16 capab;
	u8 *pos;
	u8 rsne_len;

	wpa_printf(MSG_DEBUG, "PASN: Add RSNE");

	rsne_len = sizeof(*hdr) + RSN_SELECTOR_LEN +
		2 + RSN_SELECTOR_LEN + 2 + RSN_SELECTOR_LEN +
		2 + RSN_SELECTOR_LEN + 2 + (pmkid ? PMKID_LEN : 0);

	if (wpabuf_tailroom(buf) < rsne_len)
		return -1;
	hdr = wpabuf_put(buf, rsne_len);
	hdr->elem_id = WLAN_EID_RSN;
	hdr->len = rsne_len - 2;
	WPA_PUT_LE16(hdr->version, RSN_VERSION);
	pos = (u8 *) (hdr + 1);

	/* Group addressed data is not allowed */
	RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_NO_GROUP_ADDRESSED);
	pos += RSN_SELECTOR_LEN;

	/* Add the pairwise cipher */
	WPA_PUT_LE16(pos, 1);
	pos += 2;
	suite = wpa_cipher_to_suite(WPA_PROTO_RSN, cipher);
	RSN_SELECTOR_PUT(pos, suite);
	pos += RSN_SELECTOR_LEN;

	/* Add the AKM suite */
	WPA_PUT_LE16(pos, 1);
	pos += 2;

	switch (akmp) {
	case WPA_KEY_MGMT_PASN:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_PASN);
		break;
#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
	case WPA_KEY_MGMT_SAE:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_SAE);
		break;
	case WPA_KEY_MGMT_SAE_EXT_KEY:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_SAE_EXT_KEY);
		break;
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */
#ifdef CONFIG_FILS
	case WPA_KEY_MGMT_FILS_SHA256:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FILS_SHA256);
		break;
	case WPA_KEY_MGMT_FILS_SHA384:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FILS_SHA384);
		break;
#endif /* CONFIG_FILS */
#ifdef CONFIG_IEEE80211R
	case WPA_KEY_MGMT_FT_PSK:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_PSK);
		break;
	case WPA_KEY_MGMT_FT_IEEE8021X:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_802_1X);
		break;
	case WPA_KEY_MGMT_FT_IEEE8021X_SHA384:
		RSN_SELECTOR_PUT(pos, RSN_AUTH_KEY_MGMT_FT_802_1X_SHA384);
		break;
#endif /* CONFIG_IEEE80211R */
	default:
		wpa_printf(MSG_ERROR, "PASN: Invalid AKMP=0x%x", akmp);
		return -1;
	}
	pos += RSN_SELECTOR_LEN;

	/* RSN Capabilities: PASN mandates both MFP capable and required */
	capab = WPA_CAPABILITY_MFPC | WPA_CAPABILITY_MFPR;
	capab |= WPA_CAPABILITY_OCVC; /* OCVC: testing — Operating Channel Validation */
	WPA_PUT_LE16(pos, capab);
	pos += 2;

	if (pmkid) {
		wpa_printf(MSG_DEBUG, "PASN: Adding PMKID");

		WPA_PUT_LE16(pos, 1);
		pos += 2;
		os_memcpy(pos, pmkid, PMKID_LEN);
		pos += PMKID_LEN;
	} else {
		WPA_PUT_LE16(pos, 0);
		pos += 2;
	}

	/* Group addressed management is not allowed */
	RSN_SELECTOR_PUT(pos, RSN_CIPHER_SUITE_NO_GROUP_ADDRESSED);

	return 0;
}


/*
 * wpa_pasn_add_parameter_ie - Add PASN Parameters IE for PASN authentication
 * @buf: Buffer in which the IE will be added
 * @pasn_group: Finite Cyclic Group ID for PASN authentication
 * @wrapped_data_format: Format of the data in the Wrapped Data IE
 * @pubkey: A buffer holding the local public key. Can be NULL
 * @compressed: In case pubkey is included, indicates if the public key is
 *     compressed (only x coordinate is included) or not (both x and y
 *     coordinates are included)
 * @comeback: A buffer holding the comeback token. Can be NULL
 * @after: If comeback is set, defined the comeback time in seconds. -1 to not
 *	include the Comeback After field (frames from non-AP STA).
 */
void wpa_pasn_add_parameter_ie(struct wpabuf *buf, u16 pasn_group,
			       u8 wrapped_data_format,
			       const struct wpabuf *pubkey, bool compressed,
			       const struct wpabuf *comeback, int after)
{
	struct pasn_parameter_ie *params;

	wpa_printf(MSG_DEBUG, "PASN: Add PASN Parameters element");

	params = wpabuf_put(buf, sizeof(*params));

	params->id = WLAN_EID_EXTENSION;
	params->len = sizeof(*params) - 2;
	params->id_ext = WLAN_EID_EXT_PASN_PARAMS;
	params->control = 0;
	params->wrapped_data_format = wrapped_data_format;

	if (comeback) {
		wpa_printf(MSG_DEBUG, "PASN: Adding comeback data");

		/*
		 * 2 octets for the 'after' field + 1 octet for the length +
		 * actual cookie data
		 */
		if (after >= 0)
			params->len += 2;
		params->len += 1 + wpabuf_len(comeback);
		params->control |= WPA_PASN_CTRL_COMEBACK_INFO_PRESENT;

		if (after >= 0)
			wpabuf_put_le16(buf, after);
		wpabuf_put_u8(buf, wpabuf_len(comeback));
		wpabuf_put_buf(buf, comeback);
	}

	if (pubkey) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Adding public key and group ID %u",
			   pasn_group);

		/*
		 * 2 octets for the finite cyclic group + 2 octets public key
		 * length + 1 octet for the compressed/uncompressed indication +
		 * the actual key.
		 */
		params->len += 2 + 1 + 1 + wpabuf_len(pubkey);
		params->control |= WPA_PASN_CTRL_GROUP_AND_KEY_PRESENT;

		wpabuf_put_le16(buf, pasn_group);

		/*
		 * The first octet indicates whether the public key is
		 * compressed, as defined in RFC 5480 section 2.2.
		 */
		wpabuf_put_u8(buf, wpabuf_len(pubkey) + 1);
		wpabuf_put_u8(buf, compressed ? WPA_PASN_PUBKEY_COMPRESSED_0 :
			      WPA_PASN_PUBKEY_UNCOMPRESSED);

		wpabuf_put_buf(buf, pubkey);
	}
}

/*
 * wpa_pasn_add_wrapped_data - Add a Wrapped Data IE to PASN Authentication
 * frame. If needed, the Wrapped Data IE would be fragmented.
 *
 * @buf: Buffer in which the IE will be added
 * @wrapped_data_buf: Buffer holding the wrapped data
 */
int wpa_pasn_add_wrapped_data(struct wpabuf *buf,
			      struct wpabuf *wrapped_data_buf)
{
	const u8 *data;
	size_t data_len;
	u8 len;

	if (!wrapped_data_buf)
		return 0;

	wpa_printf(MSG_DEBUG, "PASN: Add wrapped data");

	data = wpabuf_head_u8(wrapped_data_buf);
	data_len = wpabuf_len(wrapped_data_buf);

	/* nothing to add */
	if (!data_len)
		return 0;

	if (data_len <= 254)
		len = 1 + data_len;
	else
		len = 255;

	if (wpabuf_tailroom(buf) < 3 + data_len)
		return -1;

	wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
	wpabuf_put_u8(buf, len);
	wpabuf_put_u8(buf, WLAN_EID_EXT_WRAPPED_DATA);
	wpabuf_put_data(buf, data, len - 1);

	data += len - 1;
	data_len -= len - 1;

	while (data_len) {
		if (wpabuf_tailroom(buf) < 2 + data_len)
			return -1;
		wpabuf_put_u8(buf, WLAN_EID_FRAGMENT);
		len = data_len > 255 ? 255 : data_len;
		wpabuf_put_u8(buf, len);
		wpabuf_put_data(buf, data, len);
		data += len;
		data_len -= len;
	}

	return 0;
}


/*
 * wpa_pasn_validate_rsne - Validate PASN specific data of RSNE
 * @data: Parsed representation of an RSNE
 * Returns -1 for invalid data; otherwise 0
 */
int wpa_pasn_validate_rsne(const struct wpa_ie_data *data)
{
	u16 capab = WPA_CAPABILITY_MFPC | WPA_CAPABILITY_MFPR;

	if (data->proto != WPA_PROTO_RSN)
		return -1;

	if ((data->capabilities & capab) != capab) {
		wpa_printf(MSG_DEBUG, "PASN: Invalid RSNE capabilities");
		return -1;
	}

	if ((!data->has_group) || (data->group_cipher != WPA_CIPHER_GTK_NOT_USED)) {
		wpa_printf(MSG_DEBUG, "PASN: Invalid group data cipher");
		return -1;
	}

	if (!data->has_pairwise || !data->pairwise_cipher ||
	    (data->pairwise_cipher & (data->pairwise_cipher - 1))) {
		wpa_printf(MSG_DEBUG, "PASN: No valid pairwise suite");
		return -1;
	}

	switch (data->key_mgmt) {
#ifdef CONFIG_SAE
	case WPA_KEY_MGMT_SAE:
	case WPA_KEY_MGMT_SAE_EXT_KEY:
	/* fall through */
#endif /* CONFIG_SAE */
#ifdef CONFIG_FILS
	case WPA_KEY_MGMT_FILS_SHA256:
	case WPA_KEY_MGMT_FILS_SHA384:
	/* fall through */
#endif /* CONFIG_FILS */
#ifdef CONFIG_IEEE80211R
	case WPA_KEY_MGMT_FT_PSK:
	case WPA_KEY_MGMT_FT_IEEE8021X:
	case WPA_KEY_MGMT_FT_IEEE8021X_SHA384:
	/* fall through */
#endif /* CONFIG_IEEE80211R */
	case WPA_KEY_MGMT_PASN:
		break;
	default:
		wpa_printf(MSG_ERROR, "PASN: invalid key_mgmt: 0x%0x",
			   data->key_mgmt);
		return -1;
	}

	if (data->mgmt_group_cipher != WPA_CIPHER_GTK_NOT_USED) {
		wpa_printf(MSG_DEBUG, "PASN: Invalid group mgmt cipher");
		return -1;
	}

	if (data->num_pmkid > 1) {
		wpa_printf(MSG_DEBUG, "PASN: Invalid number of PMKIDs");
		return -1;
	}

	return 0;
}


/*
 * wpa_pasn_parse_parameter_ie - Validates PASN Parameters IE
 * @data: Pointer to the PASN Parameters IE (starting with the EID).
 * @len: Length of the data in the PASN Parameters IE
 * @from_ap: Whether this was received from an AP
 * @pasn_params: On successful return would hold the parsed PASN parameters.
 * Returns: -1 for invalid data; otherwise 0
 *
 * Note: On successful return, the pointers in &pasn_params point to the data in
 * the IE and are not locally allocated (so they should not be freed etc.).
 */
int wpa_pasn_parse_parameter_ie(const u8 *data, u8 len, bool from_ap,
				struct wpa_pasn_params_data *pasn_params)
{
	struct pasn_parameter_ie *params = (struct pasn_parameter_ie *) data;
	const u8 *pos = (const u8 *) (params + 1);

	if (!pasn_params) {
		wpa_printf(MSG_DEBUG, "PASN: Invalid params");
		return -1;
	}

	if (!params || ((size_t) (params->len + 2) < sizeof(*params)) ||
	    len < sizeof(*params) || params->len + 2 != len) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Invalid parameters IE. len=(%u, %u)",
			   params ? params->len : 0, len);
		return -1;
	}

	os_memset(pasn_params, 0, sizeof(*pasn_params));

	switch (params->wrapped_data_format) {
	case WPA_PASN_WRAPPED_DATA_NO:
	case WPA_PASN_WRAPPED_DATA_SAE:
	case WPA_PASN_WRAPPED_DATA_FILS_SK:
	case WPA_PASN_WRAPPED_DATA_FT:
		break;
	default:
		wpa_printf(MSG_DEBUG, "PASN: Invalid wrapped data format");
		return -1;
	}

	pasn_params->wrapped_data_format = params->wrapped_data_format;

	len -= sizeof(*params);

	if (params->control & WPA_PASN_CTRL_COMEBACK_INFO_PRESENT) {
		if (from_ap) {
			if (len < 2) {
				wpa_printf(MSG_DEBUG,
					   "PASN: Invalid Parameters IE: Truncated Comeback After");
				return -1;
			}
			pasn_params->after = WPA_GET_LE16(pos);
			pos += 2;
			len -= 2;
		}

		if (len < 1 || len < 1 + *pos) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Invalid Parameters IE: comeback len");
			return -1;
		}

		pasn_params->comeback_len = *pos++;
		len--;
		pasn_params->comeback = pos;
		len -=  pasn_params->comeback_len;
		pos += pasn_params->comeback_len;
	}

	if (params->control & WPA_PASN_CTRL_GROUP_AND_KEY_PRESENT) {
		if (len < 3 || len < 3 + pos[2]) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Invalid Parameters IE: group and key");
			return -1;
		}

		pasn_params->group = WPA_GET_LE16(pos);
		pos += 2;
		len -= 2;
		pasn_params->pubkey_len = *pos++;
		len--;
		pasn_params->pubkey = pos;
		len -= pasn_params->pubkey_len;
		pos += pasn_params->pubkey_len;
	}

	if (len) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Invalid Parameters IE. Bytes left=%u", len);
		return -1;
	}

	return 0;
}


void wpa_pasn_add_rsnxe(struct wpabuf *buf, u16 capab)
{
	size_t flen;

	flen = (capab & 0xff00) ? 2 : 1;
	if (!capab)
		return; /* no supported extended RSN capabilities */
	if (wpabuf_tailroom(buf) < 2 + flen)
		return;
	capab |= flen - 1; /* bit 0-3 = Field length (n - 1) */

	wpabuf_put_u8(buf, WLAN_EID_RSNX);
	wpabuf_put_u8(buf, flen);
	wpabuf_put_u8(buf, capab & 0x00ff);
	capab >>= 8;
	if (capab)
		wpabuf_put_u8(buf, capab);
}


/*
 * wpa_pasn_add_extra_ies - Add protocol specific IEs in Authentication
 * frame for PASN.
 *
 * @buf: Buffer in which the elements will be added
 * @extra_ies: Protocol specific elements to add
 * @len: Length of the elements
 * Returns: 0 on success, -1 on failure
 */

int wpa_pasn_add_extra_ies(struct wpabuf *buf, const u8 *extra_ies, size_t len)
{
	if (!buf)
		return -1;
	if (!len || !extra_ies)
		return 0;

	if (wpabuf_tailroom(buf) < len)
		return -1;

	wpabuf_put_data(buf, extra_ies, len);
	return 0;
}

#endif /* CONFIG_PASN */

void rsn_set_snonce_cookie(u8 *snonce)
{
	u8 *pos;

	pos = snonce + WPA_NONCE_LEN - 6;
	WPA_PUT_BE24(pos, OUI_WFA);
	pos += 3;
	WPA_PUT_BE24(pos, 0x000029);
}


bool rsn_is_snonce_cookie(const u8 *snonce)
{
	const u8 *pos;

	pos = snonce + WPA_NONCE_LEN - 6;
	return WPA_GET_BE24(pos) == OUI_WFA &&
		WPA_GET_BE24(pos + 3) == 0x000029;
}

#ifdef CONFIG_PASN

/*
 * pasn_use_sha384 - Should SHA384 be used or SHA256
 *
 * @akmp: Authentication and key management protocol
 * @cipher: The cipher suite
 *
 * According to IEEE Std 802.11-2024, 12.13.8 (PTKSA derivation with PASN
 * authentication), the hash algorithm to use is the
 * hash algorithm defined for the Base AKM (see Table 9-190 (AKM suite
 * selectors)). When there is no Base AKM, the hash algorithm is selected based
 * on the pairwise cipher suite provided in the RSNE by the AP in the second
 * PASN frame. SHA-256 is used as the hash algorithm, except for the ciphers
 * 00-0F-AC:9 and 00-0F-AC:10 for which SHA-384 is used.
 */
static bool pasn_use_sha384(int akmp, int cipher)
{
	return (akmp == WPA_KEY_MGMT_PASN && (cipher == WPA_CIPHER_GCMP_256)) ||
		wpa_key_mgmt_sha384(akmp);
}


/**
 * pasn_select_hash_alg - Select hash algorithm for PTK derivation
 * @akmp: Authentication and key management protocol
 * @cipher: The cipher suite
 * @pmk_len: PMK length in octets
 *
 * According to IEEE Std 802.11-2024, Table 9-190 (AKM suite selectors), AKMs
 * 00-0F-AC:24 and 00-0F-AC:25 have the length of the PMK, the length
 * of the SAE key confirmation key, SAE-KCK, and PTK-KCK, and the length of
 * PTK-KEK depending on the hash algorithm specified in 12.4.2 (see 12.7.1.3
 * and 12.7.3), i.e, the hash algorithm depends on the prime length associated
 * with the selected group per Table 12-1 (Hash algorithm based on length of
 * prime).
 */
static enum rsn_hash_alg pasn_select_hash_alg(int akmp, int cipher,
					      size_t pmk_len)
{
#ifdef CONFIG_SAE
	if (wpa_key_mgmt_sae_ext_key(akmp)) {
		if (pmk_len == 48)
			return RSN_HASH_SHA384;
	}
#endif /* CONFIG_SAE */

	if (pasn_use_sha384(akmp, cipher))
		return RSN_HASH_SHA384;

	return RSN_HASH_SHA256;
}

struct pasn_nd_pmk_store pasn_nd_pmk_global;

void pasn_nd_pmk_global_clear(void)
{
	os_memset(&pasn_nd_pmk_global, 0, sizeof(pasn_nd_pmk_global));
}


/**
 * pasn_nd_pmk_derive_from_kdk_store - NAN ND-PMK from NM-KDK (hostap equivalent)
 *
 * Mirrors hostap nan_crypto_derive_nd_pmk_from_kdk() / nan_crypto_derive_from_kdk():
 *
 * ND-PMK = KDF-HASH-NNN(KDK, "NDP PMK Derivation",
 *			 Pairing Initiator NMI || Pairing Responder NMI)
 *
 * NNN follows the NAN PASN cipher suite: SHA-256 for 128-bit suite (e.g. CCMP),
 * SHA-384 for 256-bit suite (GCMP-256). Output is always PMK_LEN octets.
 */
int pasn_nd_pmk_derive_from_kdk_store(const u8 *kdk, size_t kdk_len,
				      int pairwise_cipher,
				      const u8 *initiator_nmi,
				      const u8 *responder_nmi)
{
	static const char label[] = "NDP PMK Derivation";
	u8 data[2 * ETH_ALEN];
	int ret;

	if (!kdk || !kdk_len || !initiator_nmi || !responder_nmi)
		return -1;

	os_memcpy(data, initiator_nmi, ETH_ALEN);
	os_memcpy(data + ETH_ALEN, responder_nmi, ETH_ALEN);

	wpa_printf(MSG_DEBUG, "PASN: Deriving ND-PMK from NM-KDK (NAN pairing)");
	wpa_hexdump_key(MSG_DEBUG, "PASN: KDK", kdk, kdk_len);
	wpa_printf(MSG_DEBUG, "PASN: Initiator NMI " MACSTR,
		   MAC2STR(initiator_nmi));
	wpa_printf(MSG_DEBUG, "PASN: Responder NMI " MACSTR,
		   MAC2STR(responder_nmi));

	if (pairwise_cipher == WPA_CIPHER_GCMP_256) {
#ifdef CONFIG_SHA384
		wpa_printf(MSG_DEBUG, "PASN: ND-PMK derivation using SHA384");

		ret = sha384_prf(kdk, kdk_len, label, data, sizeof(data),
				 pasn_nd_pmk_global.nd_pmk, PMK_LEN);
#else /* CONFIG_SHA384 */
		wpa_printf(MSG_DEBUG, "PASN: ND-PMK SHA384 not supported");
		return -1;
#endif /* CONFIG_SHA384 */
	} else {
		wpa_printf(MSG_DEBUG, "PASN: ND-PMK derivation using SHA256");

		ret = sha256_prf(kdk, kdk_len, label, data, sizeof(data),
				 pasn_nd_pmk_global.nd_pmk, PMK_LEN);
	}

	if (ret < 0)
		return -1;

	pasn_nd_pmk_global.valid = 1;
	wpa_hexdump_key(MSG_DEBUG, "PASN: ND-PMK (global):",
			pasn_nd_pmk_global.nd_pmk, PMK_LEN);

	return 0;
}


/**
 * pasn_pmk_to_ptk - Calculate PASN/EPPKE PTK from PMK, addresses, etc.
 * @pmk: Pairwise master key
 * @pmk_len: Length of PMK
 * @spa: For EPPKE authentication, non-AP MLD MAC address is used for MLO. For
 *	PASN authentication or EPPKE authentication for non-MLO, non-AP STA link
 *	MAC address is used.
 * @bssid: For EPPKE authentication, AP MLD MAC address is used for MLO. For
 *	PASN authentication or EPPKE authentication for non-MLO, AP BSSID is
 *	used.
 * @dhss: Is the shared secret (DHss) derived from the PASN ephemeral key
 *	exchange encoded as an octet string
 * @dhss_len: The length of dhss in octets
 * @ptk: Buffer for pairwise transient key
 * @akmp: Negotiated AKM
 * @cipher: Negotiated pairwise cipher
 * @kdk_len: the length in octets that should be derived for HTLK. Can be zero.
 * @kek_len: The length in octets that should be derived for KEK. Can be zero.
 * @alg: Output variable for indicating the selected hash algorithm
 * @is_eppke: EPPKE authentication
 * Returns: 0 on success, -1 on failure
 */
int pasn_pmk_to_ptk(const u8 *pmk, size_t pmk_len,
		    const u8 *spa, const u8 *bssid,
		    const u8 *dhss, size_t dhss_len,
		    struct wpa_ptk *ptk, int akmp, int cipher,
		    size_t kdk_len, size_t kek_len, enum rsn_hash_alg *alg,
		    bool is_eppke)
{
	u8 tmp[WPA_KCK_MAX_LEN + WPA_KEK_MAX_LEN + WPA_TK_MAX_LEN +
	       WPA_KDK_MAX_LEN];
	const u8 *pos;
	u8 *data;
	size_t data_len, ptk_len;
	int ret = -1;
	const char *label = is_eppke ? "EPPKE PTK Derivation" :
		"PASN PTK Derivation";

	if (!pmk || !pmk_len) {
		wpa_printf(MSG_ERROR, "PASN: No PMK set for PTK derivation");
		return -1;
	}

	if (!dhss || !dhss_len) {
		wpa_printf(MSG_ERROR, "PASN: No DHss set for PTK derivation");
		return -1;
	}

	/*
	 * Use "EPPKE PTK Derivation" instead of "PASN PTK Derivation" for
	 * EPPKE Authentication per IEEE P802.11bi/D4.0, 12.16.9.3.4 (PTKSA
	 * derivation and MIC computation with EPPKE authentication). For EPPKE
	 * MLO, the non-AP MLD MAC address is used instead of the SPA and the
	 * AP MLD MAC address instead of the BSSID.
	 *
	 * PASN-PTK = KDF(PMK, "PASN PTK Derivation", SPA || BSSID || DHss)
	 *
	 * KCK = L(PASN-PTK, 0, 256)
	 * TK = L(PASN-PTK, 256, TK_bits)
	 * KDK = L(PASN-PTK, 256 + TK_bits, kdk_len * 8)
	 */
	data_len = 2 * ETH_ALEN + dhss_len;
	data = os_zalloc(data_len);
	if (!data)
		return -1;

	os_memcpy(data, spa, ETH_ALEN);
	os_memcpy(data + ETH_ALEN, bssid, ETH_ALEN);
	os_memcpy(data + 2 * ETH_ALEN, dhss, dhss_len);

	ptk->kck_len = WPA_PASN_KCK_LEN;
	ptk->tk_len = wpa_cipher_key_len(cipher);
	ptk->kdk_len = kdk_len;
	ptk->kek_len = kek_len;
	ptk->kek2_len = 0;
	ptk->kck2_len = 0;

	if (ptk->tk_len == 0) {
		wpa_printf(MSG_ERROR,
			   "PASN: Unsupported cipher (0x%x) used in PTK derivation",
			   cipher);
		goto err;
	}

	ptk_len = ptk->kck_len + ptk->tk_len + ptk->kdk_len + ptk->kek_len;
	if (ptk_len > sizeof(tmp))
		goto err;

	*alg = pasn_select_hash_alg(akmp, cipher, pmk_len);

	switch (*alg) {
	case RSN_HASH_SHA512:
		wpa_printf(MSG_DEBUG, "PASN: SHA512 PTK derivation not supported");
		goto err;
	case RSN_HASH_SHA384:
#ifdef CONFIG_SHA384
		wpa_printf(MSG_DEBUG, "PASN: PTK derivation using SHA384");

		if (sha384_prf(pmk, pmk_len, label, data, data_len, tmp,
			       ptk_len) < 0)
			goto err;
		break;
#endif /* CONFIG_SHA384 */
	case RSN_HASH_SHA256:
		wpa_printf(MSG_DEBUG, "PASN: PTK derivation using SHA256");

		if (sha256_prf(pmk, pmk_len, label, data, data_len, tmp,
			       ptk_len) < 0)
			goto err;
		break;
	default:
		wpa_printf(MSG_DEBUG, "PASN: Unsupported hash algorithm %d",
			   *alg);
		goto err;
	}

	wpa_printf(MSG_DEBUG,
		   "PASN: PTK derivation: SPA=" MACSTR " BSSID=" MACSTR,
		   MAC2STR(spa), MAC2STR(bssid));

	wpa_hexdump_key(MSG_DEBUG, "PASN: DHss", dhss, dhss_len);
	wpa_hexdump_key(MSG_DEBUG, "PASN: PMK", pmk, pmk_len);
	wpa_hexdump_key(MSG_DEBUG, "PASN: PASN-PTK", tmp, ptk_len);

	os_memcpy(ptk->kck, tmp, WPA_PASN_KCK_LEN);
	wpa_hexdump_key(MSG_DEBUG, "PASN: KCK:", ptk->kck, WPA_PASN_KCK_LEN);
	pos = &tmp[WPA_PASN_KCK_LEN];

	if (kek_len) {
		os_memcpy(ptk->kek, pos, kek_len);
		wpa_hexdump_key(MSG_DEBUG, "PASN: KEK:",
				ptk->kek, ptk->kek_len);
		pos += kek_len;
	}

	os_memcpy(ptk->tk, pos, ptk->tk_len);
	wpa_hexdump_key(MSG_DEBUG, "PASN: TK:", ptk->tk, ptk->tk_len);
	pos += ptk->tk_len;

	if (kdk_len) {
		os_memcpy(ptk->kdk, pos, ptk->kdk_len);
		wpa_hexdump_key(MSG_DEBUG, "PASN: KDK:",
				ptk->kdk, ptk->kdk_len);
	}

	ptk->ptk_len = ptk_len;
	forced_memzero(tmp, sizeof(tmp));
	ret = 0;
err:
	bin_clear_free(data, data_len);
	return ret;
}


/*
 * pasn_mic_len - Returns the MIC length for PASN authentication
 * @alg: Selected hash algorithm from pasn_pmk_to_ptk()
 */
size_t pasn_mic_len(enum rsn_hash_alg alg)
{
	switch (alg) {
	case RSN_HASH_SHA512:
		return 32;
	case RSN_HASH_SHA384:
		return 24;
	case RSN_HASH_SHA256:
	default:
		return 16;
	}
}


/**
 * wpa_ltf_keyseed - Compute LTF keyseed from KDK
 * @ptk: Buffer that holds pairwise transient key
 * @akmp: Negotiated AKM
 * @cipher: Negotiated pairwise cipher
 * Returns: 0 on success, -1 on failure
 */
int wpa_ltf_keyseed(struct wpa_ptk *ptk, int akmp, int cipher)
{
	u8 *buf;
	size_t buf_len;
	u8 hash[SHA384_MAC_LEN];
	const u8 *kdk = ptk->kdk;
	size_t kdk_len = ptk->kdk_len;
	const char *label = "Secure LTF key seed";

	if (!kdk || !kdk_len) {
		wpa_printf(MSG_ERROR, "WPA: No KDK for LTF keyseed generation");
		return -1;
	}

	buf = (u8 *)label;
	buf_len = os_strlen(label);

	if (pasn_use_sha384(akmp, cipher)) {
		wpa_printf(MSG_DEBUG,
			   "WPA: Secure LTF keyseed using HMAC-SHA384");

		if (hmac_sha384(kdk, kdk_len, buf, buf_len, hash)) {
			wpa_printf(MSG_ERROR,
				   "WPA: HMAC-SHA384 compute failed");
			return -1;
		}
		os_memcpy(ptk->ltf_keyseed, hash, SHA384_MAC_LEN);
		ptk->ltf_keyseed_len = SHA384_MAC_LEN;
		wpa_hexdump_key(MSG_DEBUG, "WPA: Secure LTF keyseed: ",
				ptk->ltf_keyseed, ptk->ltf_keyseed_len);

	} else {
		wpa_printf(MSG_DEBUG, "WPA: LTF keyseed using HMAC-SHA256");

		if (hmac_sha256(kdk, kdk_len, buf, buf_len, hash)) {
			wpa_printf(MSG_ERROR,
				   "WPA: HMAC-SHA256 compute failed");
			return -1;
		}
		os_memcpy(ptk->ltf_keyseed, hash, SHA256_MAC_LEN);
		ptk->ltf_keyseed_len = SHA256_MAC_LEN;
		wpa_hexdump_key(MSG_DEBUG, "WPA: Secure LTF keyseed: ",
				ptk->ltf_keyseed, ptk->ltf_keyseed_len);
	}

	return 0;
}


/**
 * pasn_mic - Calculate PASN MIC
 * @alg: Selected hash algorithm from pasn_pmk_to_ptk()
 * @kck: The key confirmation key for the PASN PTKSA
 * @kck_len: KCK length in octets
 * @addr1: For the 2nd PASN frame supplicant address; for the 3rd frame the
 *	BSSID
 * @addr2: For the 2nd PASN frame the BSSID; for the 3rd frame the supplicant
 *	address
 * @data: For the 2nd PASN frame, RSNE concatenated with RSNXE (each full IE)
 *	as present in that Authentication frame, in order. For the 3rd PASN
 *	frame, this should hold the hash of the body of the PASN 1st frame.
 * @data_len: The length of data
 * @frame: The body of the PASN frame including the MIC element with the octets
 *	in the MIC field of the MIC element set to 0.
 * @frame_len: The length of frame
 * @mic: Buffer to hold the MIC on success. Should be big enough to handle the
 *	maximal MIC length
 * Returns: 0 on success, -1 on failure
 */
int pasn_mic(enum rsn_hash_alg alg, const u8 *kck, size_t kck_len,
	     const u8 *addr1, const u8 *addr2,
	     const u8 *data, size_t data_len,
	     const u8 *frame, size_t frame_len, u8 *mic)
{
	u8 *buf;
	u8 hash[SHA512_MAC_LEN];
	size_t buf_len = 2 * ETH_ALEN + data_len + frame_len;
	int ret = -1;
	size_t mic_len;

	if (!kck) {
		wpa_printf(MSG_ERROR, "PASN: No KCK for MIC calculation");
		return -1;
	}

	if (kck_len != WPA_PASN_KCK_LEN) {
		wpa_printf(MSG_ERROR,
			   "PASN: Unexpected KCK length %zu for MIC calculation",
			   kck_len);
		return -1;
	}

	if (!data || !data_len) {
		wpa_printf(MSG_ERROR, "PASN: invalid data for MIC calculation");
		return -1;
	}

	if (!frame || !frame_len) {
		wpa_printf(MSG_ERROR, "PASN: invalid data for MIC calculation");
		return -1;
	}

	buf = os_zalloc(buf_len);
	if (!buf)
		return -1;

	os_memcpy(buf, addr1, ETH_ALEN);
	os_memcpy(buf + ETH_ALEN, addr2, ETH_ALEN);

	wpa_hexdump_key(MSG_DEBUG, "PASN: MIC: data", data, data_len);
	os_memcpy(buf + 2 * ETH_ALEN, data, data_len);

	wpa_hexdump_key(MSG_DEBUG, "PASN: MIC: frame", frame, frame_len);
	os_memcpy(buf + 2 * ETH_ALEN + data_len, frame, frame_len);

	wpa_hexdump_key(MSG_DEBUG, "PASN: MIC: KCK", kck, kck_len);
	wpa_hexdump_key(MSG_DEBUG, "PASN: MIC: buf", buf, buf_len);

	mic_len = pasn_mic_len(alg);

	switch (alg) {
#ifdef CONFIG_SHA384
	case RSN_HASH_SHA384:
		wpa_printf(MSG_DEBUG, "PASN: MIC using HMAC-SHA384");
		if (hmac_sha384(kck, kck_len, buf, buf_len, hash))
			goto err;
		break;
#endif /* CONFIG_SHA384 */
	case RSN_HASH_SHA256:
		wpa_printf(MSG_DEBUG, "PASN: MIC using HMAC-SHA256");
		if (hmac_sha256(kck, kck_len, buf, buf_len, hash))
			goto err;
		break;
	default:
		wpa_printf(MSG_ERROR,
			   "PASN: Unsupported alg=%d for MIC calculation", alg);
		goto err;
	}

	os_memcpy(mic, hash, mic_len);
	wpa_hexdump_key(MSG_DEBUG, "PASN: MIC", mic, mic_len);

	ret = 0;
err:
	bin_clear_free(buf, buf_len);
	return ret;
}


/**
 * pasn_auth_frame_hash - Computes a hash of an Authentication frame body
 * @alg: Selected hash algorithm from pasn_pmk_to_ptk()
 * @data: Pointer to the Authentication frame body
 * @len: Length of the Authentication frame body
 * @hash: On return would hold the computed hash. Should be big enough to handle
 *	SHA512.
 * Returns: 0 on success, -1 on failure
 */
int pasn_auth_frame_hash(enum rsn_hash_alg alg, const u8 *data, size_t len,
			 u8 *hash)
{
	switch (alg) {
#ifdef CONFIG_SHA384
	case RSN_HASH_SHA384:
		wpa_printf(MSG_DEBUG, "PASN: Frame hash using SHA-384");
		return sha384_vector(1, &data, &len, hash);
#endif /* CONFIG_SHA384 */
	case RSN_HASH_SHA256:
		wpa_printf(MSG_DEBUG, "PASN: Frame hash using SHA-256");
		return sha256_vector(1, &data, &len, hash);
	default:
		wpa_printf(MSG_ERROR, "PASN: Unsupported alg=%d", alg);
		return -1;
	}
}

#endif /* CONFIG_PASN */
#endif // ESP_SUPPLICANT
