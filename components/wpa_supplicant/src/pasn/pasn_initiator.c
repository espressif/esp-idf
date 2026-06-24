/*
 * PASN initiator processing
 *
 * Copyright (C) 2019, Intel Corporation
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "common/wpa_common.h"
#include "common/sae.h"
#include "common/ieee802_11_common.h"
#include "common/ieee802_11_defs.h"
#include "common/dragonfly.h"
#include "crypto/sha384.h"
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "eap_peer/eap_defs.h"
#include "rsn_supp/wpa.h"
#include "rsn_supp/pmksa_cache.h"
#include "pasn_common.h"


struct rsn_pmksa_cache * pasn_initiator_pmksa_cache_init(void)
{
	return pmksa_cache_init(NULL, NULL, NULL);
}


void pasn_initiator_pmksa_cache_deinit(struct rsn_pmksa_cache *pmksa)
{
	return pmksa_cache_deinit(pmksa);
}


int pasn_initiator_pmksa_cache_add(struct rsn_pmksa_cache *pmksa,
				   const u8 *own_addr, const u8 *bssid,
				   const u8 *pmk,
				   size_t pmk_len, const u8 *pmkid)
{
	if (pmksa_cache_add(pmksa, pmk, pmk_len, pmkid, NULL, 0, bssid,
			    own_addr, NULL, WPA_KEY_MGMT_SAE))
		return 0;
	return -1;
}


void pasn_initiator_pmksa_cache_remove(struct rsn_pmksa_cache *pmksa,
				       const u8 *bssid)
{
	struct rsn_pmksa_cache_entry *entry;

	entry = pmksa_cache_get(pmksa, bssid, NULL, NULL);
	if (!entry)
		return;

	pmksa_cache_remove(pmksa, entry);
}


int pasn_initiator_pmksa_cache_get(struct rsn_pmksa_cache *pmksa,
				   const u8 *bssid, u8 *pmkid, u8 *pmk,
				   size_t *pmk_len)
{
	struct rsn_pmksa_cache_entry *entry;

	entry = pmksa_cache_get(pmksa, bssid, NULL, NULL);
	if (entry) {
		os_memcpy(pmkid, entry->pmkid, PMKID_LEN);
		os_memcpy(pmk, entry->pmk, entry->pmk_len);
		*pmk_len = entry->pmk_len;
		return 0;
	}
	return -1;
}


void pasn_initiator_pmksa_cache_flush(struct rsn_pmksa_cache *pmksa)
{
	return pmksa_cache_flush(pmksa, NULL, NULL, 0);
}


void pasn_set_initiator_pmksa(struct pasn_data *pasn,
			      struct rsn_pmksa_cache *pmksa)
{
	if (pasn)
		pasn->pmksa = pmksa;
}


#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)

static struct wpabuf * wpas_pasn_wd_sae_commit(struct pasn_data *pasn)
{
	struct wpabuf *buf = NULL;
	int ret;

	ret = sae_set_group(&pasn->sae, pasn->group);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to set SAE group");
		return NULL;
	}

	pasn->sae.akmp = pasn->akmp;
	ret = sae_prepare_commit_pt(&pasn->sae, pasn->pt,
				    pasn->own_addr, pasn->peer_addr,
				    NULL, NULL);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to prepare SAE commit");
		return NULL;
	}

	/* Need to add the entire Authentication frame body */
	buf = wpabuf_alloc(6 + SAE_COMMIT_MAX_LEN);
	if (!buf) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to allocate SAE buffer");
		return NULL;
	}

	wpabuf_put_le16(buf, WLAN_AUTH_SAE);
	wpabuf_put_le16(buf, 1);
	wpabuf_put_le16(buf, WLAN_STATUS_SAE_HASH_TO_ELEMENT);

	sae_write_commit(&pasn->sae, buf, NULL, NULL);
	pasn->sae.state = SAE_COMMITTED;

	return buf;
}


static int wpas_pasn_wd_sae_rx(struct pasn_data *pasn, struct wpabuf *wd)
{
	const u8 *data;
	size_t buf_len;
	u16 len, res, alg, seq, status;
	int groups[] = { pasn->group, 0 };
	int ret;

	if (!wd)
		return -1;

	data = wpabuf_head_u8(wd);
	buf_len = wpabuf_len(wd);

	/* first handle the commit message */
	if (buf_len < 2) {
		wpa_printf(MSG_DEBUG, "PASN: SAE buffer too short (commit)");
		return -1;
	}

	len = WPA_GET_LE16(data);
	if (len < 6 || buf_len - 2 < len) {
		wpa_printf(MSG_DEBUG, "PASN: SAE buffer too short for commit");
		return -1;
	}

	buf_len -= 2;
	data += 2;

	alg = WPA_GET_LE16(data);
	seq = WPA_GET_LE16(data + 2);
	status = WPA_GET_LE16(data + 4);

	wpa_printf(MSG_DEBUG, "PASN: SAE: commit: alg=%u, seq=%u, status=%u",
		   alg, seq, status);

	if (alg != WLAN_AUTH_SAE || seq != 1 ||
	    status != WLAN_STATUS_SAE_HASH_TO_ELEMENT) {
		wpa_printf(MSG_DEBUG, "PASN: SAE: dropping peer commit");
		return -1;
	}

	res = sae_parse_commit(&pasn->sae, data + 6, len - 6, NULL, NULL,
			       groups, 1);
	if (res != WLAN_STATUS_SUCCESS) {
		wpa_printf(MSG_DEBUG, "PASN: SAE failed parsing commit");
		return -1;
	}

	/* Process the commit message and derive the PMK */
	ret = sae_process_commit(&pasn->sae);
	if (ret) {
		wpa_printf(MSG_DEBUG, "SAE: Failed to process peer commit");
		return -1;
	}

	buf_len -= len;
	data += len;

	/* Handle the confirm message */
	if (buf_len < 2) {
		wpa_printf(MSG_DEBUG, "PASN: SAE buffer too short (confirm)");
		return -1;
	}

	len = WPA_GET_LE16(data);
	if (len < 6 || buf_len - 2 < len) {
		wpa_printf(MSG_DEBUG, "PASN: SAE buffer too short for confirm");
		return -1;
	}

	buf_len -= 2;
	data += 2;

	alg = WPA_GET_LE16(data);
	seq = WPA_GET_LE16(data + 2);
	status = WPA_GET_LE16(data + 4);

	wpa_printf(MSG_DEBUG, "PASN: SAE confirm: alg=%u, seq=%u, status=%u",
		   alg, seq, status);

	if (alg != WLAN_AUTH_SAE || seq != 2 || status != WLAN_STATUS_SUCCESS) {
		wpa_printf(MSG_DEBUG, "PASN: Dropping peer SAE confirm");
		return -1;
	}

	res = sae_check_confirm(&pasn->sae, data + 6, len - 6);
	if (res != WLAN_STATUS_SUCCESS) {
		wpa_printf(MSG_DEBUG, "PASN: SAE failed checking confirm");
		return -1;
	}

	wpa_printf(MSG_DEBUG, "PASN: SAE completed successfully");
	pasn->sae.state = SAE_ACCEPTED;

	return 0;
}


