/*
 * DPP crypto functionality shared between hostapd and wpa_supplicant
 * Copyright (c) 2017, Qualcomm Atheros, Inc.
 * Copyright (c) 2018-2024, The Linux Foundation
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include <fcntl.h>

#include "utils/common.h"
#include "common/defs.h"
#include "utils/base64.h"
#include "utils/json.h"
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "crypto/aes.h"
#include "crypto/aes_siv.h"
#include "crypto/sha256.h"
#include "dpp.h"
#include "dpp_i.h"

const struct dpp_curve_params dpp_curves[] = {
	/* The mandatory to support and the default NIST P-256 curve needs to
	 * be the first entry on this list. */
	{ "secp256r1", 32, 32, 16, 32, "P-256", 19, "ES256" },
#ifndef ESP_SUPPLICANT
	{ "secp384r1", 48, 48, 24, 48, "P-384", 20, "ES384" },
	{ "secp521r1", 64, 64, 32, 66, "P-521", 21, "ES512" },
	{ "brainpoolP256r1", 32, 32, 16, 32, "BP-256", 28, "BS256" },
	{ "brainpoolP384r1", 48, 48, 24, 48, "BP-384", 29, "BS384" },
	{ "brainpoolP512r1", 64, 64, 32, 64, "BP-512", 30, "BS512" },
#endif
	{ NULL, 0, 0, 0, 0, NULL, 0, NULL }
};


const struct dpp_curve_params * dpp_get_curve_name(const char *name)
{
	int i;

	if (!name)
		return &dpp_curves[0];

	for (i = 0; dpp_curves[i].name; i++) {
		if (os_strcmp(name, dpp_curves[i].name) == 0 ||
		    (dpp_curves[i].jwk_crv &&
		     os_strcmp(name, dpp_curves[i].jwk_crv) == 0))
			return &dpp_curves[i];
	}
	return NULL;
}


const struct dpp_curve_params * dpp_get_curve_jwk_crv(const char *name)
{
	int i;

	for (i = 0; dpp_curves[i].name; i++) {
		if (dpp_curves[i].jwk_crv &&
		    os_strcmp(name, dpp_curves[i].jwk_crv) == 0)
			return &dpp_curves[i];
	}
	return NULL;
}


const struct dpp_curve_params * dpp_get_curve_ike_group(u16 group)
{
	int i;

	for (i = 0; dpp_curves[i].name; i++) {
		if (dpp_curves[i].ike_group == group)
			return &dpp_curves[i];
	}
	return NULL;
}


void dpp_debug_print_key(const char *title, struct crypto_ec_key *key)
{
    crypto_ec_key_debug_print(key, title);
}


int dpp_hash_vector(const struct dpp_curve_params *curve,
			   size_t num_elem, const u8 *addr[], const size_t *len,
			   u8 *mac)
{
	if (curve->hash_len == 32)
		return sha256_vector(num_elem, addr, len, mac);
#ifndef ESP_SUPPLICANT
	if (curve->hash_len == 48)
		return sha384_vector(num_elem, addr, len, mac);
	if (curve->hash_len == 64)
		return sha512_vector(num_elem, addr, len, mac);
#endif
	return -1;
}


int dpp_hkdf_expand(size_t hash_len, const u8 *secret, size_t secret_len,
		    const char *label, u8 *out, size_t outlen)
{
	if (hash_len == 32)
		return hmac_sha256_kdf(secret, secret_len, NULL,
				       (const u8 *) label, os_strlen(label),
				       out, outlen);
#ifndef ESP_SUPPLICANT
	if (hash_len == 48)
		return hmac_sha384_kdf(secret, secret_len, NULL,
				       (const u8 *) label, os_strlen(label),
				       out, outlen);
	if (hash_len == 64)
		return hmac_sha512_kdf(secret, secret_len, NULL,
				       (const u8 *) label, os_strlen(label),
				       out, outlen);
#endif
	return -1;
}


int dpp_hmac_vector(size_t hash_len, const u8 *key, size_t key_len,
		    size_t num_elem, const u8 *addr[], const size_t *len,
		    u8 *mac)
{
	if (hash_len == 32)
		return hmac_sha256_vector(key, key_len, num_elem, addr, len,
					  mac);
#ifndef ESP_SUPPLICANT
	if (hash_len == 48)
		return hmac_sha384_vector(key, key_len, num_elem, addr, len,
					  mac);
	if (hash_len == 64)
		return hmac_sha512_vector(key, key_len, num_elem, addr, len,
					  mac);
#endif
	return -1;
}


int dpp_hmac(size_t hash_len, const u8 *key, size_t key_len,
		    const u8 *data, size_t data_len, u8 *mac)
{
	if (hash_len == 32)
		return hmac_sha256(key, key_len, data, data_len, mac);
#ifndef ESP_SUPPLICANT
	if (hash_len == 48)
		return hmac_sha384(key, key_len, data, data_len, mac);
	if (hash_len == 64)
		return hmac_sha512(key, key_len, data, data_len, mac);
#endif
	return -1;
}

