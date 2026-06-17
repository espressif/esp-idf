/*
 * PASN info for initiator and responder
 *
 * Copyright (C) 2019, Intel Corporation
 * Copyright (c) 2022, Jouni Malinen <j@w1.fi>
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef PASN_COMMON_H
#define PASN_COMMON_H

#include "common/wpa_common.h"
#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
#include "common/sae.h"
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP_SUPPLICANT
#ifdef CONFIG_FILS
enum pasn_fils_state {
	PASN_FILS_STATE_NONE = 0,
	PASN_FILS_STATE_PENDING_AS,
	PASN_FILS_STATE_COMPLETE
};

struct pasn_fils {
	u8 state;
	u8 nonce[FILS_NONCE_LEN];
	u8 anonce[FILS_NONCE_LEN];
	u8 session[FILS_SESSION_LEN];
	u8 erp_pmkid[PMKID_LEN];
	bool completed;
	struct wpabuf *erp_resp;
};
#endif /* CONFIG_FILS */
#endif /* ESP_SUPPLICANT */

struct pasn_data {
	/* External modules access below variables using setter and getter
	 * functions */
	int akmp;
	int cipher;
	u8 own_addr[ETH_ALEN];
	u8 peer_addr[ETH_ALEN];
	u8 bssid[ETH_ALEN];
	struct rsn_pmksa_cache *pmksa;
	bool derive_kdk;
	size_t kdk_len;
	void *cb_ctx;

	struct sae_pt *pt;

	/* Responder */
	const char *password;
	int wpa_key_mgmt;
	int rsn_pairwise;
	u16 rsnxe_capab;
	u8 *rsnxe_ie;
	bool custom_pmkid_valid;
	u8 custom_pmkid[PMKID_LEN];

	/*
	 * Extra elements to add into Authentication frames. These can be used,
	 * e.g., for Wi-Fi Aware use cases.
	 */
	const u8 *extra_ies;
	size_t extra_ies_len;

	/* External modules do not access below variables */
	bool derive_kek;
	size_t kek_len;
	u16 group;
	bool secure_ltf;
	int freq;

	u8 trans_seq;
	u8 status;

	size_t pmk_len;
	u8 pmk[PMK_LEN_MAX];
	bool using_pmksa;
	enum rsn_hash_alg hash_alg;

	struct wpabuf *auth1;

	struct wpa_ptk ptk;
	struct crypto_ecdh *ecdh;

	struct wpabuf *comeback;
	u16 comeback_after;

#if defined(CONFIG_SAE) || defined(CONFIG_WPA3_SAE)
	struct sae_data sae;
#endif /* CONFIG_SAE || CONFIG_WPA3_SAE */

#ifndef ESP_SUPPLICANT
#ifdef CONFIG_FILS
	bool fils_eapol;
	bool fils_wd_valid;
	struct pasn_fils fils;
#endif /* CONFIG_FILS */
#endif /* ESP_SUPPLICANT */

#ifdef CONFIG_IEEE80211R
	u8 pmk_r1[PMK_LEN_MAX];
	size_t pmk_r1_len;
	u8 pmk_r1_name[WPA_PMK_NAME_LEN];
#endif /* CONFIG_IEEE80211R */
	/* Note that these pointers to RSN PMKSA cache are defined differently
	 * for the PASN initiator (RSN supplicant) and PASN responder (RSN
	 * authenticator). Functions cannot be mixed between those cases. */
	struct rsn_pmksa_cache_entry *pmksa_entry;
	struct eapol_sm *eapol;
	int fast_reauth;
#ifdef CONFIG_TESTING_OPTIONS
	int corrupt_mic;
#endif /* CONFIG_TESTING_OPTIONS */
	int network_id;

	u8 wrapped_data_format;
	struct wpabuf *secret;