static struct wpabuf * wpas_pasn_wd_sae_confirm(struct pasn_data *pasn)
{
	struct wpabuf *buf = NULL;

	/* Need to add the entire authentication frame body */
	buf = wpabuf_alloc(6 + SAE_CONFIRM_MAX_LEN);
	if (!buf) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to allocate SAE buffer");
		return NULL;
	}

	wpabuf_put_le16(buf, WLAN_AUTH_SAE);
	wpabuf_put_le16(buf, 2);
	wpabuf_put_le16(buf, WLAN_STATUS_SUCCESS);

	sae_write_confirm(&pasn->sae, buf);
	pasn->sae.state = SAE_CONFIRMED;

	return buf;
}

#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */


#ifndef ESP_SUPPLICANT
#ifdef CONFIG_FILS

static struct wpabuf * wpas_pasn_fils_build_auth(struct pasn_data *pasn)
{
	struct wpabuf *buf = NULL;
	struct wpabuf *erp_msg;
	int ret;

	erp_msg = eapol_sm_build_erp_reauth_start(pasn->eapol);
	if (!erp_msg) {
		wpa_printf(MSG_DEBUG,
			   "PASN: FILS: ERP EAP-Initiate/Re-auth unavailable");
		return NULL;
	}

	if (random_get_bytes(pasn->fils.nonce, FILS_NONCE_LEN) < 0 ||
	    random_get_bytes(pasn->fils.session, FILS_SESSION_LEN) < 0)
		goto fail;

	wpa_hexdump(MSG_DEBUG, "PASN: FILS: Nonce", pasn->fils.nonce,
		    FILS_NONCE_LEN);

	wpa_hexdump(MSG_DEBUG, "PASN: FILS: Session", pasn->fils.session,
		    FILS_SESSION_LEN);

	buf = wpabuf_alloc(1500);
	if (!buf)
		goto fail;

	/* Add the authentication algorithm */
	wpabuf_put_le16(buf, WLAN_AUTH_FILS_SK);

	/* Authentication Transaction seq# */
	wpabuf_put_le16(buf, WLAN_AUTH_TR_SEQ_PASN_AUTH1);

	/* Status Code */
	wpabuf_put_le16(buf, WLAN_STATUS_SUCCESS);

	/* Own RSNE */
	wpa_pasn_add_rsne(buf, NULL, pasn->akmp, pasn->cipher);

	/* FILS Nonce */
	wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
	wpabuf_put_u8(buf, 1 + FILS_NONCE_LEN);
	wpabuf_put_u8(buf, WLAN_EID_EXT_FILS_NONCE);
	wpabuf_put_data(buf, pasn->fils.nonce, FILS_NONCE_LEN);

	/* FILS Session */
	wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
	wpabuf_put_u8(buf, 1 + FILS_SESSION_LEN);
	wpabuf_put_u8(buf, WLAN_EID_EXT_FILS_SESSION);
	wpabuf_put_data(buf, pasn->fils.session, FILS_SESSION_LEN);

	/* Wrapped Data (ERP) */
	wpabuf_put_u8(buf, WLAN_EID_EXTENSION);
	wpabuf_put_u8(buf, 1 + wpabuf_len(erp_msg));
	wpabuf_put_u8(buf, WLAN_EID_EXT_WRAPPED_DATA);
	wpabuf_put_buf(buf, erp_msg);

	/*
	 * Calculate pending PMKID here so that we do not need to maintain a
	 * copy of the EAP-Initiate/Reauth message.
	 */
	ret = fils_pmkid_erp(pasn->akmp, wpabuf_head(erp_msg),
			     wpabuf_len(erp_msg),
			     pasn->fils.erp_pmkid);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Failed to get ERP PMKID");
		goto fail;
	}

	wpabuf_free(erp_msg);
	erp_msg = NULL;

	wpa_hexdump_buf(MSG_DEBUG, "PASN: FILS: Authentication frame", buf);
	return buf;
fail:
	wpabuf_free(erp_msg);
	wpabuf_free(buf);
	return NULL;
}


static struct wpabuf * wpas_pasn_wd_fils_auth(struct pasn_data *pasn)
{
	wpa_printf(MSG_DEBUG, "PASN: FILS: wrapped data - completed=%u",
		   pasn->fils.completed);

	/* Nothing to add as we are done */
	if (pasn->fils.completed)
		return NULL;

	if (!pasn->fils_eapol) {
		wpa_printf(MSG_DEBUG,
			   "PASN: FILS: Missing Indication IE or PFS");
		return NULL;
	}

	return wpas_pasn_fils_build_auth(pasn);
}