struct crypto_ec_key * dpp_set_pubkey_point(struct crypto_ec_key *group_key,
					    const u8 *buf, size_t len)
{
	const struct crypto_ec_group *group;
	struct crypto_ec_key *pkey = NULL;

	if (len & 1)
		return NULL;

	group = crypto_ec_get_group_from_key(group_key);
	if (group)
		pkey = crypto_ec_key_set_pub(group, buf, len);
	else
		wpa_printf(MSG_ERROR, "DPP: Could not get EC group");

	return pkey;
}


struct crypto_ec_key * dpp_gen_keypair(const struct dpp_curve_params *curve)
{
	struct crypto_ec_key *key;

	wpa_printf(MSG_DEBUG, "DPP: Generating a keypair");

	key = crypto_ec_key_gen(curve->ike_group);
	if (key)
	    dpp_debug_print_key("Own generated key", key);

	return key;
}


struct crypto_ec_key * dpp_set_keypair(const struct dpp_curve_params **curve,
				       const u8 *privkey, size_t privkey_len)
{
	struct crypto_ec_key *pkey;
	int group;

	pkey = crypto_ec_key_parse_priv(privkey, privkey_len);
	if (!pkey) {
		wpa_printf(MSG_ERROR, "%s: failed to get pkey", __func__);
		return NULL;
	}

	group = crypto_ec_key_group(pkey);
	if (group < 0) {
		crypto_ec_key_deinit(pkey);
		return NULL;
	}

	*curve = dpp_get_curve_ike_group(group);
	if (!*curve) {
		wpa_printf(MSG_INFO,
			   "DPP: Unsupported curve (group=%d) in pre-assigned key",
			   group);
		crypto_ec_key_deinit(pkey);
		return NULL;
	}

	return pkey;
}

int dpp_bi_pubkey_hash(struct dpp_bootstrap_info *bi,
		       const u8 *data, size_t data_len)
{
	const u8 *addr[2];
	size_t len[2];

	addr[0] = data;
	len[0] = data_len;
	if (sha256_vector(1, addr, len, bi->pubkey_hash) < 0)
		return -1;
	wpa_hexdump(MSG_DEBUG, "DPP: Public key hash",
		    bi->pubkey_hash, SHA256_MAC_LEN);
#ifndef ESP_SUPPLICANT
	addr[0] = (const u8 *) "chirp";
	len[0] = 5;
	addr[1] = data;
	len[1] = data_len;
	if (sha256_vector(2, addr, len, bi->pubkey_hash_chirp) < 0)
		return -1;
	wpa_hexdump(MSG_DEBUG, "DPP: Public key hash (chirp)",
		    bi->pubkey_hash_chirp, SHA256_MAC_LEN);
#endif //ESP_SUPPLICANT
	return 0;
}


int dpp_bootstrap_key_hash(struct dpp_bootstrap_info *bi)
{
	struct wpabuf *der;
	int res;

	der = crypto_ec_key_get_subject_public_key(bi->pubkey);
	if (!der)
		return -1;
	wpa_hexdump_buf(MSG_DEBUG, "DPP: Compressed public key (DER)",
			der);
	res = dpp_bi_pubkey_hash(bi, wpabuf_head(der), wpabuf_len(der));
	if (res < 0)
		wpa_printf(MSG_DEBUG, "DPP: Failed to hash public key");
	wpabuf_free(der);
	return res;
}


int dpp_keygen(struct dpp_bootstrap_info *bi, const char *curve,
		  u8 *privkey, size_t privkey_len)
{
	char *base64 = NULL;
	char *pos, *end;
	size_t len;
	struct wpabuf *der = NULL;

	bi->curve = dpp_get_curve_name(curve);
	if (!bi->curve) {
		wpa_printf(MSG_INFO, "DPP: Unsupported curve: %s", curve);
		return -1;
	}

	if (privkey)
		bi->pubkey = dpp_set_keypair(&bi->curve, privkey, privkey_len);
	else
		bi->pubkey = dpp_gen_keypair(bi->curve);
	if (!bi->pubkey)
		goto fail;
	bi->own = 1;

	der = crypto_ec_key_get_subject_public_key(bi->pubkey);
	if (!der)
		goto fail;
	wpa_hexdump_buf(MSG_DEBUG, "DPP: Compressed public key (DER)",
			der);

	if (dpp_bi_pubkey_hash(bi, wpabuf_head(der), wpabuf_len(der)) < 0) {
		wpa_printf(MSG_DEBUG, "DPP: Failed to hash public key");
		goto fail;
	}

	base64 = (char *)base64_encode(wpabuf_head(der), wpabuf_len(der), &len);
	wpabuf_free(der);
	der = NULL;
	if (!base64)
		goto fail;
	pos = base64;
	end = pos + len;
	for (;;) {
		pos = os_strchr(pos, '\n');
		if (!pos)
			break;
		os_memmove(pos, pos + 1, end - pos);
	}
	os_free(bi->pk);
	bi->pk = base64;
	return 0;
fail:
	os_free(base64);
	wpabuf_free(der);
	return -1;
}


