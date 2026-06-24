/*
 * DPP module internal definitions
 * Copyright (c) 2017, Qualcomm Atheros, Inc.
 * Copyright (c) 2018-2020, The Linux Foundation
 * Copyright (c) 2021-2022, Qualcomm Innovation Center, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef ESP_DPP_I_H
#define ESP_DPP_I_H

#include "esp_err.h"
#include "utils/includes.h"
#include "utils/common.h"
#include "common/dpp.h"
#include "esp_dpp.h"
#include "esp_wifi_driver.h"

/* dpp_crypto.c */

struct dpp_signed_connector_info {
	unsigned char *payload;
	size_t payload_len;
};

const struct dpp_curve_params *dpp_get_curve_name(const char *name);
const struct dpp_curve_params *dpp_get_curve_jwk_crv(const char *name);
void dpp_debug_print_key(const char *title, struct crypto_ec_key *key);
int dpp_hash_vector(const struct dpp_curve_params *curve,
	size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac);
int dpp_hkdf_expand(size_t hash_len, const u8 *secret, size_t secret_len,
			   const char *label, u8 *out, size_t outlen);
int dpp_hmac_vector(size_t hash_len, const u8 *key, size_t key_len,
			   size_t num_elem, const u8 *addr[],
			   const size_t *len, u8 *mac);
int dpp_hmac(size_t hash_len, const u8 *key, size_t key_len,
		    const u8 *data, size_t data_len, u8 *mac);
struct crypto_ec_key * dpp_set_pubkey_point(struct crypto_ec_key *group_key,
				       const u8 *buf, size_t len);
struct crypto_ec_key * dpp_gen_keypair(const struct dpp_curve_params *curve);
struct crypto_ec_key * dpp_set_keypair(const struct dpp_curve_params **curve,
					   const u8 *privkey, size_t privkey_len);
int dpp_bootstrap_key_hash(struct dpp_bootstrap_info *bi);
int dpp_keygen(struct dpp_bootstrap_info *bi, const char *curve,
		  u8 *privkey, size_t privkey_len);
int dpp_derive_k1(const u8 *Mx, size_t Mx_len, u8 *k1,
			 unsigned int hash_len);
int dpp_derive_k2(const u8 *Nx, size_t Nx_len, u8 *k2,
			 unsigned int hash_len);
int dpp_ecdh(struct crypto_ec_key *own, struct crypto_ec_key *peer,
		    u8 *secret, size_t *secret_len);
struct wpabuf *dpp_parse_jws_prot_hdr(const struct dpp_curve_params *curve,
		       const u8 *prot_hdr, u16 prot_hdr_len, int *hash_func);
int dpp_check_pubkey_match(struct crypto_ec_key *pub, struct wpabuf *r_hash);
enum dpp_status_error dpp_process_signed_connector(struct dpp_signed_connector_info *info,
			struct crypto_ec_key *csign_pub, const char *connector);
int dpp_gen_r_auth(struct dpp_authentication *auth, u8 *r_auth);
int dpp_gen_i_auth(struct dpp_authentication *auth, u8 *i_auth);
int dpp_auth_derive_l_responder(struct dpp_authentication *auth);
int dpp_auth_derive_l_initiator(struct dpp_authentication *auth);
int dpp_derive_pmk(const u8 *Nx, size_t Nx_len, u8 *pmk,
			  unsigned int hash_len);
int dpp_derive_pmkid(const struct dpp_curve_params *curve,
			    struct crypto_ec_key *own_key, struct crypto_ec_key *peer_key, u8 *pmkid);
int dpp_bn2bin_pad(const struct crypto_bignum *bn, u8 *pos, size_t len);
struct wpabuf * dpp_bootstrap_key_der(struct crypto_ec_key *key);
struct wpabuf * dpp_get_pubkey_point(struct crypto_ec_key *pkey, int prefix);
int dpp_get_subject_public_key(struct dpp_bootstrap_info *bi, const u8 *data, size_t data_len);
int dpp_derive_bk_ke(struct dpp_authentication *auth);
enum dpp_status_error
dpp_check_signed_connector(struct dpp_signed_connector_info *info,
                           const u8 *csign_key, size_t csign_key_len,
                           const u8 *peer_connector, size_t peer_connector_len);

/* dpp crypto apis */

#endif /* ESP_DPP_I_H */