static int wpas_pasn_wd_fils_rx(struct pasn_data *pasn, struct wpabuf *wd)
{
	struct ieee802_11_elems elems;
	struct wpa_ie_data rsne_data;
	u8 rmsk[ERP_MAX_KEY_LEN];
	size_t rmsk_len;
	u8 anonce[FILS_NONCE_LEN];
	const u8 *data;
	size_t buf_len;
	struct wpabuf *fils_wd = NULL;
	u16 alg, seq, status;
	int ret;

	if (!wd)
		return -1;

	data = wpabuf_head(wd);
	buf_len = wpabuf_len(wd);

	wpa_hexdump(MSG_DEBUG, "PASN: FILS: Authentication frame len=%zu",
		    data, buf_len);

	/* first handle the header */
	if (buf_len < 6) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Buffer too short");
		return -1;
	}

	alg = WPA_GET_LE16(data);
	seq = WPA_GET_LE16(data + 2);
	status = WPA_GET_LE16(data + 4);

	wpa_printf(MSG_DEBUG, "PASN: FILS: commit: alg=%u, seq=%u, status=%u",
		   alg, seq, status);

	if (alg != WLAN_AUTH_FILS_SK || seq != 2 ||
	    status != WLAN_STATUS_SUCCESS) {
		wpa_printf(MSG_DEBUG,
			   "PASN: FILS: Dropping peer authentication");
		return -1;
	}

	data += 6;
	buf_len -= 6;

	if (ieee802_11_parse_elems(data, buf_len, &elems, 1) == ParseFailed) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Could not parse elements");
		return -1;
	}

	if (!elems.rsn_ie || !elems.fils_nonce || !elems.fils_nonce ||
	    !elems.wrapped_data) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Missing IEs");
		return -1;
	}

	ret = wpa_parse_wpa_ie(elems.rsn_ie - 2, elems.rsn_ie_len + 2,
			       &rsne_data);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Failed parsing RSNE");
		return -1;
	}

	ret = wpa_pasn_validate_rsne(&rsne_data);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Failed validating RSNE");
		return -1;
	}

	if (rsne_data.num_pmkid) {
		wpa_printf(MSG_DEBUG,
			   "PASN: FILS: Not expecting PMKID in RSNE");
		return -1;
	}

	wpa_hexdump(MSG_DEBUG, "PASN: FILS: ANonce", elems.fils_nonce,
		    FILS_NONCE_LEN);
	os_memcpy(anonce, elems.fils_nonce, FILS_NONCE_LEN);

	wpa_hexdump(MSG_DEBUG, "PASN: FILS: FILS Session", elems.fils_session,
		    FILS_SESSION_LEN);

	if (os_memcmp(pasn->fils.session, elems.fils_session,
		      FILS_SESSION_LEN)) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Session mismatch");
		return -1;
	}

	fils_wd = ieee802_11_defrag(elems.wrapped_data, elems.wrapped_data_len,
				    true);

	if (!fils_wd) {
		wpa_printf(MSG_DEBUG,
			   "PASN: FILS: Failed getting wrapped data");
		return -1;
	}

	eapol_sm_process_erp_finish(pasn->eapol, wpabuf_head(fils_wd),
				    wpabuf_len(fils_wd));

	wpabuf_free(fils_wd);
	fils_wd = NULL;

	if (eapol_sm_failed(pasn->eapol)) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: ERP finish failed");
		return -1;
	}

	rmsk_len = ERP_MAX_KEY_LEN;
	ret = eapol_sm_get_key(pasn->eapol, rmsk, rmsk_len);

	if (ret == PMK_LEN) {
		rmsk_len = PMK_LEN;
		ret = eapol_sm_get_key(pasn->eapol, rmsk, rmsk_len);
	}

	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Failed getting RMSK");
		return -1;
	}

	ret = fils_rmsk_to_pmk(pasn->akmp, rmsk, rmsk_len,
			       pasn->fils.nonce, anonce, NULL, 0,
			       pasn->pmk, &pasn->pmk_len);

	forced_memzero(rmsk, sizeof(rmsk));

	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: FILS: Failed to derive PMK");
		return -1;
	}

	wpa_hexdump(MSG_DEBUG, "PASN: FILS: PMKID", pasn->fils.erp_pmkid,
		    PMKID_LEN);

	wpa_printf(MSG_DEBUG, "PASN: FILS: ERP processing succeeded");

	pasn->pmksa_entry = pmksa_cache_add(pasn->pmksa, pasn->pmk,
					    pasn->pmk_len, pasn->fils.erp_pmkid,
					    NULL, 0, pasn->peer_addr,
					    pasn->own_addr, NULL,
					    pasn->akmp);

	pasn->fils.completed = true;
	return 0;
}

#endif /* CONFIG_FILS */
#endif /* ESP_SUPPLICANT */


static struct wpabuf * wpas_pasn_get_wrapped_data(struct pasn_data *pasn)
{
	if (pasn->using_pmksa)
		return NULL;

	switch (pasn->akmp) {
	case WPA_KEY_MGMT_PASN:
		/* no wrapped data */
		return NULL;
	case WPA_KEY_MGMT_SAE:
	case WPA_KEY_MGMT_SAE_EXT_KEY:
#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
		if (pasn->trans_seq == 0)
			return wpas_pasn_wd_sae_commit(pasn);
		if (pasn->trans_seq == WLAN_AUTH_TR_SEQ_SAE_CONFIRM)
			return wpas_pasn_wd_sae_confirm(pasn);
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */
		wpa_printf(MSG_ERROR,
			   "PASN: SAE: Cannot derive wrapped data");
		return NULL;
#ifndef ESP_SUPPLICANT
	case WPA_KEY_MGMT_FILS_SHA256:
	case WPA_KEY_MGMT_FILS_SHA384:
#ifdef CONFIG_FILS
		return wpas_pasn_wd_fils_auth(pasn);
#endif /* CONFIG_FILS */
#endif /* ESP_SUPPLICANT */
	case WPA_KEY_MGMT_FT_PSK:
	case WPA_KEY_MGMT_FT_IEEE8021X:
	case WPA_KEY_MGMT_FT_IEEE8021X_SHA384:
		/*
		 * Wrapped data with these AKMs is optional and is only needed
		 * for further validation of FT security parameters. For now do
		 * not use them.
		 */
		return NULL;
	default:
		wpa_printf(MSG_ERROR,
			   "PASN: TODO: Wrapped data for akmp=0x%x",
			   pasn->akmp);
		return NULL;
	}
}


static u8 wpas_pasn_get_wrapped_data_format(struct pasn_data *pasn)
{
	if (pasn->using_pmksa)
		return WPA_PASN_WRAPPED_DATA_NO;

	/* Note: Valid AKMP is expected to already be validated */
	switch (pasn->akmp) {
	case WPA_KEY_MGMT_SAE:
	case WPA_KEY_MGMT_SAE_EXT_KEY:
		return WPA_PASN_WRAPPED_DATA_SAE;
#ifndef ESP_SUPPLICANT
	case WPA_KEY_MGMT_FILS_SHA256:
	case WPA_KEY_MGMT_FILS_SHA384:
		return WPA_PASN_WRAPPED_DATA_FILS_SK;
#endif /* ESP_SUPPLICANT */
	case WPA_KEY_MGMT_FT_PSK:
	case WPA_KEY_MGMT_FT_IEEE8021X:
	case WPA_KEY_MGMT_FT_IEEE8021X_SHA384:
		/*
		 * Wrapped data with these AKMs is optional and is only needed
		 * for further validation of FT security parameters. For now do
		 * not use them.
		 */
		return WPA_PASN_WRAPPED_DATA_NO;
	case WPA_KEY_MGMT_PASN:
	default:
		return WPA_PASN_WRAPPED_DATA_NO;
	}
}