	/* Responder */
	bool noauth; /* Whether PASN without mutual authentication is enabled */
	int disable_pmksa_caching;
	int *pasn_groups;
	int use_anti_clogging;
	const u8 *rsn_ie;
	size_t rsn_ie_len;

	u8 *comeback_key;
	struct os_reltime last_comeback_key_update;
	u16 comeback_idx;
	u16 *comeback_pending_idx;
	struct wpabuf *frame;

	/**
	 * send_mgmt - Function handler to transmit a Management frame
	 * @ctx: Callback context from cb_ctx
	 * @frame_buf : Frame to transmit
	 * @frame_len: Length of frame to transmit
	 * @freq: Frequency in MHz for the channel on which to transmit
	 * @wait_dur: How many milliseconds to wait for a response frame
	 * Returns: 0 on success, -1 on failure
	 */
	int (*send_mgmt)(void *ctx, const u8 *data, size_t data_len, int noack,
			 unsigned int freq, unsigned int wait);
	/**
	 * validate_custom_pmkid - Handler to validate vendor specific PMKID
	 * @ctx: Callback context from cb_ctx
	 * @addr : MAC address of the peer
	 * @pmkid: Custom PMKID
	 * Returns: 0 on success (valid PMKID), -1 on failure
	 */
	int (*validate_custom_pmkid)(void *ctx, const u8 *addr,
				     const u8 *pmkid);

	int (*prepare_data_element)(void *ctx, const u8 *peer_addr);

	int (*parse_data_element)(void *ctx, const u8 *data, size_t len);
};

/* Initiator */
void wpa_pasn_reset(struct pasn_data *pasn);
int wpas_pasn_start(struct pasn_data *pasn, const u8 *own_addr,
		    const u8 *peer_addr, const u8 *bssid,
		    int akmp, int cipher, u16 group,
		    int freq, const u8 *beacon_rsne, u8 beacon_rsne_len,
		    const u8 *beacon_rsnxe, u8 beacon_rsnxe_len,
		    const struct wpabuf *comeback);
int wpa_pasn_verify(struct pasn_data *pasn, const u8 *own_addr,
		    const u8 *peer_addr, const u8 *bssid,
		    int akmp, int cipher, u16 group,
		    int freq, const u8 *beacon_rsne, u8 beacon_rsne_len,
		    const u8 *beacon_rsnxe, u8 beacon_rsnxe_len,
		    const struct wpabuf *comeback);
int wpa_pasn_auth_rx(struct pasn_data *pasn, const u8 *data, size_t len,
		     struct wpa_pasn_params_data *pasn_params);
int wpa_pasn_auth_tx_status(struct pasn_data *pasn,
			    const u8 *data, size_t data_len, u8 acked);

/* Responder */
int handle_auth_pasn_1(struct pasn_data *pasn,
		       const u8 *own_addr, const u8 *peer_addr,
		       const struct ieee80211_auth *mgmt, size_t len,
		       bool reject);
int handle_auth_pasn_3(struct pasn_data *pasn, const u8 *own_addr,
		       const u8 *peer_addr,
		       const struct ieee80211_auth *mgmt, size_t len);
int handle_auth_pasn_resp(struct pasn_data *pasn, const u8 *own_addr,
			  const u8 *peer_addr,
			  struct rsn_pmksa_cache_entry *pmksa, u16 status);

struct pasn_data * pasn_data_init(void);
void pasn_data_deinit(struct pasn_data *pasn);
void pasn_register_callbacks(struct pasn_data *pasn, void *cb_ctx,
			     int (*send_mgmt)(void *ctx, const u8 *data,
					      size_t data_len, int noack,
					      unsigned int freq,
					      unsigned int wait),
			     int (*validate_custom_pmkid)(void *ctx,
							  const u8 *addr,
							  const u8 *pmkid));
void pasn_enable_kdk_derivation(struct pasn_data *pasn);
void pasn_disable_kdk_derivation(struct pasn_data *pasn);