int dpp_derive_k1(const u8 *Mx, size_t Mx_len, u8 *k1, unsigned int hash_len)
{
	u8 salt[DPP_MAX_HASH_LEN], prk[DPP_MAX_HASH_LEN];
	const char *info = "first intermediate key";
	int res;

	/* k1 = HKDF(<>, "first intermediate key", M.x) */

	/* HKDF-Extract(<>, M.x) */
	os_memset(salt, 0, hash_len);
	if (dpp_hmac(hash_len, salt, hash_len, Mx, Mx_len, prk) < 0)
		return -1;
	wpa_hexdump_key(MSG_DEBUG, "DPP: PRK = HKDF-Extract(<>, IKM=M.x)",
			prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = dpp_hkdf_expand(hash_len, prk, hash_len, info, k1, hash_len);
	forced_memzero(prk, hash_len);
	if (res < 0)
		return -1;

	wpa_hexdump_key(MSG_DEBUG, "DPP: k1 = HKDF-Expand(PRK, info, L)",
			k1, hash_len);
	return 0;
}


int dpp_derive_k2(const u8 *Nx, size_t Nx_len, u8 *k2, unsigned int hash_len)
{
	u8 salt[DPP_MAX_HASH_LEN], prk[DPP_MAX_HASH_LEN];
	const char *info = "second intermediate key";
	int res;

	/* k2 = HKDF(<>, "second intermediate key", N.x) */

	/* HKDF-Extract(<>, N.x) */
	os_memset(salt, 0, hash_len);
	res = dpp_hmac(hash_len, salt, hash_len, Nx, Nx_len, prk);
	if (res < 0)
		return -1;
	wpa_hexdump_key(MSG_DEBUG, "DPP: PRK = HKDF-Extract(<>, IKM=N.x)",
			prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = dpp_hkdf_expand(hash_len, prk, hash_len, info, k2, hash_len);
	forced_memzero(prk, hash_len);
	if (res < 0)
		return -1;

	wpa_hexdump_key(MSG_DEBUG, "DPP: k2 = HKDF-Expand(PRK, info, L)",
			k2, hash_len);
	return 0;
}

int dpp_ecdh(struct crypto_ec_key *own, struct crypto_ec_key *peer,
	     u8 *secret, size_t *secret_len)
{
    return crypto_ecdh(own, peer, secret, secret_len);
}


struct wpabuf *
dpp_parse_jws_prot_hdr(const struct dpp_curve_params *curve,
		       const u8 *prot_hdr, u16 prot_hdr_len,
		       int *hash_func)
{
	struct json_token *root, *token;
	struct wpabuf *kid = NULL;

	root = json_parse((const char *) prot_hdr, prot_hdr_len);
	if (!root) {
		wpa_printf(MSG_DEBUG,
			   "DPP: JSON parsing failed for JWS Protected Header");
		goto fail;
	}

	if (root->type != JSON_OBJECT) {
		wpa_printf(MSG_DEBUG,
			   "DPP: JWS Protected Header root is not an object");
		goto fail;
	}

	token = json_get_member(root, "typ");
	if (!token || token->type != JSON_STRING) {
		wpa_printf(MSG_DEBUG, "DPP: No typ string value found");
		goto fail;
	}
	wpa_printf(MSG_DEBUG, "DPP: JWS Protected Header typ=%s",
		   token->string);
	if (os_strcmp(token->string, "dppCon") != 0) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Unsupported JWS Protected Header typ=%s",
			   token->string);
		goto fail;
	}

	token = json_get_member(root, "alg");
	if (!token || token->type != JSON_STRING) {
		wpa_printf(MSG_DEBUG, "DPP: No alg string value found");
		goto fail;
	}
	wpa_printf(MSG_DEBUG, "DPP: JWS Protected Header alg=%s",
		   token->string);
	if (os_strcmp(token->string, curve->jws_alg) != 0) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Unexpected JWS Protected Header alg=%s (expected %s based on C-sign-key)",
			   token->string, curve->jws_alg);
		goto fail;
	}
        if (os_strcmp(token->string, "ES256") == 0 ||
            os_strcmp(token->string, "BS256") == 0) {
            *hash_func = CRYPTO_HASH_ALG_SHA256;
#ifndef ESP_SUPPLICANT
	} else if (os_strcmp(token->string, "ES384") == 0 ||
		   os_strcmp(token->string, "BS384") == 0) {
		*hash_func = CRYPTO_HASH_ALG_SHA384;
	} else if (os_strcmp(token->string, "ES512") == 0 ||
		   os_strcmp(token->string, "BS512") == 0) {
		*hash_func = CRYPTO_HASH_ALG_SHA512;
#endif // ESP_SUPPLICANT
        } else {
            *hash_func = -1;
	    wpa_printf(MSG_ERROR, "DPP : Unsupported JWS Protected Header alg=%s",
			   token->string);
	    goto fail;
        }

	kid = json_get_member_base64url(root, "kid");
	if (!kid) {
		wpa_printf(MSG_DEBUG, "DPP: No kid string value found");
		goto fail;
	}
	wpa_hexdump_buf(MSG_DEBUG, "DPP: JWS Protected Header kid (decoded)",
			kid);

fail:
	json_free(root);
	return kid;
}