static struct wpabuf * wpas_pasn_build_auth_1(struct pasn_data *pasn,
					      const struct wpabuf *comeback,
					      bool verify)
{
	struct wpabuf *buf, *pubkey = NULL, *wrapped_data_buf = NULL;
	const u8 *pmkid;
	u8 wrapped_data;

	wpa_printf(MSG_DEBUG, "PASN: Building frame 1");

	if (pasn->trans_seq)
		return NULL;

	buf = wpabuf_alloc(1500);
	if (!buf)
		goto fail;

	/* Get public key */
	pubkey = crypto_ecdh_get_pubkey(pasn->ecdh, 0);
	pubkey = wpabuf_zeropad(pubkey, crypto_ecdh_prime_len(pasn->ecdh));
	if (!pubkey) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to get pubkey");
		goto fail;
	}

	wrapped_data = wpas_pasn_get_wrapped_data_format(pasn);

	wpa_pasn_build_auth_header(buf, pasn->bssid,
				   pasn->own_addr, pasn->peer_addr,
				   pasn->trans_seq + 1, WLAN_STATUS_SUCCESS);

	pmkid = NULL;
	if (wpa_key_mgmt_ft(pasn->akmp)) {
#ifdef CONFIG_IEEE80211R
		pmkid = pasn->pmk_r1_name;
#else /* CONFIG_IEEE80211R */
		goto fail;
#endif /* CONFIG_IEEE80211R */
	} else if (wrapped_data != WPA_PASN_WRAPPED_DATA_NO) {
		struct rsn_pmksa_cache_entry *pmksa;

		pmksa = pmksa_cache_get(pasn->pmksa, pasn->peer_addr, NULL, NULL);
		if (pmksa && pasn->custom_pmkid_valid)
			pmkid = pasn->custom_pmkid;
		else if (pmksa)
			pmkid = pmksa->pmkid;

		/*
		 * Note: Even when PMKSA is available, also add wrapped data as
		 * it is possible that the PMKID is no longer valid at the AP.
		 */
		if (!verify)
			wrapped_data_buf = wpas_pasn_get_wrapped_data(pasn);
	}

	if (wpa_pasn_add_rsne(buf, pmkid, pasn->akmp, pasn->cipher) < 0)
		goto fail;

	if (!wrapped_data_buf)
		wrapped_data = WPA_PASN_WRAPPED_DATA_NO;

	if (wpa_pasn_add_wrapped_data(buf, wrapped_data_buf) < 0)
		goto fail;

	if (pasn->rsnxe_ie){
		wpabuf_put_data(buf, pasn->rsnxe_ie, 2 + pasn->rsnxe_ie[1]);
	}
	else {
		wpa_pasn_add_rsnxe(buf, pasn->rsnxe_capab);
	}

	wpa_pasn_add_parameter_ie(buf, pasn->group, wrapped_data,
				  pubkey, true, comeback, -1);

	wpa_pasn_add_extra_ies(buf, pasn->extra_ies, pasn->extra_ies_len);

	wpabuf_free(pasn->auth1);
	pasn->auth1 = wpabuf_alloc_copy(wpabuf_head_u8(buf) + IEEE80211_HDRLEN,
					wpabuf_len(buf) - IEEE80211_HDRLEN);
	if (!pasn->auth1) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to store a copy of Auth1");
		goto fail;
	}

	pasn->trans_seq++;

	wpabuf_free(wrapped_data_buf);
	wpabuf_free(pubkey);

	wpa_printf(MSG_DEBUG, "PASN: Frame 1: Success");
	return buf;
fail:
	pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
	wpabuf_free(wrapped_data_buf);
	wpabuf_free(pubkey);
	wpabuf_free(buf);
	return NULL;
}


static struct wpabuf * wpas_pasn_build_auth_3(struct pasn_data *pasn)
{
	struct wpabuf *buf, *wrapped_data_buf = NULL;
	u8 mic[WPA_PASN_MAX_MIC_LEN];
	u8 mic_len;
	size_t data_len;
	const u8 *data;
	u8 *ptr;
	u8 wrapped_data;
	int ret;
	u8 hash[SHA512_MAC_LEN];

	wpa_printf(MSG_DEBUG, "PASN: Building frame 3");

	if (pasn->trans_seq != WLAN_AUTH_TR_SEQ_PASN_AUTH2)
		return NULL;

	buf = wpabuf_alloc(1500);
	if (!buf)
		goto fail;

	wrapped_data = wpas_pasn_get_wrapped_data_format(pasn);

	wpa_pasn_build_auth_header(buf, pasn->bssid,
				   pasn->own_addr, pasn->peer_addr,
				   WLAN_AUTH_TR_SEQ_PASN_AUTH3,
				   WLAN_STATUS_SUCCESS);

	wrapped_data_buf = wpas_pasn_get_wrapped_data(pasn);

	if (!wrapped_data_buf)
		wrapped_data = WPA_PASN_WRAPPED_DATA_NO;

	if (wpa_pasn_add_wrapped_data(buf, wrapped_data_buf) < 0)
		goto fail;
	wpabuf_free(wrapped_data_buf);
	wrapped_data_buf = NULL;

	wpa_pasn_add_parameter_ie(buf, pasn->group, wrapped_data,
				  NULL, false, NULL, -1);

	if (pasn->prepare_data_element && pasn->cb_ctx)
		pasn->prepare_data_element(pasn->cb_ctx, pasn->peer_addr);

	wpa_pasn_add_extra_ies(buf, pasn->extra_ies, pasn->extra_ies_len);

	/* Add the MIC */
	mic_len = pasn_mic_len(pasn->hash_alg);
	wpabuf_put_u8(buf, WLAN_EID_MIC);
	wpabuf_put_u8(buf, mic_len);
	ptr = wpabuf_put(buf, mic_len);

	os_memset(ptr, 0, mic_len);

	data = wpabuf_head_u8(buf) + IEEE80211_HDRLEN;
	data_len = wpabuf_len(buf) - IEEE80211_HDRLEN;

	if (!pasn->auth1 ||
	    pasn_auth_frame_hash(pasn->hash_alg, wpabuf_head(pasn->auth1),
				 wpabuf_len(pasn->auth1), hash)) {
		wpa_printf(MSG_INFO, "PASN: Failed to calculate Auth1 hash");
		goto fail;
	}

	ret = pasn_mic(pasn->hash_alg, pasn->ptk.kck, pasn->ptk.kck_len,
		       pasn->own_addr, pasn->peer_addr,
		       hash, mic_len * 2, data, data_len, mic);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: frame 3: Failed MIC calculation");
		goto fail;
	}

#ifdef CONFIG_TESTING_OPTIONS
	if (pasn->corrupt_mic) {
		wpa_printf(MSG_DEBUG, "PASN: frame 3: Corrupt MIC");
		mic[0] = ~mic[0];
	}
#endif /* CONFIG_TESTING_OPTIONS */

	os_memcpy(ptr, mic, mic_len);

	pasn->trans_seq++;

	wpa_printf(MSG_DEBUG, "PASN: frame 3: Success");
	return buf;
fail:
	pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
	wpabuf_free(wrapped_data_buf);
	wpabuf_free(buf);
	return NULL;
}