void pasn_set_akmp(struct pasn_data *pasn, int akmp);
void pasn_set_cipher(struct pasn_data *pasn, int cipher);
void pasn_set_own_addr(struct pasn_data *pasn, const u8 *addr);
void pasn_set_peer_addr(struct pasn_data *pasn, const u8 *addr);
void pasn_set_bssid(struct pasn_data *pasn, const u8 *addr);
void pasn_set_initiator_pmksa(struct pasn_data *pasn,
			      struct rsn_pmksa_cache *pmksa);
void pasn_set_responder_pmksa(struct pasn_data *pasn,
			      struct rsn_pmksa_cache *pmksa);
int pasn_set_pt(struct pasn_data *pasn, struct sae_pt *pt);
struct rsn_pmksa_cache * pasn_initiator_pmksa_cache_init(void);
void pasn_initiator_pmksa_cache_deinit(struct rsn_pmksa_cache *pmksa);
int pasn_initiator_pmksa_cache_add(struct rsn_pmksa_cache *pmksa,
				   const u8 *own_addr, const u8 *bssid,
				   const u8 *pmk, size_t pmk_len,
				   const u8 *pmkid);
int pasn_initiator_pmksa_cache_get(struct rsn_pmksa_cache *pmksa,
				   const u8 *bssid, u8 *pmkid, u8 *pmk,
				   size_t *pmk_len);
void pasn_initiator_pmksa_cache_remove(struct rsn_pmksa_cache *pmksa,
				       const u8 *bssid);
void pasn_initiator_pmksa_cache_flush(struct rsn_pmksa_cache *pmksa);

/* Responder */
void pasn_set_noauth(struct pasn_data *pasn, bool noauth);
void pasn_set_password(struct pasn_data *pasn, const char *password);
void pasn_set_wpa_key_mgmt(struct pasn_data *pasn, int key_mgmt);
void pasn_set_rsn_pairwise(struct pasn_data *pasn, int rsn_pairwise);
void pasn_set_rsnxe_caps(struct pasn_data *pasn, u16 rsnxe_capab);
void pasn_set_rsnxe_ie(struct pasn_data *pasn, const u8 *rsnxe_ie);
void pasn_set_custom_pmkid(struct pasn_data *pasn, const u8 *pmkid);
int pasn_set_extra_ies(struct pasn_data *pasn, const u8 *extra_ies,
		       size_t extra_ies_len);
struct rsn_pmksa_cache * pasn_responder_pmksa_cache_init(void);
void pasn_responder_pmksa_cache_deinit(struct rsn_pmksa_cache *pmksa);
int pasn_responder_pmksa_cache_add(struct rsn_pmksa_cache *pmksa,
				   const u8 *own_addr, const u8 *bssid,
				   const u8 *pmk, size_t pmk_len,
				   const u8 *pmkid);
int pasn_responder_pmksa_cache_get(struct rsn_pmksa_cache *pmksa,
				   const u8 *bssid, u8 *pmkid, u8 *pmk,
				   size_t *pmk_len);
void pasn_responder_pmksa_cache_remove(struct rsn_pmksa_cache *pmksa,
				       const u8 *bssid);
void pasn_responder_pmksa_cache_flush(struct rsn_pmksa_cache *pmksa);

int pasn_get_akmp(struct pasn_data *pasn);
int pasn_get_cipher(struct pasn_data *pasn);
size_t pasn_get_pmk_len(struct pasn_data *pasn);
u8 * pasn_get_pmk(struct pasn_data *pasn);
struct wpa_ptk * pasn_get_ptk(struct pasn_data *pasn);
int pasn_add_encrypted_data(struct pasn_data *pasn, struct wpabuf *buf,
			    const u8 *data, size_t data_len);
int pasn_parse_encrypted_data(struct pasn_data *pasn, const u8 *data,
			      size_t len);

#ifdef __cplusplus
}
#endif
#endif /* PASN_COMMON_H */
