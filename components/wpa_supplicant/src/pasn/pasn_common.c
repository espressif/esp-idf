/*
 * PASN common processing
 *
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/wpa_common.h"
#include "common/sae.h"
#include "crypto/sha384.h"
#include "crypto/crypto.h"
#include "common/ieee802_11_defs.h"
#include "common/ieee802_11_common.h"
#include "crypto/aes_wrap.h"
#include "pasn_common.h"


struct pasn_data * pasn_data_init(void)
{
	struct pasn_data *pasn = os_zalloc(sizeof(struct pasn_data));

	return pasn;
}


void pasn_data_deinit(struct pasn_data *pasn)
{
	if (!pasn)
		return;
	os_free(pasn->rsnxe_ie);
	wpabuf_free(pasn->frame);
	os_free(pasn->pasn_groups);
	wpabuf_free(pasn->auth1);
	bin_clear_free(pasn, sizeof(struct pasn_data));
}


void pasn_register_callbacks(struct pasn_data *pasn, void *cb_ctx,
			     int (*send_mgmt)(void *ctx, const u8 *data,
					      size_t data_len, int noack,
					      unsigned int freq,
					      unsigned int wait),
			     int (*validate_custom_pmkid)(void *ctx,
							  const u8 *addr,
							  const u8 *pmkid))
{
	if (!pasn)
		return;

	pasn->cb_ctx = cb_ctx;
	pasn->send_mgmt = send_mgmt;
	pasn->validate_custom_pmkid = validate_custom_pmkid;
}


void pasn_enable_kdk_derivation(struct pasn_data *pasn)
{
	if (!pasn)
		return;
	pasn->derive_kdk = true;
	pasn->kdk_len = WPA_KDK_MAX_LEN;
}


void pasn_disable_kdk_derivation(struct pasn_data *pasn)
{
	if (!pasn)
		return;
	pasn->derive_kdk = false;
	pasn->kdk_len = 0;
}


void pasn_set_akmp(struct pasn_data *pasn, int akmp)
{
	if (!pasn)
		return;
	pasn->akmp = akmp;
}


void pasn_set_cipher(struct pasn_data *pasn, int cipher)
{
	if (!pasn)
		return;
	pasn->cipher = cipher;
}


void pasn_set_own_addr(struct pasn_data *pasn, const u8 *addr)
{
	if (!pasn || !addr)
		return;
	os_memcpy(pasn->own_addr, addr, ETH_ALEN);
}


void pasn_set_peer_addr(struct pasn_data *pasn, const u8 *addr)
{
	if (!pasn || !addr)
		return;
	os_memcpy(pasn->peer_addr, addr, ETH_ALEN);
}


void pasn_set_bssid(struct pasn_data *pasn, const u8 *addr)
{
	if (!pasn || !addr)
		return;
	os_memcpy(pasn->bssid, addr, ETH_ALEN);
}


int pasn_set_pt(struct pasn_data *pasn, struct sae_pt *pt)
{
	if (!pasn)
		return -1;
#ifdef CONFIG_SAE
	pasn->pt = pt;
	return 0;
#else /* CONFIG_SAE */
	return -1;
#endif /* CONFIG_SAE */
}


void pasn_set_password(struct pasn_data *pasn, const char *password)
{
	if (!pasn)
		return;
	pasn->password = password;
}


void pasn_set_wpa_key_mgmt(struct pasn_data *pasn, int key_mgmt)
{
	if (!pasn)
		return;
	pasn->wpa_key_mgmt = key_mgmt;
}


void pasn_set_rsn_pairwise(struct pasn_data *pasn, int rsn_pairwise)
{
	if (!pasn)
		return;
	pasn->rsn_pairwise = rsn_pairwise;
}


void pasn_set_rsnxe_caps(struct pasn_data *pasn, u16 rsnxe_capab)
{
	if (!pasn)
		return;
	pasn->rsnxe_capab = rsnxe_capab;
}


void pasn_set_rsnxe_ie(struct pasn_data *pasn, const u8 *rsnxe_ie)
{
	if (!pasn || !rsnxe_ie)
		return;
	pasn->rsnxe_ie = os_memdup(rsnxe_ie, 2 + rsnxe_ie[1]);
}


void pasn_set_custom_pmkid(struct pasn_data *pasn, const u8 *pmkid)
{
	if (!pasn || !pmkid)
		return;
	os_memcpy(pasn->custom_pmkid, pmkid, PMKID_LEN);
	pasn->custom_pmkid_valid = true;
}