void wpa_pasn_reset(struct pasn_data *pasn)
{
	wpa_printf(MSG_DEBUG, "PASN: Reset");

	crypto_ecdh_deinit(pasn->ecdh);
	pasn->ecdh = NULL;


	pasn->akmp = 0;
	pasn->cipher = 0;
	pasn->group = 0;
	pasn->trans_seq = 0;
	pasn->pmk_len = 0;
	pasn->using_pmksa = false;

	forced_memzero(pasn->pmk, sizeof(pasn->pmk));
	forced_memzero(&pasn->ptk, sizeof(pasn->ptk));

	wpabuf_free(pasn->auth1);
	pasn->auth1 = NULL;

	wpabuf_free(pasn->comeback);
	pasn->comeback = NULL;
	pasn->comeback_after = 0;

#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
	sae_clear_data(&pasn->sae);
	if (pasn->pt) {
		sae_deinit_pt(pasn->pt);
		pasn->pt = NULL;
	}
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */

#ifndef ESP_SUPPLICANT
#ifdef CONFIG_FILS
	pasn->fils_eapol = false;
	os_memset(&pasn->fils, 0, sizeof(pasn->fils));
#endif /* CONFIG_FILS*/
#endif /* ESP_SUPPLICANT */

#ifdef CONFIG_IEEE80211R
	forced_memzero(pasn->pmk_r1, sizeof(pasn->pmk_r1));
	pasn->pmk_r1_len = 0;
	os_memset(pasn->pmk_r1_name, 0, sizeof(pasn->pmk_r1_name));
#endif /* CONFIG_IEEE80211R */
	pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
	pasn->pmksa_entry = NULL;
#ifdef CONFIG_TESTING_OPTIONS
	pasn->corrupt_mic = 0;
#endif /* CONFIG_TESTING_OPTIONS */
	pasn->network_id = 0;
	pasn->derive_kdk = false;
	pasn->rsn_ie = NULL;
	pasn->rsn_ie_len = 0;
	os_free(pasn->rsnxe_ie);
	pasn->rsnxe_ie = NULL;
	pasn->custom_pmkid_valid = false;

	if (pasn->extra_ies) {
		os_free((u8 *) pasn->extra_ies);
		pasn->extra_ies = NULL;
	}

	wpabuf_free(pasn->frame);
	pasn->frame = NULL;

	wpabuf_free(pasn->auth1);
	pasn->auth1 = NULL;
}


static int wpas_pasn_set_pmk(struct pasn_data *pasn,
			     struct wpa_ie_data *rsn_data,
			     struct wpa_pasn_params_data *pasn_data,
			     struct wpabuf *wrapped_data)
{
	static const u8 pasn_default_pmk[] = {'P', 'M', 'K', 'z'};

	os_memset(pasn->pmk, 0, sizeof(pasn->pmk));
	pasn->pmk_len = 0;

	if (pasn->akmp == WPA_KEY_MGMT_PASN) {
		wpa_printf(MSG_DEBUG, "PASN: Using default PMK");

		pasn->pmk_len = WPA_PASN_PMK_LEN;
		os_memcpy(pasn->pmk, pasn_default_pmk,
			  sizeof(pasn_default_pmk));
		return 0;
	}

	if (wpa_key_mgmt_ft(pasn->akmp)) {
#ifdef CONFIG_IEEE80211R
		wpa_printf(MSG_DEBUG, "PASN: FT: Using PMK-R1");
		pasn->pmk_len = pasn->pmk_r1_len;
		os_memcpy(pasn->pmk, pasn->pmk_r1, pasn->pmk_r1_len);
		pasn->using_pmksa = true;
		return 0;
#else /* CONFIG_IEEE80211R */
		wpa_printf(MSG_DEBUG, "PASN: FT: Not supported");
		return -1;
#endif /* CONFIG_IEEE80211R */
	}

	if (rsn_data->num_pmkid) {
		int ret;
		struct rsn_pmksa_cache_entry *pmksa;
		const u8 *pmkid = NULL;

		if (pasn->custom_pmkid_valid) {
			ret = pasn->validate_custom_pmkid(pasn->cb_ctx,
							  pasn->peer_addr,
							  rsn_data->pmkid);
			if (ret) {
				wpa_printf(MSG_DEBUG,
					   "PASN: Failed custom PMKID validation");
				return -1;
			}
		} else {
			pmkid = rsn_data->pmkid;
		}

		pmksa = pmksa_cache_get(pasn->pmksa, pasn->peer_addr,
					pmkid, NULL);
		if (pmksa) {
			wpa_printf(MSG_DEBUG, "PASN: Using PMKSA");

			pasn->pmk_len = pmksa->pmk_len;
			os_memcpy(pasn->pmk, pmksa->pmk, pmksa->pmk_len);
			pasn->using_pmksa = true;

			return 0;
		}
	}

#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
	if (pasn->akmp == WPA_KEY_MGMT_SAE ||
	    pasn->akmp == WPA_KEY_MGMT_SAE_EXT_KEY) {
		int ret;

		ret = wpas_pasn_wd_sae_rx(pasn, wrapped_data);
		if (ret) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Failed processing SAE wrapped data");
			pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			return -1;
		}

		wpa_printf(MSG_DEBUG, "PASN: Success deriving PMK with SAE");
		pasn->pmk_len = pasn->sae.pmk_len;
		os_memcpy(pasn->pmk, pasn->sae.pmk, pasn->pmk_len);

		pasn->pmksa_entry = pmksa_cache_add(pasn->pmksa, pasn->pmk,
						    pasn->pmk_len,
						    pasn->sae.pmkid,
						    NULL, 0, pasn->peer_addr,
						    pasn->own_addr, NULL,
						    pasn->akmp);
		return 0;
	}
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */

#ifndef ESP_SUPPLICANT
#ifdef CONFIG_FILS
	if (pasn->akmp == WPA_KEY_MGMT_FILS_SHA256 ||
	    pasn->akmp == WPA_KEY_MGMT_FILS_SHA384) {
		int ret;

		ret = wpas_pasn_wd_fils_rx(pasn, wrapped_data);
		if (ret) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Failed processing FILS wrapped data");
			pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
			return -1;
		}

		return 0;
	}
#endif	/* CONFIG_FILS */
#endif /* ESP_SUPPLICANT */

	/* TODO: Derive PMK based on wrapped data */
	wpa_printf(MSG_DEBUG, "PASN: Missing implementation to derive PMK");
	pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
	return -1;
}


static int wpas_pasn_send_auth_1(struct pasn_data *pasn, const u8 *own_addr,
				 const u8 *peer_addr, const u8 *bssid, int akmp,
				 int cipher, u16 group, int freq,
				 const u8 *beacon_rsne, u8 beacon_rsne_len,
				 const u8 *beacon_rsnxe, u8 beacon_rsnxe_len,
				 const struct wpabuf *comeback, bool verify)
{
	struct wpabuf *frame;
	int ret;

	(void) beacon_rsne;
	(void) beacon_rsne_len;
	(void) beacon_rsnxe;
	(void) beacon_rsnxe_len;

	pasn->ecdh = crypto_ecdh_init(group);
	if (!pasn->ecdh) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to init ECDH");
		goto fail;
	}

	pasn->akmp = akmp;
	pasn->cipher = cipher;
	pasn->group = group;
	pasn->freq = freq;

	os_memcpy(pasn->own_addr, own_addr, ETH_ALEN);
	os_memcpy(pasn->peer_addr, peer_addr, ETH_ALEN);
	os_memcpy(pasn->bssid, bssid, ETH_ALEN);

	wpa_printf(MSG_DEBUG,
		   "PASN: Init%s: " MACSTR " akmp=0x%x, cipher=0x%x, group=%u",
		   verify ? " (verify)" : "",
		   MAC2STR(pasn->peer_addr), pasn->akmp, pasn->cipher,
		   pasn->group);

	frame = wpas_pasn_build_auth_1(pasn, comeback, verify);
	if (!frame) {
		wpa_printf(MSG_DEBUG, "PASN: Failed building 1st auth frame");
		goto fail;
	}

	wpabuf_free(pasn->frame);
	pasn->frame = NULL;

	ret = pasn->send_mgmt(pasn->cb_ctx,
			      wpabuf_head(frame), wpabuf_len(frame), 0,
			      pasn->freq, 1000);

	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed sending 1st auth frame");
		wpabuf_free(frame);
		goto fail;
	}

	pasn->frame = frame;
	return 0;