int dpp_get_subject_public_key(struct dpp_bootstrap_info *bi,
                               const u8 *data, size_t data_len)
{
	struct crypto_ec_key *pkey;
	const unsigned char *p;

	if (dpp_bi_pubkey_hash(bi, data, data_len) < 0) {
		wpa_printf(MSG_DEBUG, "DPP: Failed to hash public key");
		return -1;
	}

	p = data;
	pkey = crypto_ec_parse_subpub_key((unsigned char *)p, data_len);
	if (!pkey) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Could not parse URI public-key SubjectPublicKeyInfo");
		return -1;
	}

	if (!crypto_is_ec_key(pkey)) {
		wpa_printf(MSG_DEBUG,
				"DPP: SubjectPublicKeyInfo does not describe an EC key");
		crypto_ec_key_deinit(pkey);
		return -1;
	}

	bi->curve = dpp_get_curve_ike_group(crypto_ec_key_group(pkey));
	if (!bi->curve) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Unsupported SubjectPublicKeyInfo curve: group %d",
			   crypto_ec_key_group(pkey));
		goto fail;
	}

	bi->pubkey = pkey;
	return 0;
fail:
	crypto_ec_key_deinit(pkey);
	return -1;

}

int dpp_derive_bk_ke(struct dpp_authentication *auth)
{
	unsigned int hash_len = auth->curve->hash_len;
	size_t nonce_len = auth->curve->nonce_len;
	u8 nonces[2 * DPP_MAX_NONCE_LEN];
	const char *info_ke = "DPP Key";
	u8 prk[DPP_MAX_HASH_LEN];
	int res;
	const u8 *addr[3];
	size_t len[3];
	size_t num_elem = 0;

	if (!auth->Mx_len || !auth->Nx_len) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Mx/Nx not available - cannot derive ke");
		return -1;
	}

	/* ke = HKDF(I-nonce | R-nonce, "DPP Key", M.x | N.x [| L.x]) */

	/* HKDF-Extract(I-nonce | R-nonce, M.x | N.x [| L.x]) */
	os_memcpy(nonces, auth->i_nonce, nonce_len);
	os_memcpy(&nonces[nonce_len], auth->r_nonce, nonce_len);
	addr[num_elem] = auth->Mx;
	len[num_elem] = auth->Mx_len;
	num_elem++;
	addr[num_elem] = auth->Nx;
	len[num_elem] = auth->Nx_len;
	num_elem++;
	if (auth->peer_bi && auth->own_bi) {
		if (!auth->Lx_len) {
			wpa_printf(MSG_DEBUG,
				   "DPP: Lx not available - cannot derive ke");
			return -1;
		}
		addr[num_elem] = auth->Lx;
		len[num_elem] = auth->secret_len;
		num_elem++;
	}
	res = dpp_hmac_vector(hash_len, nonces, 2 * nonce_len,
			      num_elem, addr, len, prk);
	if (res < 0)
		return -1;
	wpa_hexdump_key(MSG_DEBUG, "DPP: PRK = HKDF-Extract(<>, IKM)",
			prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = dpp_hkdf_expand(hash_len, prk, hash_len, info_ke, auth->ke, hash_len);
	forced_memzero(prk, hash_len);
	if (res < 0)
		return -1;

	wpa_hexdump_key(MSG_DEBUG, "DPP: ke = HKDF-Expand(PRK, info, L)",
			auth->ke, hash_len);
	return 0;
}

struct wpabuf * dpp_get_pubkey_point(struct crypto_ec_key *pkey, int prefix)
{
	int len, res;
	struct wpabuf *buf;
	unsigned char *pos = NULL;

	len = crypto_ec_get_publickey_buf(pkey, pos, 0);
	if (len <= 0) {
		wpa_printf(MSG_ERROR,
			   "DDP: Failed to determine public key encoding length");
		return NULL;
	}

	buf = wpabuf_alloc(len);
	if (!buf) {
		return NULL;
	}

	pos = wpabuf_put(buf, len);

	res = crypto_ec_get_publickey_buf(pkey, pos, len);
	if (res != len) {
		wpa_printf(MSG_ERROR,
			   "DDP: Failed to encode public key (res=%d/%d)",
			   res, len);
		wpabuf_free(buf);
		return NULL;
	}

	if (!prefix) {
		/* Remove 0x04 prefix to match DPP definition */
		pos = wpabuf_mhead(buf);
		os_memmove(pos, pos + 1, len - 1);
		buf->used--;
	}

	return buf;
}

int dpp_check_pubkey_match(struct crypto_ec_key *pub, struct wpabuf *r_hash)
{
	struct wpabuf *uncomp;
	int res;
	u8 hash[SHA256_MAC_LEN];
	const u8 *addr[1];
	size_t len[1];

	if (wpabuf_len(r_hash) != SHA256_MAC_LEN)
		return -1;
	uncomp = dpp_get_pubkey_point(pub, 1);
	if (!uncomp)
		return -1;
	addr[0] = wpabuf_head(uncomp);
	len[0] = wpabuf_len(uncomp);
	wpa_hexdump(MSG_DEBUG, "DPP: Uncompressed public key",
		    addr[0], len[0]);
	res = sha256_vector(1, addr, len, hash);
	wpabuf_free(uncomp);
	if (res < 0)
		return -1;
	if (os_memcmp(hash, wpabuf_head(r_hash), SHA256_MAC_LEN) != 0) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Received hash value does not match calculated public key hash value");
		wpa_hexdump(MSG_DEBUG, "DPP: Calculated hash",
			    hash, SHA256_MAC_LEN);
		return -1;
	}
	return 0;
}

