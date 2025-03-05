/*
 * Counter with CBC-MAC (CCM) with AES
 *
 * Copyright (c) 2010-2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifdef CONFIG_IEEE80211W
#include "includes.h"

#include "common.h"
#include "aes.h"
#include "aes_wrap.h"
#include "psa/crypto.h"


static void xor_aes_block(u8 *dst, const u8 *src)
{
	u32 *d = (u32 *) dst;
	u32 *s = (u32 *) src;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
	*d++ ^= *s++;
}


static void aes_ccm_auth_start(void *aes, size_t M, size_t L, const u8 *nonce,
			       const u8 *aad, size_t aad_len, size_t plain_len,
			       u8 *x)
{
	u8 aad_buf[2 * AES_BLOCK_SIZE];
	u8 b[AES_BLOCK_SIZE];

	/* Authentication */
	/* B_0: Flags | Nonce N | l(m) */
	b[0] = aad_len ? 0x40 : 0 /* Adata */;
	b[0] |= (((M - 2) / 2) /* M' */ << 3);
	b[0] |= (L - 1) /* L' */;
	os_memcpy(&b[1], nonce, 15 - L);
	WPA_PUT_BE16(&b[AES_BLOCK_SIZE - L], plain_len);

	wpa_hexdump_key(MSG_DEBUG, "CCM B_0", b, AES_BLOCK_SIZE);
	aes_encrypt(aes, b, x); /* X_1 = E(K, B_0) */

	if (!aad_len)
		return;

	WPA_PUT_BE16(aad_buf, aad_len);
	os_memcpy(aad_buf + 2, aad, aad_len);
	os_memset(aad_buf + 2 + aad_len, 0, sizeof(aad_buf) - 2 - aad_len);

	xor_aes_block(aad_buf, x);
	aes_encrypt(aes, aad_buf, x); /* X_2 = E(K, X_1 XOR B_1) */

	if (aad_len > AES_BLOCK_SIZE - 2) {
		xor_aes_block(&aad_buf[AES_BLOCK_SIZE], x);
		/* X_3 = E(K, X_2 XOR B_2) */
		aes_encrypt(aes, &aad_buf[AES_BLOCK_SIZE], x);
	}
}


static void aes_ccm_auth(void *aes, const u8 *data, size_t len, u8 *x)
{
	size_t last = len % AES_BLOCK_SIZE;
	size_t i;

	for (i = 0; i < len / AES_BLOCK_SIZE; i++) {
		/* X_i+1 = E(K, X_i XOR B_i) */
		xor_aes_block(x, data);
		data += AES_BLOCK_SIZE;
		aes_encrypt(aes, x, x);
	}
	if (last) {
		/* XOR zero-padded last block */
		for (i = 0; i < last; i++)
			x[i] ^= *data++;
		aes_encrypt(aes, x, x);
	}
}


static void aes_ccm_encr_start(size_t L, const u8 *nonce, u8 *a)
{
	/* A_i = Flags | Nonce N | Counter i */
	a[0] = L - 1; /* Flags = L' */
	os_memcpy(&a[1], nonce, 15 - L);
}


static void aes_ccm_encr(void *aes, size_t L, const u8 *in, size_t len, u8 *out,
			 u8 *a)
{
	size_t last = len % AES_BLOCK_SIZE;
	size_t i;

	/* crypt = msg XOR (S_1 | S_2 | ... | S_n) */
	for (i = 1; i <= len / AES_BLOCK_SIZE; i++) {
		WPA_PUT_BE16(&a[AES_BLOCK_SIZE - 2], i);
		/* S_i = E(K, A_i) */
		aes_encrypt(aes, a, out);
		xor_aes_block(out, in);
		out += AES_BLOCK_SIZE;
		in += AES_BLOCK_SIZE;
	}
	if (last) {
		WPA_PUT_BE16(&a[AES_BLOCK_SIZE - 2], i);
		aes_encrypt(aes, a, out);
		/* XOR zero-padded last block */
		for (i = 0; i < last; i++)
			*out++ ^= *in++;
	}
}


static void aes_ccm_encr_auth(void *aes, size_t M, u8 *x, u8 *a, u8 *auth)
{
	size_t i;
	u8 tmp[AES_BLOCK_SIZE];

	wpa_hexdump_key(MSG_DEBUG, "CCM T", x, M);
	/* U = T XOR S_0; S_0 = E(K, A_0) */
	WPA_PUT_BE16(&a[AES_BLOCK_SIZE - 2], 0);
	aes_encrypt(aes, a, tmp);
	for (i = 0; i < M; i++)
		auth[i] = x[i] ^ tmp[i];
	wpa_hexdump_key(MSG_DEBUG, "CCM U", auth, M);
}

/* AES-CCM with fixed L=2 and aad_len <= 30 assumption */
int aes_ccm_ae(const u8 *key, size_t key_len, const u8 *nonce,
	       size_t M, const u8 *plain, size_t plain_len,
	       const u8 *aad, size_t aad_len, u8 *crypt, u8 *auth)
{
	const size_t L = 2;
	void *aes;
	u8 x[AES_BLOCK_SIZE], a[AES_BLOCK_SIZE];

	if (aad_len > 30 || M > AES_BLOCK_SIZE)
		return -1;

	aes = aes_encrypt_init(key, key_len);
	if (aes == NULL)
		return -1;

	aes_ccm_auth_start(aes, M, L, nonce, aad, aad_len, plain_len, x);
	aes_ccm_auth(aes, plain, plain_len, x);

	/* Encryption */
	aes_ccm_encr_start(L, nonce, a);
	aes_ccm_encr(aes, L, plain, plain_len, crypt, a);
	aes_ccm_encr_auth(aes, M, x, a, auth);

	aes_encrypt_deinit(aes);

	return 0;
}


/* AES-CCM with fixed L=2 and aad_len <= 30 assumption */
int aes_ccm_ad(const u8 *key, size_t key_len, const u8 *nonce,
	       size_t M, const u8 *crypt, size_t crypt_len,
	       const u8 *aad, size_t aad_len, const u8 *auth, u8 *plain)
{
	psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id;

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_crypto_init failed", __func__);
        return -1;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_len * 8);

    status = psa_import_key(&attributes, key, key_len, &key_id);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_import_key failed", __func__);
        return -1;
    }

    psa_reset_key_attributes(&attributes);

    psa_aead_operation_t operation = PSA_AEAD_OPERATION_INIT;

    status = psa_aead_decrypt_setup(&operation, key_id, PSA_ALG_CCM);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_decrypt_setup failed", __func__);
        return -1;
    }

    status = psa_aead_set_nonce(&operation, nonce, 13);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_set_nonce failed", __func__);
        return -1;
    }

    status = psa_aead_set_lengths(&operation, aad_len, crypt_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_set_lengths failed", __func__);
        return -1;
    }

    size_t output_length = 0;

    status = psa_aead_update_ad(&operation, aad, aad_len);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_update_ad failed", __func__);
        return -1;
    }

    status = psa_aead_update(&operation, crypt, crypt_len, plain, crypt_len, &output_length);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_update failed", __func__);
        return -1;
    }

    status = psa_aead_verify(&operation, plain + output_length, 16 - output_length, &output_length, auth, M);
    if (status != PSA_SUCCESS) {
        wpa_printf(MSG_ERROR, "%s: psa_aead_verify failed", __func__);
        return -1;
    }

    psa_aead_abort(&operation);

    psa_destroy_key(key_id);

    return 0;
}
#endif /* CONFIG_IEEE80211W */