fail:
	return -1;
}


int wpas_pasn_start(struct pasn_data *pasn, const u8 *own_addr,
		    const u8 *peer_addr, const u8 *bssid,
		    int akmp, int cipher, u16 group,
		    int freq, const u8 *beacon_rsne, u8 beacon_rsne_len,
		    const u8 *beacon_rsnxe, u8 beacon_rsnxe_len,
		    const struct wpabuf *comeback)
{
	/* TODO: Currently support only ECC groups */
	if (!dragonfly_suitable_group(group, 1)) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Reject unsuitable group %u", group);
		return -1;
	}

	switch (akmp) {
	case WPA_KEY_MGMT_PASN:
		break;
#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
	case WPA_KEY_MGMT_SAE:
	case WPA_KEY_MGMT_SAE_EXT_KEY:
		if (beacon_rsnxe &&
		    !ieee802_11_rsnx_capab(beacon_rsnxe,
					   WLAN_RSNX_CAPAB_SAE_H2E)) {
			wpa_printf(MSG_DEBUG,
				   "PASN: AP does not support SAE H2E");
			return -1;
		}
		pasn->sae.state = SAE_NOTHING;
		pasn->sae.send_confirm = 0;
		break;
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */
#ifndef ESP_SUPPLICANT
#ifdef CONFIG_FILS
	case WPA_KEY_MGMT_FILS_SHA256:
	case WPA_KEY_MGMT_FILS_SHA384:
		break;
#endif /* CONFIG_FILS */
#endif /* ESP_SUPPLICANT */
#ifdef CONFIG_IEEE80211R
	case WPA_KEY_MGMT_FT_PSK:
	case WPA_KEY_MGMT_FT_IEEE8021X:
	case WPA_KEY_MGMT_FT_IEEE8021X_SHA384:
		break;
#endif /* CONFIG_IEEE80211R */
	default:
		wpa_printf(MSG_ERROR, "PASN: Unsupported AKMP=0x%x", akmp);
		return -1;
	}

	return wpas_pasn_send_auth_1(pasn, own_addr, peer_addr, bssid, akmp,
				     cipher, group,
				     freq, beacon_rsne, beacon_rsne_len,
				     beacon_rsnxe, beacon_rsnxe_len, comeback,
				     false);
}

/*
 * Wi-Fi Aware uses PASN handshake to authenticate peer devices.
 * Devices can simply verify each other for subsequent sessions using
 * pairing verification procedure.
 *
 * In pairing verification, Wi-Fi aware devices use PASN authentication
 * frames with a custom PMKID and Wi-Fi Aware R4 specific verification IEs.
 * It does not use wrapped data in the Authentication frames. This function
 * provides support to construct PASN Authentication frames for pairing
 * verification.
 */
int wpa_pasn_verify(struct pasn_data *pasn, const u8 *own_addr,
		    const u8 *peer_addr, const u8 *bssid,
		    int akmp, int cipher, u16 group,
		    int freq, const u8 *beacon_rsne, u8 beacon_rsne_len,
		    const u8 *beacon_rsnxe, u8 beacon_rsnxe_len,
		    const struct wpabuf *comeback)
{
	return wpas_pasn_send_auth_1(pasn, own_addr, peer_addr, bssid, akmp,
				     cipher, group, freq, beacon_rsne,
				     beacon_rsne_len, beacon_rsnxe,
				     beacon_rsnxe_len, comeback, true);
}


static bool is_pasn_auth_frame(struct pasn_data *pasn,
			       const struct ieee80211_auth *mgmt,
			       size_t len, bool rx)
{
	u16 fc;
	size_t min_len = offsetof(struct ieee80211_auth, auth.variable);

	if (!mgmt || len < min_len) {
		wpa_printf(MSG_DEBUG,
			   "PASN: is_pasn_auth_frame: reject len=%zu (min %zu) mgmt=%p",
			   len, min_len, mgmt);
		return false;
	}

	/* Not an Authentication frame; do nothing */
	fc = le_to_host16(mgmt->frame_control);
	if (WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_MGMT ||
	    WLAN_FC_GET_STYPE(fc) != WLAN_FC_STYPE_AUTH) {
		wpa_printf(MSG_DEBUG,
			   "PASN: is_pasn_auth_frame: reject fc=0x%04x type=%u stype=%u (want MGMT AUTH)",
			   fc, WLAN_FC_GET_TYPE(fc), WLAN_FC_GET_STYPE(fc));
		return false;
	}

	/* Not our frame; do nothing */
	/* if (!ether_addr_equal(mgmt->bssid, pasn->bssid)) { */
	/* 	wpa_printf(MSG_DEBUG, */
	/* 		   "PASN: is_pasn_auth_frame: reject BSSID mismatch frame=" MACSTR */
	/* 		   " pasn->bssid=" MACSTR, */
	/* 		   MAC2STR(mgmt->bssid), MAC2STR(pasn->bssid)); */
	/* 	return false; */
	/* } */

	if (rx && (!ether_addr_equal(mgmt->da, pasn->own_addr) ||
		   !ether_addr_equal(mgmt->sa, pasn->peer_addr))) {
		wpa_printf(MSG_DEBUG,
			   "PASN: is_pasn_auth_frame: reject RX addr DA=" MACSTR
			   " SA=" MACSTR " (expect DA=" MACSTR " SA=" MACSTR ")",
			   MAC2STR(mgmt->da), MAC2STR(mgmt->sa),
			   MAC2STR(pasn->own_addr), MAC2STR(pasn->peer_addr));
		return false;
	}

	if (!rx && (!ether_addr_equal(mgmt->sa, pasn->own_addr) ||
		    !ether_addr_equal(mgmt->da, pasn->peer_addr))) {
		wpa_printf(MSG_DEBUG,
			   "PASN: is_pasn_auth_frame: reject TX-status addr SA=" MACSTR
			   " DA=" MACSTR " (expect SA=" MACSTR " DA=" MACSTR ")",
			   MAC2STR(mgmt->sa), MAC2STR(mgmt->da),
			   MAC2STR(pasn->own_addr), MAC2STR(pasn->peer_addr));
		return false;
	}

	/* Not PASN; do nothing */
	if (mgmt->auth.auth_alg != host_to_le16(WLAN_AUTH_PASN)) {
		wpa_printf(MSG_DEBUG,
			   "PASN: is_pasn_auth_frame: reject auth_alg=%u (want PASN %u)",
			   le_to_host16(mgmt->auth.auth_alg), WLAN_AUTH_PASN);
		return false;
	}

	wpa_printf(MSG_DEBUG,
		   "PASN: is_pasn_auth_frame: OK rx=%d trans=%u status=%u",
		   rx, le_to_host16(mgmt->auth.auth_transaction),
		   le_to_host16(mgmt->auth.status_code));
	return true;
}