enum dpp_status_error
dpp_process_signed_connector(struct dpp_signed_connector_info *info,
			     struct crypto_ec_key *csign_pub,
			     const char *connector)
{
	enum dpp_status_error ret = 255;
	const char *pos, *end, *signed_start, *signed_end;
	struct wpabuf *kid = NULL;
	unsigned char *prot_hdr = NULL, *signature = NULL;
	size_t prot_hdr_len = 0, signature_len = 0, signed_len;
	const struct dpp_curve_params *curve;
	int res, hash_func = -1;
	u8 *hash = NULL;

	curve = dpp_get_curve_ike_group(crypto_ec_key_group(csign_pub));
	if (!curve)
		goto fail;
	wpa_printf(MSG_DEBUG, "DPP: C-sign-key group: %s", curve->jwk_crv);
	os_memset(info, 0, sizeof(*info));

	signed_start = pos = connector;
	end = os_strchr(pos, '.');
	if (!end) {
		wpa_printf(MSG_DEBUG, "DPP: Missing dot(1) in signedConnector");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	prot_hdr = base64_url_decode(pos, end - pos, &prot_hdr_len);
	if (!prot_hdr) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Failed to base64url decode signedConnector JWS Protected Header");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	wpa_hexdump_ascii(MSG_DEBUG,
			  "DPP: signedConnector - JWS Protected Header",
			  (u8 *)prot_hdr, prot_hdr_len);
	kid = dpp_parse_jws_prot_hdr(curve, prot_hdr, prot_hdr_len, &hash_func);
	if (!kid) {
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	if (wpabuf_len(kid) != SHA256_MAC_LEN) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Unexpected signedConnector JWS Protected Header kid length: %u (expected %u)",
			   (unsigned int) wpabuf_len(kid), SHA256_MAC_LEN);
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	pos = end + 1;
	end = os_strchr(pos, '.');
	if (!end) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Missing dot(2) in signedConnector");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	signed_end = end - 1;
	info->payload = base64_url_decode(pos, end - pos, &info->payload_len);
	if (!info->payload) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Failed to base64url decode signedConnector JWS Payload");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	wpa_hexdump_ascii(MSG_DEBUG,
			  "DPP: signedConnector - JWS Payload",
			  (u8 *)info->payload, info->payload_len);
	pos = end + 1;
	signature = base64_url_decode(pos, os_strlen(pos), &signature_len);
	if (!signature) {
		wpa_printf(MSG_DEBUG,
			"DPP: Failed to base64url decode signedConnector signature");
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}
	wpa_hexdump(MSG_DEBUG, "DPP: signedConnector - signature",
		    signature, signature_len);

	if (dpp_check_pubkey_match(csign_pub, kid) < 0) {
		ret = DPP_STATUS_NO_MATCH;
		goto fail;
	}

	if (signature_len & 0x01) {
		wpa_printf(MSG_DEBUG,
			   "DPP: Unexpected signedConnector signature length (%d)",
			   (int) signature_len);
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	signed_len = signed_end - signed_start + 1;
	hash = os_malloc(curve->hash_len);
	if (!hash) {
		wpa_printf(MSG_ERROR, "malloc failed");
		goto fail;
	}

	if (hash_func == CRYPTO_HASH_ALG_SHA256) {
		if ((sha256_vector(1, (const u8 **) &signed_start, &signed_len, hash)) != 0) {
			goto fail;
		}
	} else {
		goto fail;
	}

	res = crypto_ec_key_verify_signature_r_s(csign_pub,
					(unsigned char *)hash, curve->hash_len,
						 signature, signature_len / 2,
						 signature + signature_len / 2,
						 signature_len / 2);
	if (res != 0) {
		wpa_printf(MSG_DEBUG,
			   "DPP: signedConnector signature check failed (res=%d)",
			   res);
		ret = DPP_STATUS_INVALID_CONNECTOR;
		goto fail;
	}

	ret = DPP_STATUS_OK;
fail:
	if (hash != NULL) {
		os_free(hash);
	}
	os_free(prot_hdr);
	wpabuf_free(kid);
	os_free(signature);
	return ret;
}