int pasn_set_extra_ies(struct pasn_data *pasn, const u8 *extra_ies,
		       size_t extra_ies_len)
{
	if (!pasn || !extra_ies_len || !extra_ies)
		return -1;

	if (pasn->extra_ies) {
		os_free((u8 *) pasn->extra_ies);
		pasn->extra_ies_len = 0;
	}

	pasn->extra_ies = os_memdup(extra_ies, extra_ies_len);
	if (!pasn->extra_ies) {
		wpa_printf(MSG_ERROR,
			   "PASN: Extra IEs memory allocation failed");
		return -1;
	}
	pasn->extra_ies_len = extra_ies_len;
	return 0;
}


void pasn_set_noauth(struct pasn_data *pasn, bool noauth)
{
	if (!pasn)
		return;
	pasn->noauth = noauth;
}


int pasn_get_akmp(struct pasn_data *pasn)
{
	if (!pasn)
		return 0;
	return pasn->akmp;
}


int pasn_get_cipher(struct pasn_data *pasn)
{
	if (!pasn)
		return 0;
	return pasn->cipher;
}


size_t pasn_get_pmk_len(struct pasn_data *pasn)
{
	if (!pasn)
		return 0;
	return pasn->pmk_len;
}


u8 * pasn_get_pmk(struct pasn_data *pasn)
{
	if (!pasn)
		return NULL;
	return pasn->pmk;
}


struct wpa_ptk * pasn_get_ptk(struct pasn_data *pasn)
{
	if (!pasn)
		return NULL;
	return &pasn->ptk;
}


int pasn_add_encrypted_data(struct pasn_data *pasn, struct wpabuf *buf,
			    const u8 *data, size_t data_len)
{
	int ret;
	u8 *encrypted_data, *padded_data = NULL;
	u8 *len;
	size_t pad_len = 0;

	if (!pasn->ptk.kek_len) {
		wpa_printf(MSG_DEBUG, "PASN: KEK not available");
		return -2;
	}

	pad_len = data_len % 8;
	if (pad_len) {
		pad_len = 8 - pad_len;
		padded_data = os_zalloc(data_len + pad_len);
		if (!padded_data)
			return -1;
		os_memcpy(padded_data, data, data_len);
		data = padded_data;
		padded_data[data_len] = 0xdd;
	}
	data_len += pad_len + 8;

	encrypted_data = os_malloc(data_len);
	if (!encrypted_data) {
		os_free(padded_data);
		return -1;
	}

	ret = aes_wrap(pasn->ptk.kek, pasn->ptk.kek_len,
		       (data_len - 8) / 8, data, encrypted_data);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: AES wrap failed, ret=%d", ret);
		goto out;
	}

	if (wpabuf_tailroom(buf) < 1 + 1 + 1 + data_len) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Not enough room in the buffer for PASN Encrypred Data element");
		ret = -1;
		goto out;
	}

	wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
	len = wpabuf_put(buf, 1);

	wpabuf_put_u8(buf, WLAN_EID_EXT_PASN_ENCRYPTED_DATA);

	wpabuf_put_data(buf, encrypted_data, data_len);
	*len = (u8 *) wpabuf_put(buf, 0) - len - 1;

out:
	os_free(padded_data);
	os_free(encrypted_data);
	return ret;
}


int pasn_parse_encrypted_data(struct pasn_data *pasn, const u8 *data,
			      size_t len)
{
	int ret = -1;
	u8 *buf;
	u16 buf_len;
	struct ieee802_11_elems elems;
	const struct ieee80211_auth *mgmt =
		(const struct ieee80211_auth *) data;

	if (len < 24 + 6 ||
	    ieee802_11_parse_elems(mgmt->auth.variable,
				   len - offsetof(struct ieee80211_auth,
						  auth.variable),
				   &elems, 0) == ParseFailed) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Failed parsing Authentication frame");
		return -1;
	}

	if (!elems.pasn_encrypted_data || elems.pasn_encrypted_data_len < 8 ||
	    elems.pasn_encrypted_data_len % 8) {
		wpa_printf(MSG_DEBUG, "PASN: No encrypted elements");
		return 0;
	}

	buf_len = elems.pasn_encrypted_data_len - 8;

	buf = os_malloc(buf_len);
	if (!buf)
		return -1;

	ret = aes_unwrap(pasn->ptk.kek, pasn->ptk.kek_len, buf_len / 8,
			 elems.pasn_encrypted_data, buf);
	if (ret)
		wpa_printf(MSG_DEBUG, "PASN: AES unwrap failed, ret=%d", ret);
	else if (pasn->parse_data_element && pasn->cb_ctx)
		ret = pasn->parse_data_element(pasn->cb_ctx, buf, buf_len);

	os_free(buf);
	return ret;
}