int wpa_pasn_auth_rx(struct pasn_data *pasn, const u8 *data, size_t len,
		     struct wpa_pasn_params_data *pasn_params)

{
	struct ieee802_11_elems elems;
	struct wpa_ie_data rsn_data;
	const struct ieee80211_auth *mgmt =
		(const struct ieee80211_auth *) data;
	struct wpabuf *wrapped_data = NULL, *secret = NULL, *frame = NULL;
	u8 mic[WPA_PASN_MAX_MIC_LEN], out_mic[WPA_PASN_MAX_MIC_LEN];
	u8 mic_len;
	u16 status;
	int ret, inc_y;
	u8 *copy = NULL;
	size_t mic_offset, copy_len;

	if (!pasn) {
		wpa_printf(MSG_DEBUG,
			   "PASN: RX: wpa_pasn_auth_rx pasn is NULL len=%zu pasn_params=%p",
			   len, pasn_params);
		return -2;
	}

	wpa_printf(MSG_DEBUG,
		   "PASN: RX: wpa_pasn_auth_rx enter len=%zu pasn_params=%p trans_seq=%u "
		   "own=" MACSTR " peer=" MACSTR " bssid=" MACSTR,
		   len, pasn_params, pasn->trans_seq,
		   MAC2STR(pasn->own_addr), MAC2STR(pasn->peer_addr),
		   MAC2STR(pasn->bssid));

	if (!is_pasn_auth_frame(pasn, mgmt, len, true)) {
		wpa_printf(MSG_DEBUG,
			   "PASN: RX: wpa_pasn_auth_rx not a PASN auth frame (is_pasn_auth_frame)");
		return -2;
	}

	wpa_printf(MSG_DEBUG,
		   "PASN: RX: wpa_pasn_auth_rx frame accepted, expect trans=%u",
		   pasn->trans_seq + 1);

	if (mgmt->auth.auth_transaction !=
	    host_to_le16(pasn->trans_seq + 1)) {
		wpa_printf(MSG_DEBUG,
			   "PASN: RX: wpa_pasn_auth_rx invalid trans got=%u expect=%u -> -3",
			   le_to_host16(mgmt->auth.auth_transaction),
			   pasn->trans_seq + 1);
		return -3;
	}

	status = le_to_host16(mgmt->auth.status_code);

	if (status != WLAN_STATUS_SUCCESS &&
	    status != WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Authentication rejected - status=%u", status);
		goto fail;
	}