enum dpp_status_error
dpp_check_signed_connector(struct dpp_signed_connector_info *info,
                           const u8 *csign_key, size_t csign_key_len,
                           const u8 *peer_connector, size_t peer_connector_len)
{
	struct crypto_ec_key *csign;
	char *signed_connector = NULL;
	enum dpp_status_error res = DPP_STATUS_INVALID_CONNECTOR;
	const unsigned char *p;

	p = csign_key;
	csign = crypto_ec_parse_subpub_key(p, csign_key_len);
	if (!csign) {
		wpa_printf(MSG_ERROR,
			   "DPP: Failed to parse local C-sign-key information");
		goto fail;
	}
	wpa_hexdump_ascii(MSG_DEBUG, "DPP: Peer signedConnector",
			  (u8 *)peer_connector, peer_connector_len);
	signed_connector = os_malloc(peer_connector_len + 1);
	if (!signed_connector)
		goto fail;
	os_memcpy(signed_connector, peer_connector, peer_connector_len);
	signed_connector[peer_connector_len] = '\0';

	res = dpp_process_signed_connector(info, csign, signed_connector);
fail:
	os_free(signed_connector);
	crypto_ec_key_deinit(csign);
	return res;

}


int dpp_gen_r_auth(struct dpp_authentication *auth, u8 *r_auth)
{
	struct wpabuf *pix, *prx, *bix, *brx;
	const u8 *addr[7];
	size_t len[7];
	size_t i, num_elem = 0;
	size_t nonce_len;
	u8 zero = 0;
	int res = -1;

	/* R-auth = H(I-nonce | R-nonce | PI.x | PR.x | [BI.x |] BR.x | 0) */
	nonce_len = auth->curve->nonce_len;

	if (auth->initiator) {
		pix = dpp_get_pubkey_point(auth->own_protocol_key, 0);
		prx = dpp_get_pubkey_point(auth->peer_protocol_key, 0);
		if (auth->own_bi)
			bix = dpp_get_pubkey_point(auth->own_bi->pubkey, 0);
		else
			bix = NULL;
		brx = dpp_get_pubkey_point(auth->peer_bi->pubkey, 0);
	} else {
		pix = dpp_get_pubkey_point(auth->peer_protocol_key, 0);
		prx = dpp_get_pubkey_point(auth->own_protocol_key, 0);
		if (auth->peer_bi)
			bix = dpp_get_pubkey_point(auth->peer_bi->pubkey, 0);
		else
			bix = NULL;
		brx = dpp_get_pubkey_point(auth->own_bi->pubkey, 0);
	}
	if (!pix || !prx || !brx)
		goto fail;

	addr[num_elem] = auth->i_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = auth->r_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = wpabuf_head(pix);
	len[num_elem] = wpabuf_len(pix) / 2;
	num_elem++;

	addr[num_elem] = wpabuf_head(prx);
	len[num_elem] = wpabuf_len(prx) / 2;
	num_elem++;

	if (bix) {
		addr[num_elem] = wpabuf_head(bix);
		len[num_elem] = wpabuf_len(bix) / 2;
		num_elem++;
	}

	addr[num_elem] = wpabuf_head(brx);
	len[num_elem] = wpabuf_len(brx) / 2;
	num_elem++;

	addr[num_elem] = &zero;
	len[num_elem] = 1;
	num_elem++;

	wpa_printf(MSG_DEBUG, "DPP: R-auth hash components");
	for (i = 0; i < num_elem; i++)
		wpa_hexdump(MSG_DEBUG, "DPP: hash component", addr[i], len[i]);
	res = dpp_hash_vector(auth->curve, num_elem, addr, len, r_auth);
	if (res == 0)
		wpa_hexdump(MSG_DEBUG, "DPP: R-auth", r_auth,
			    auth->curve->hash_len);
fail:
	wpabuf_free(pix);
	wpabuf_free(prx);
	wpabuf_free(bix);
	wpabuf_free(brx);
	return res;
}


int dpp_gen_i_auth(struct dpp_authentication *auth, u8 *i_auth)
{
	struct wpabuf *pix = NULL, *prx = NULL, *bix = NULL, *brx = NULL;
	const u8 *addr[7];
	size_t len[7];
	size_t i, num_elem = 0;
	size_t nonce_len;
	u8 one = 1;
	int res = -1;

	/* I-auth = H(R-nonce | I-nonce | PR.x | PI.x | BR.x | [BI.x |] 1) */
	nonce_len = auth->curve->nonce_len;

	if (auth->initiator) {
		pix = dpp_get_pubkey_point(auth->own_protocol_key, 0);
		prx = dpp_get_pubkey_point(auth->peer_protocol_key, 0);
		if (auth->own_bi)
			bix = dpp_get_pubkey_point(auth->own_bi->pubkey, 0);
		else
			bix = NULL;
		if (!auth->peer_bi)
			goto fail;
		brx = dpp_get_pubkey_point(auth->peer_bi->pubkey, 0);
	} else {
		pix = dpp_get_pubkey_point(auth->peer_protocol_key, 0);
		prx = dpp_get_pubkey_point(auth->own_protocol_key, 0);
		if (auth->peer_bi)
			bix = dpp_get_pubkey_point(auth->peer_bi->pubkey, 0);
		else
			bix = NULL;
		if (!auth->own_bi)
			goto fail;
		brx = dpp_get_pubkey_point(auth->own_bi->pubkey, 0);
	}
	if (!pix || !prx || !brx)
		goto fail;

	addr[num_elem] = auth->r_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = auth->i_nonce;
	len[num_elem] = nonce_len;
	num_elem++;

	addr[num_elem] = wpabuf_head(prx);
	len[num_elem] = wpabuf_len(prx) / 2;
	num_elem++;

	addr[num_elem] = wpabuf_head(pix);
	len[num_elem] = wpabuf_len(pix) / 2;
	num_elem++;

	addr[num_elem] = wpabuf_head(brx);
	len[num_elem] = wpabuf_len(brx) / 2;
	num_elem++;

	if (bix) {
		addr[num_elem] = wpabuf_head(bix);
		len[num_elem] = wpabuf_len(bix) / 2;
		num_elem++;
	}

	addr[num_elem] = &one;
	len[num_elem] = 1;
	num_elem++;

	wpa_printf(MSG_DEBUG, "DPP: I-auth hash components");
	for (i = 0; i < num_elem; i++)
		wpa_hexdump(MSG_DEBUG, "DPP: hash component", addr[i], len[i]);
	res = dpp_hash_vector(auth->curve, num_elem, addr, len, i_auth);
	if (res == 0)
		wpa_hexdump(MSG_DEBUG, "DPP: I-auth", i_auth,
			    auth->curve->hash_len);
fail:
	wpabuf_free(pix);
	wpabuf_free(prx);
	wpabuf_free(bix);
	wpabuf_free(brx);
	return res;
}