	if (ieee802_11_parse_elems(mgmt->auth.variable,
				   len - offsetof(struct ieee80211_auth,
						  auth.variable),
				   &elems, 0) == ParseFailed) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Failed parsing Authentication frame");
		goto fail;
	}

	if (!elems.pasn_params || !elems.pasn_params_len) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Missing PASN Parameters IE");
		goto fail;
	}

	if (!pasn_params) {
		wpa_printf(MSG_DEBUG, "PASN: pasn_params == NULL");
		goto fail;
	}

	ret = wpa_pasn_parse_parameter_ie(elems.pasn_params - 3,
					  elems.pasn_params_len + 3,
					  true, pasn_params);
	if (ret) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Failed validation PASN of Parameters IE");
		goto fail;
	}

	if (status == WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY) {
		wpa_printf(MSG_DEBUG,
			   "PASN: Authentication temporarily rejected");

		if (pasn_params->comeback && pasn_params->comeback_len) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Comeback token available. After=%u",
				   pasn_params->after);

			if (!pasn_params->after) {
				wpa_printf(MSG_DEBUG,
					   "PASN: RX: wpa_pasn_auth_rx comeback immediate -> 1");
				return 1;
			}

			pasn->comeback = wpabuf_alloc_copy(
				pasn_params->comeback,
				pasn_params->comeback_len);
			if (pasn->comeback)
				pasn->comeback_after = pasn_params->after;
		}

		pasn->status = status;
		goto fail;
	}

	if (!elems.rsn_ie) {
		wpa_printf(MSG_DEBUG, "PASN: Missing RSNE");
		goto fail;
	}

	ret = wpa_parse_wpa_ie(elems.rsn_ie - 2, elems.rsn_ie_len + 2,
			       &rsn_data);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed parsing RSNE");
		goto fail;
	}

	ret = wpa_pasn_validate_rsne(&rsn_data);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed validating RSNE");
		goto fail;
	}

	if (pasn->akmp != rsn_data.key_mgmt ||
	    pasn->cipher != rsn_data.pairwise_cipher) {
		wpa_printf(MSG_DEBUG, "PASN: Mismatch in AKMP/cipher");
		goto fail;
	}

	if (pasn->group != pasn_params->group) {
		wpa_printf(MSG_DEBUG, "PASN: Mismatch in group");
		goto fail;
	}

	if (!pasn_params->pubkey || !pasn_params->pubkey_len) {
		wpa_printf(MSG_DEBUG, "PASN: Invalid public key");
		goto fail;
	}

	if (pasn_params->pubkey[0] == WPA_PASN_PUBKEY_UNCOMPRESSED) {
		inc_y = 1;
	} else if (pasn_params->pubkey[0] == WPA_PASN_PUBKEY_COMPRESSED_0 ||
		   pasn_params->pubkey[0] == WPA_PASN_PUBKEY_COMPRESSED_1) {
		inc_y = 0;
	} else {
		wpa_printf(MSG_DEBUG,
			   "PASN: Invalid first octet in pubkey=0x%x",
			   pasn_params->pubkey[0]);
		goto fail;
	}

	secret = crypto_ecdh_set_peerkey(pasn->ecdh, inc_y,
					 pasn_params->pubkey + 1,
					 pasn_params->pubkey_len - 1);

	if (!secret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to derive shared secret");
		goto fail;
	}

	if (pasn_params->wrapped_data_format != WPA_PASN_WRAPPED_DATA_NO) {
		wrapped_data = ieee802_11_defrag(elems.wrapped_data,
						 elems.wrapped_data_len,
						 true);

		if (!wrapped_data) {
			wpa_printf(MSG_DEBUG, "PASN: Missing wrapped data");
			goto fail;
		}
	}

	ret = wpas_pasn_set_pmk(pasn, &rsn_data, pasn_params, wrapped_data);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to set PMK");
		goto fail;
	}

	pasn_nd_pmk_global_clear();

	ret = pasn_pmk_to_ptk(pasn->pmk, pasn->pmk_len,
			      pasn->own_addr, pasn->peer_addr,
			      wpabuf_head(secret), wpabuf_len(secret),
			      &pasn->ptk, pasn->akmp, pasn->cipher,
			      pasn->kdk_len, pasn->kek_len, &pasn->hash_alg,
			      false);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed to derive PTK");
		goto fail;
	}

	if (pasn->ptk.kdk_len) {
		ret = pasn_nd_pmk_derive_from_kdk_store(
			pasn->ptk.kdk, pasn->ptk.kdk_len, pasn->cipher,
			pasn->own_addr, pasn->peer_addr);
		if (ret) {
			wpa_printf(MSG_DEBUG, "PASN: Failed to derive ND-PMK");
			goto fail;
		}
	}

	if (pasn->secure_ltf) {
		ret = wpa_ltf_keyseed(&pasn->ptk, pasn->akmp, pasn->cipher);
		if (ret) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Failed to derive LTF keyseed");
			goto fail;
		}
	}

	wpabuf_free(wrapped_data);
	wrapped_data = NULL;
	wpabuf_free(secret);
	secret = NULL;

	/* Check that the MIC IE exists. Save it and zero out the memory */
	mic_len = pasn_mic_len(pasn->hash_alg);
	if (status == WLAN_STATUS_SUCCESS) {
		if (!elems.mic || elems.mic_len != mic_len) {
			wpa_printf(MSG_DEBUG,
				   "PASN: Invalid MIC. Expecting len=%u %p %d",
				   mic_len,elems.mic,elems.mic_len);
			goto fail;
		}
		os_memcpy(mic, elems.mic, mic_len);
	}

	/* Use a copy of the message since we need to clear the MIC field */
	if (!elems.mic)
		goto fail;
	mic_offset = elems.mic - (const u8 *) &mgmt->auth;
	copy_len = len - offsetof(struct ieee80211_auth, auth);
	if (mic_offset + mic_len > copy_len)
		goto fail;
	copy = os_memdup(&mgmt->auth, copy_len);
	if (!copy)
		goto fail;
	os_memset(copy + mic_offset, 0, mic_len);

	{
		u8 *rsne_rsnxe;
		size_t rsne_rsnxe_len = 0;
		size_t off = 0;

		/* MIC uses RSNE and RSNXE as present in this Authentication frame */
		if (!elems.rsn_ie || !elems.rsn_ie_len)
			goto fail;
		rsne_rsnxe_len = elems.rsn_ie_len + 2;
		if (elems.rsnxe && elems.rsnxe_len)
			rsne_rsnxe_len += elems.rsnxe_len + 2;

		rsne_rsnxe = os_malloc(rsne_rsnxe_len);
		if (!rsne_rsnxe)
			goto fail;

		os_memcpy(rsne_rsnxe, elems.rsn_ie - 2, elems.rsn_ie_len + 2);
		off = elems.rsn_ie_len + 2;
		if (elems.rsnxe && elems.rsnxe_len)
			os_memcpy(rsne_rsnxe + off, elems.rsnxe - 2,
				  elems.rsnxe_len + 2);

		wpa_hexdump_key(MSG_DEBUG, "PASN: RSN + RSNXE buf",
				rsne_rsnxe, rsne_rsnxe_len);

		ret = pasn_mic(pasn->hash_alg, pasn->ptk.kck, pasn->ptk.kck_len,
			       pasn->peer_addr, pasn->own_addr,
			       rsne_rsnxe, rsne_rsnxe_len,
			       copy, copy_len, out_mic);

		os_free(rsne_rsnxe);
	}
	os_free(copy);
	copy = NULL;

	wpa_hexdump_key(MSG_DEBUG, "PASN: Frame MIC", mic, mic_len);
	if (ret || os_memcmp(mic, out_mic, mic_len) != 0) {
		wpa_printf(MSG_DEBUG, "PASN: Failed MIC verification");
		goto fail;
	}

	pasn->trans_seq++;

	wpa_printf(MSG_DEBUG, "PASN: Success verifying Authentication frame");

	if (pasn_parse_encrypted_data(pasn, data, len) < 0) {
		wpa_printf(MSG_DEBUG, "PASN: Encrypted data processing failed");
		goto fail;
	}

	frame = wpas_pasn_build_auth_3(pasn);
	if (!frame) {
		wpa_printf(MSG_DEBUG, "PASN: Failed building 3rd auth frame");
		goto fail;
	}

	wpabuf_free(pasn->frame);
	pasn->frame = NULL;

	ret = pasn->send_mgmt(pasn->cb_ctx,
			      wpabuf_head(frame), wpabuf_len(frame), 0,
			      pasn->freq, 100);
	if (ret) {
		wpa_printf(MSG_DEBUG, "PASN: Failed sending 3st auth frame");
		wpabuf_free(frame);
		goto fail;
	}

	pasn->frame = frame;
	wpa_printf(MSG_DEBUG, "PASN: Success sending last frame. Store PTK");

	pasn->status = WLAN_STATUS_SUCCESS;

	wpa_printf(MSG_DEBUG,
		   "PASN: RX: wpa_pasn_auth_rx complete trans_seq=%u -> 0",
		   pasn->trans_seq);
	return 0;
fail:
	wpa_printf(MSG_DEBUG,
		   "PASN: RX: wpa_pasn_auth_rx fail status=%u -> -1",
		   status);
	wpa_printf(MSG_DEBUG, "PASN: Failed RX processing - terminating");
	wpabuf_free(wrapped_data);
	wpabuf_free(secret);
	os_free(copy);

	/*
	 * TODO: In case of an error the standard allows to silently drop
	 * the frame and terminate the authentication exchange. However, better
	 * reply to the AP with an error status.
	 */
	if (status == WLAN_STATUS_SUCCESS)
		pasn->status = WLAN_STATUS_UNSPECIFIED_FAILURE;
	else
		pasn->status = status;

	return -1;
}


int wpa_pasn_auth_tx_status(struct pasn_data *pasn,
			    const u8 *data, size_t data_len, u8 acked)

{
	const struct ieee80211_auth *mgmt =
		(const struct ieee80211_auth *) data;

	wpa_printf(MSG_DEBUG, "PASN: auth_tx_status: acked=%u", acked);

	if (!is_pasn_auth_frame(pasn, mgmt, data_len, false))
		return -1;

	if (mgmt->auth.auth_transaction != host_to_le16(pasn->trans_seq)) {
		wpa_printf(MSG_ERROR,
			   "PASN: Invalid transaction sequence: (%u != %u)",
			   pasn->trans_seq,
			   le_to_host16(mgmt->auth.auth_transaction));
		return 0;
	}

	wpa_printf(MSG_ERROR,
		   "PASN: auth with trans_seq=%u, acked=%u", pasn->trans_seq,
		   acked);

	/*
	 * Even if the frame was not acked, do not treat this is an error, and
	 * try to complete the flow, relying on the PASN timeout callback to
	 * clean up.
	 */
	if (pasn->trans_seq == WLAN_AUTH_TR_SEQ_PASN_AUTH3) {
		wpa_printf(MSG_DEBUG, "PASN: auth complete with: " MACSTR,
			   MAC2STR(pasn->peer_addr));
		/*
		 * Either frame was not ACKed or it was ACKed but the trans_seq
		 * != 1, i.e., not expecting an RX frame, so we are done.
		 */
		return 1;
	}

	return 0;
}