int dpp_bn2bin_pad(const struct crypto_bignum *bn, u8 *pos, size_t len)
{
	if (crypto_bignum_to_bin(bn, pos, len, 0) < 0)
		return -1;

	return 0;
}

int dpp_auth_derive_l_responder(struct dpp_authentication *auth)
{
	struct crypto_ec_group *group;
	struct crypto_ec_point *L = NULL;
	struct crypto_ec_point *BI;
	struct crypto_bignum *lx, *sum, *q;
	struct crypto_bignum *bR, *pR;
	int ret = -1;

	/* L = ((bR + pR) modulo q) * BI */

	sum = crypto_bignum_init();
	q = crypto_bignum_init();
	lx = crypto_bignum_init();
	if (!sum || !q || !lx)
		goto fail;

	BI = crypto_ec_key_get_public_key(auth->peer_bi->pubkey);
	group = crypto_ec_get_group_from_key(auth->peer_bi->pubkey);
	bR = crypto_ec_key_get_private_key(auth->own_bi->pubkey);
	pR = crypto_ec_key_get_private_key(auth->own_protocol_key);

	if (!bR || !pR)
		goto fail;

	if ((crypto_get_order(group, q) != 0) ||
	    (crypto_bignum_addmod(sum, bR, pR, q) != 0))
		goto fail;

	L = crypto_ec_point_init((struct crypto_ec *)group);
	if (!L || (crypto_ec_point_mul((struct crypto_ec *)group, BI, sum, L) != 0) ||
	    (crypto_ec_get_affine_coordinates((struct crypto_ec *)group, L, lx, NULL) != 0)) {
		wpa_printf(MSG_ERROR,
				"OpenSSL: failed: %s", __func__);
		goto fail;
	}
	if (dpp_bn2bin_pad(lx, auth->Lx, auth->secret_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "DPP: L.x", auth->Lx, auth->secret_len);
	auth->Lx_len = auth->secret_len;
	ret = 0;
fail:
	crypto_ec_point_deinit(L, 1);
	crypto_bignum_deinit(lx, 0);
	crypto_bignum_deinit(sum, 0);
	crypto_bignum_deinit(q, 0);

	return ret;
}


int dpp_auth_derive_l_initiator(struct dpp_authentication *auth)
{
	struct crypto_ec_group *group;
	struct crypto_ec_point *L = NULL, *sum = NULL;
	struct crypto_ec_point *BR_point, *PR_point;
	struct crypto_bignum *lx;
	struct crypto_bignum *bI_bn;
	int ret = -1;

	/* L = bI * (BR + PR) */

	lx = crypto_bignum_init();
	if (!lx)
		goto fail;
	BR_point = crypto_ec_key_get_public_key(auth->peer_bi->pubkey);
	PR_point = crypto_ec_key_get_public_key(auth->peer_protocol_key);

	group = crypto_ec_get_group_from_key(auth->own_bi->pubkey);
	bI_bn = crypto_ec_key_get_private_key(auth->own_bi->pubkey);
	if (!group || !bI_bn)
		goto fail;
	sum = crypto_ec_point_init((struct crypto_ec *)group);
	L = crypto_ec_point_init((struct crypto_ec *)group);
	if (!sum || !L ||
	    crypto_ec_point_add((struct crypto_ec *)group, BR_point, PR_point, sum) != 0 ||
	    crypto_ec_point_mul((struct crypto_ec *)group, sum, bI_bn, L) != 0 ||
	    crypto_ec_get_affine_coordinates((struct crypto_ec *)group, L, lx, NULL) != 0) {
		wpa_printf(MSG_ERROR,
			   "OpenSSL: failed: %s", __func__);
		goto fail;
	}

	if (dpp_bn2bin_pad(lx, auth->Lx, auth->secret_len) < 0)
		goto fail;
	wpa_hexdump_key(MSG_DEBUG, "DPP: L.x", auth->Lx, auth->secret_len);
	auth->Lx_len = auth->secret_len;
	ret = 0;
fail:
	crypto_ec_point_deinit(L, 1);
	crypto_ec_point_deinit(sum, 1);
	crypto_bignum_deinit(lx, 0);


	return ret;
}


int dpp_derive_pmk(const u8 *Nx, size_t Nx_len, u8 *pmk, unsigned int hash_len)
{
	u8 salt[DPP_MAX_HASH_LEN], prk[DPP_MAX_HASH_LEN];
	const char *info = "DPP PMK";
	int res;

	/* PMK = HKDF(<>, "DPP PMK", N.x) */

	/* HKDF-Extract(<>, N.x) */
	os_memset(salt, 0, hash_len);
	if (dpp_hmac(hash_len, salt, hash_len, Nx, Nx_len, prk) < 0)
		return -1;
	wpa_hexdump_key(MSG_DEBUG, "DPP: PRK = HKDF-Extract(<>, IKM=N.x)",
			prk, hash_len);

	/* HKDF-Expand(PRK, info, L) */
	res = dpp_hkdf_expand(hash_len, prk, hash_len, info, pmk, hash_len);
	forced_memzero(prk, hash_len);
	if (res < 0)
		return -1;

	wpa_hexdump_key(MSG_DEBUG, "DPP: PMK = HKDF-Expand(PRK, info, L)",
			pmk, hash_len);
	return 0;
}


int dpp_derive_pmkid(const struct dpp_curve_params *curve,
		     struct crypto_ec_key *own_key,
		     struct crypto_ec_key *peer_key, u8 *pmkid)
{
	struct wpabuf *nkx, *pkx;
	int ret = -1, res;
	const u8 *addr[2];
	size_t len[2];
	u8 hash[SHA256_MAC_LEN];

	/* PMKID = Truncate-128(H(min(NK.x, PK.x) | max(NK.x, PK.x))) */
	nkx = dpp_get_pubkey_point(own_key, 0);
	pkx = dpp_get_pubkey_point(peer_key, 0);
	if (!nkx || !pkx)
		goto fail;
	addr[0] = wpabuf_head(nkx);
	len[0] = wpabuf_len(nkx) / 2;
	addr[1] = wpabuf_head(pkx);
	len[1] = wpabuf_len(pkx) / 2;
	if (len[0] != len[1])
		goto fail;
	if (os_memcmp(addr[0], addr[1], len[0]) > 0) {
		addr[0] = wpabuf_head(pkx);
		addr[1] = wpabuf_head(nkx);
	}
	wpa_hexdump(MSG_DEBUG, "DPP: PMKID hash payload 1", addr[0], len[0]);
	wpa_hexdump(MSG_DEBUG, "DPP: PMKID hash payload 2", addr[1], len[1]);
	res = sha256_vector(2, addr, len, hash);
	if (res < 0)
		goto fail;
	wpa_hexdump(MSG_DEBUG, "DPP: PMKID hash output", hash, SHA256_MAC_LEN);
	os_memcpy(pmkid, hash, PMKID_LEN);
	wpa_hexdump(MSG_DEBUG, "DPP: PMKID", pmkid, PMKID_LEN);
	ret = 0;
fail:
	wpabuf_free(nkx);
	wpabuf_free(pkx);
	return ret;
}

#ifdef CONFIG_TESTING_OPTIONS
int dpp_test_gen_invalid_key(struct wpabuf *msg,
			     const struct dpp_curve_params *curve)
{
	return 0;
}

char * dpp_corrupt_connector_signature(const char *connector)
{
	char *tmp, *pos, *signed3 = NULL;
	unsigned char *signature = NULL;
	size_t signature_len = 0, signed3_len;

	tmp = os_zalloc(os_strlen(connector) + 5);
	if (!tmp)
		goto fail;
	os_memcpy(tmp, connector, os_strlen(connector));

	pos = os_strchr(tmp, '.');
	if (!pos)
		goto fail;

	pos = os_strchr(pos + 1, '.');
	if (!pos)
		goto fail;
	pos++;

	wpa_printf(MSG_DEBUG, "DPP: Original base64url encoded signature: %s",
		   pos);
	signature = base64_url_decode(pos, os_strlen(pos), &signature_len);
	if (!signature || signature_len == 0)
		goto fail;
	wpa_hexdump(MSG_DEBUG, "DPP: Original Connector signature",
		    signature, signature_len);
	signature[signature_len - 1] ^= 0x01;
	wpa_hexdump(MSG_DEBUG, "DPP: Corrupted Connector signature",
		    signature, signature_len);
	signed3 = base64_url_encode(signature, signature_len, &signed3_len);
	if (!signed3)
		goto fail;
	os_memcpy(pos, signed3, signed3_len);
	pos[signed3_len] = '\0';
	wpa_printf(MSG_DEBUG, "DPP: Corrupted base64url encoded signature: %s",
		   pos);

out:
	os_free(signature);
	os_free(signed3);
	return tmp;
fail:
	os_free(tmp);
	tmp = NULL;
	goto out;
}
#endif /* CONFIG_TESTING_OPTIONS */
