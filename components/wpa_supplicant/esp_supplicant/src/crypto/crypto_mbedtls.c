/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef ESP_PLATFORM
#include "esp_system.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "random.h"
#include "sha256.h"

#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"
#include "mbedtls/aes.h"
#include "mbedtls/bignum.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/cmac.h"
#include "mbedtls/nist_kw.h"
#include "mbedtls/des.h"
#include "mbedtls/ccm.h"

#include "common.h"
#include "utils/wpabuf.h"
#include "dh_group5.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha384.h"
#include "aes_wrap.h"
#include "crypto.h"
#include "mbedtls/esp_config.h"

#ifdef CONFIG_FAST_PBKDF2
#include "fastpbkdf2.h"
#endif

static int digest_vector(mbedtls_md_type_t md_type, size_t num_elem,
			 const u8 *addr[], const size_t *len, u8 *mac)
{
	size_t i;
	const mbedtls_md_info_t *md_info;
	mbedtls_md_context_t md_ctx;
	int ret;

	mbedtls_md_init(&md_ctx);

	md_info = mbedtls_md_info_from_type(md_type);
	if (!md_info) {
		wpa_printf(MSG_ERROR, "mbedtls_md_info_from_type() failed");
		return -1;
	}

	ret = mbedtls_md_setup(&md_ctx, md_info, 0);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_md_setup() returned error");
		goto cleanup;
	}

	ret = mbedtls_md_starts(&md_ctx);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_md_starts returned error");
		goto cleanup;
	}

	for (i = 0; i < num_elem; i++) {
		ret = mbedtls_md_update(&md_ctx, addr[i], len[i]);
		if (ret != 0) {
			wpa_printf(MSG_ERROR, "mbedtls_md_update ret=%d", ret);
			goto cleanup;
		}
	}

	ret = mbedtls_md_finish(&md_ctx, mac);
cleanup:
	mbedtls_md_free(&md_ctx);

	return ret;

}

int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
	return digest_vector(MBEDTLS_MD_SHA256, num_elem, addr, len, mac);
}

int sha384_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
	return digest_vector(MBEDTLS_MD_SHA384, num_elem, addr, len, mac);
}

int sha512_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
	return digest_vector(MBEDTLS_MD_SHA512, num_elem, addr, len, mac);
}

int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return digest_vector(MBEDTLS_MD_SHA1, num_elem, addr, len, mac);
}

int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return digest_vector(MBEDTLS_MD_MD5, num_elem, addr, len, mac);
}

#ifdef MBEDTLS_MD4_C
int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	return digest_vector(MBEDTLS_MD_MD4, num_elem, addr, len, mac);
}
#endif

struct crypto_hash * crypto_hash_init(enum crypto_hash_alg alg, const u8 *key,
				      size_t key_len)
{
	mbedtls_md_context_t *ctx = NULL;
	mbedtls_md_type_t md_type;
	const mbedtls_md_info_t *md_info;
	int ret;
	int is_hmac = 0;

	switch (alg) {
	case CRYPTO_HASH_ALG_MD5:
	case CRYPTO_HASH_ALG_HMAC_MD5:
		md_type = MBEDTLS_MD_MD5;
		break;
	case CRYPTO_HASH_ALG_SHA1:
	case CRYPTO_HASH_ALG_HMAC_SHA1:
		md_type = MBEDTLS_MD_SHA1;
		break;
	case CRYPTO_HASH_ALG_SHA256:
	case CRYPTO_HASH_ALG_HMAC_SHA256:
		md_type = MBEDTLS_MD_SHA256;
		break;
	case CRYPTO_HASH_ALG_SHA384:
		md_type = MBEDTLS_MD_SHA384;
		break;
	case CRYPTO_HASH_ALG_SHA512:
		md_type = MBEDTLS_MD_SHA512;
		break;
	default:
		return NULL;
	}

	switch (alg) {
	case CRYPTO_HASH_ALG_HMAC_MD5:
	case CRYPTO_HASH_ALG_HMAC_SHA1:
	case CRYPTO_HASH_ALG_HMAC_SHA256:
		is_hmac = 1;
		break;
	default:
		break;
	}
	ctx = os_zalloc(sizeof(*ctx));
	if (ctx == NULL) {
		return NULL;
	}

	mbedtls_md_init(ctx);
	md_info = mbedtls_md_info_from_type(md_type);
	if (!md_info) {
		goto cleanup;
	}
	if (mbedtls_md_setup(ctx, md_info, is_hmac) != 0) {
		goto cleanup;
	}
	if (is_hmac) {
		ret = mbedtls_md_hmac_starts(ctx, key, key_len);
	} else {
		ret = mbedtls_md_starts(ctx);
	}
	if (ret < 0) {
		goto cleanup;
	}

	return (struct crypto_hash *)ctx;
cleanup:
	mbedtls_md_free(ctx);
	os_free(ctx);
	return NULL;
}

void crypto_hash_update(struct crypto_hash *crypto_ctx, const u8 *data, size_t len)
{
	int ret;
	mbedtls_md_context_t *ctx = (mbedtls_md_context_t *)crypto_ctx;

	if (ctx == NULL) {
		return;
	}
	if (ctx->MBEDTLS_PRIVATE(hmac_ctx)) {
		ret = mbedtls_md_hmac_update(ctx, data, len);
	} else {
		ret = mbedtls_md_update(ctx, data, len);
	}
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "%s: mbedtls_md_hmac_update failed", __func__);
	}
}

int crypto_hash_finish(struct crypto_hash *crypto_ctx, u8 *mac, size_t *len)
{
	int ret = 0;
	mbedtls_md_type_t md_type;
	mbedtls_md_context_t *ctx = (mbedtls_md_context_t *)crypto_ctx;

	if (ctx == NULL) {
		return -2;
	}

	if (mac == NULL || len == NULL) {
		goto err;
	}

	md_type = mbedtls_md_get_type(mbedtls_md_info_from_ctx(ctx));
	switch(md_type) {
	case MBEDTLS_MD_MD5:
		if (*len < MD5_MAC_LEN) {
			*len = MD5_MAC_LEN;
			ret = -1;
			goto err;
		}
		*len = MD5_MAC_LEN;
		break;
	case MBEDTLS_MD_SHA1:
		if (*len < SHA1_MAC_LEN) {
			*len = SHA1_MAC_LEN;
			ret = -1;
			goto err;
		}
		*len = SHA1_MAC_LEN;
		break;
	case MBEDTLS_MD_SHA256:
		if (*len < SHA256_MAC_LEN) {
			*len = SHA256_MAC_LEN;
			ret = -1;
			goto err;
		}
		*len = SHA256_MAC_LEN;
		break;
	case MBEDTLS_MD_SHA384:
		if (*len < SHA384_MAC_LEN) {
			*len = SHA384_MAC_LEN;
			ret = -1;
			goto err;
		}
		*len = SHA384_MAC_LEN;
		break;
	case MBEDTLS_MD_SHA512:
		if (*len < SHA512_MAC_LEN) {
			*len = SHA512_MAC_LEN;
			ret = -1;
			goto err;
		}
		*len = SHA512_MAC_LEN;
		break;
	default:
		*len = 0;
		ret = -1;
		goto err;
	}
	if (ctx->MBEDTLS_PRIVATE(hmac_ctx)) {
		ret = mbedtls_md_hmac_finish(ctx, mac);
	} else {
		ret = mbedtls_md_finish(ctx, mac);
	}

err:
	mbedtls_md_free(ctx);
	bin_clear_free(ctx, sizeof(*ctx));

	return ret;
}

static int hmac_vector(mbedtls_md_type_t md_type,
		       const u8 *key, size_t key_len,
		       size_t num_elem, const u8 *addr[],
		       const size_t *len, u8 *mac)
{
	size_t i;
	const mbedtls_md_info_t *md_info;
	mbedtls_md_context_t md_ctx;
	int ret;

	mbedtls_md_init(&md_ctx);

	md_info = mbedtls_md_info_from_type(md_type);
	if (!md_info) {
		return -1;
	}

	ret = mbedtls_md_setup(&md_ctx, md_info, 1);
	if (ret != 0) {
		return(ret);
	}

	ret = mbedtls_md_hmac_starts(&md_ctx, key, key_len);
	if (ret != 0) {
		return(ret);
	}

	for (i = 0; i < num_elem; i++) {
		ret = mbedtls_md_hmac_update(&md_ctx, addr[i], len[i]);
		if (ret != 0) {
			return(ret);
		}

	}

	ret = mbedtls_md_hmac_finish(&md_ctx, mac);

	mbedtls_md_free(&md_ctx);

	return ret;
}

int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem,
		const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(MBEDTLS_MD_SHA384, key, key_len, num_elem, addr,
			   len, mac);
}


int hmac_sha384(const u8 *key, size_t key_len, const u8 *data,
		size_t data_len, u8 *mac)
{
	return hmac_sha384_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
		       const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(MBEDTLS_MD_SHA256, key, key_len, num_elem, addr,
			   len, mac);
}

int hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
		size_t data_len, u8 *mac)
{
	return hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_md5_vector(const u8 *key, size_t key_len, size_t num_elem,
		    const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(MBEDTLS_MD_MD5, key, key_len,
			   num_elem, addr, len, mac);
}

int hmac_md5(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	     u8 *mac)
{
	return hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}

int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	return hmac_vector(MBEDTLS_MD_SHA1, key, key_len, num_elem, addr,
			   len, mac);
}

int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	      u8 *mac)
{
	return hmac_sha1_vector(key, key_len, 1, &data, &data_len, mac);
}

static void *aes_crypt_init(int mode, const u8 *key, size_t len)
{
	int ret = -1;
	mbedtls_aes_context *aes = os_malloc(sizeof(*aes));
	if (!aes) {
		return NULL;
	}
	mbedtls_aes_init(aes);

	if (mode == MBEDTLS_AES_ENCRYPT) {
		ret = mbedtls_aes_setkey_enc(aes, key, len * 8);
	} else if (mode == MBEDTLS_AES_DECRYPT){
		ret = mbedtls_aes_setkey_dec(aes, key, len * 8);
	}
	if (ret < 0) {
		mbedtls_aes_free(aes);
		os_free(aes);
		wpa_printf(MSG_ERROR, "%s: mbedtls_aes_setkey_enc/mbedtls_aes_setkey_dec failed", __func__);
		return NULL;
	}

	return (void *) aes;
}

static int aes_crypt(void *ctx, int mode, const u8 *in, u8 *out)
{
	return mbedtls_aes_crypt_ecb((mbedtls_aes_context *)ctx,
				     mode, in, out);
}

static void aes_crypt_deinit(void *ctx)
{
	mbedtls_aes_free((mbedtls_aes_context *)ctx);
	os_free(ctx);
}

void *aes_encrypt_init(const u8 *key, size_t len)
{
	return aes_crypt_init(MBEDTLS_AES_ENCRYPT, key, len);
}

int aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	return aes_crypt(ctx, MBEDTLS_AES_ENCRYPT, plain, crypt);
}

void aes_encrypt_deinit(void *ctx)
{
	return aes_crypt_deinit(ctx);
}

void * aes_decrypt_init(const u8 *key, size_t len)
{
	return aes_crypt_init(MBEDTLS_AES_DECRYPT, key, len);
}

int aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
	return aes_crypt(ctx, MBEDTLS_AES_DECRYPT, crypt, plain);
}

void aes_decrypt_deinit(void *ctx)
{
	return aes_crypt_deinit(ctx);
}

int aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
	int ret = 0;
	mbedtls_aes_context ctx;
	u8 cbc[MBEDTLS_AES_BLOCK_SIZE];

	mbedtls_aes_init(&ctx);

	ret = mbedtls_aes_setkey_enc(&ctx, key, 128);
	if (ret < 0) {
		mbedtls_aes_free(&ctx);
		return ret;
	}

	os_memcpy(cbc, iv, MBEDTLS_AES_BLOCK_SIZE);
	ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT,
				    data_len, cbc, data, data);
	mbedtls_aes_free(&ctx);

	return ret;
}

int aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
	int ret = 0;
	mbedtls_aes_context ctx;
	u8 cbc[MBEDTLS_AES_BLOCK_SIZE];

	mbedtls_aes_init(&ctx);

	ret = mbedtls_aes_setkey_dec(&ctx, key, 128);
	if (ret < 0) {
		mbedtls_aes_free(&ctx);
		return ret;
	}

	os_memcpy(cbc, iv, MBEDTLS_AES_BLOCK_SIZE);
	ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT,
				    data_len, cbc, data, data);
	mbedtls_aes_free(&ctx);

	return ret;

}

#ifdef CONFIG_TLS_INTERNAL_CLIENT
struct crypto_cipher {
	mbedtls_cipher_context_t ctx_enc;
	mbedtls_cipher_context_t ctx_dec;
};

static int crypto_init_cipher_ctx(mbedtls_cipher_context_t *ctx,
				  const mbedtls_cipher_info_t *cipher_info,
				  const u8 *iv, const u8 *key, size_t key_len,
				  mbedtls_operation_t operation)
{
	mbedtls_cipher_init(ctx);
	int ret;

	ret = mbedtls_cipher_setup(ctx, cipher_info);
	if (ret != 0) {
		return -1;
	}

	ret = mbedtls_cipher_setkey(ctx, key, key_len * 8, operation);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_cipher_setkey returned error=%d", ret);
		return -1;
	}
	ret = mbedtls_cipher_set_iv(ctx, iv, cipher_info->MBEDTLS_PRIVATE(iv_size) << MBEDTLS_IV_SIZE_SHIFT);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_cipher_set_iv returned error=%d", ret);
		return -1;
	}
	ret = mbedtls_cipher_reset(ctx);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_cipher_reset() returned error=%d", ret);
		return -1;
	}

	return 0;
}

static mbedtls_cipher_type_t alg_to_mbedtls_cipher(enum crypto_cipher_alg alg,
						   size_t key_len)
{
	switch (alg) {
	case CRYPTO_CIPHER_ALG_AES:
		if (key_len == 16) {
			return MBEDTLS_CIPHER_AES_128_CBC;
		}
		if (key_len == 24) {
			return MBEDTLS_CIPHER_AES_192_CBC;
		}
		if (key_len == 32) {
			return MBEDTLS_CIPHER_AES_256_CBC;
		}
		break;
#ifdef MBEDTLS_DES_C
	case CRYPTO_CIPHER_ALG_3DES:
		return MBEDTLS_CIPHER_DES_EDE3_CBC;
	case CRYPTO_CIPHER_ALG_DES:
		return MBEDTLS_CIPHER_DES_CBC;
#endif
	default:
		break;
	}

	return MBEDTLS_CIPHER_NONE;
}

struct crypto_cipher *crypto_cipher_init(enum crypto_cipher_alg alg,
					 const u8 *iv, const u8 *key,
					 size_t key_len)
{
	struct crypto_cipher *ctx;
	mbedtls_cipher_type_t cipher_type;
	const mbedtls_cipher_info_t *cipher_info;

	ctx = (struct crypto_cipher *)os_zalloc(sizeof(*ctx));
	if (!ctx) {
		return NULL;
	}

	cipher_type = alg_to_mbedtls_cipher(alg, key_len);
	if (cipher_type == MBEDTLS_CIPHER_NONE) {
		goto cleanup;
	}

	cipher_info = mbedtls_cipher_info_from_type(cipher_type);
	if (cipher_info == NULL) {
		goto cleanup;
	}

	/* Init both ctx encryption/decryption */
	if (crypto_init_cipher_ctx(&ctx->ctx_enc, cipher_info, iv, key,
				   key_len, MBEDTLS_ENCRYPT) < 0) {
		goto cleanup;
	}

	if (crypto_init_cipher_ctx(&ctx->ctx_dec, cipher_info, iv, key,
				   key_len, MBEDTLS_DECRYPT) < 0) {
		goto cleanup;
	}

	if (mbedtls_cipher_set_padding_mode(&ctx->ctx_enc, MBEDTLS_PADDING_NONE) < 0) {
		goto cleanup;
	}
	if (mbedtls_cipher_set_padding_mode(&ctx->ctx_dec, MBEDTLS_PADDING_NONE) < 0) {
		goto cleanup;
	}
	return ctx;

cleanup:
	os_free(ctx);
	return NULL;
}

int crypto_cipher_encrypt(struct crypto_cipher *ctx, const u8 *plain,
			  u8 *crypt, size_t len)
{
	int ret;
	size_t olen = 0;

	ret = mbedtls_cipher_update(&ctx->ctx_enc, plain, len, crypt, &olen);
	if (ret != 0) {
		return -1;
	}

	ret = mbedtls_cipher_finish(&ctx->ctx_enc, crypt + olen, &olen);
	if (ret != 0) {
		return -1;
	}

	return 0;
}

int crypto_cipher_decrypt(struct crypto_cipher *ctx, const u8 *crypt,
			  u8 *plain, size_t len)
{
	int ret;
	size_t olen = 0;

	ret = mbedtls_cipher_update(&ctx->ctx_dec, crypt, len, plain, &olen);
	if (ret != 0) {
		return -1;
	}

	ret = mbedtls_cipher_finish(&ctx->ctx_dec, plain + olen, &olen);
	if (ret != 0) {
		return -1;
	}

	return 0;
}

void crypto_cipher_deinit(struct crypto_cipher *ctx)
{
	mbedtls_cipher_free(&ctx->ctx_enc);
	mbedtls_cipher_free(&ctx->ctx_dec);
	os_free(ctx);
}
#endif

int aes_ctr_encrypt(const u8 *key, size_t key_len, const u8 *nonce,
		    u8 *data, size_t data_len)
{
	int ret;
	mbedtls_aes_context ctx;
	uint8_t stream_block[MBEDTLS_AES_BLOCK_SIZE];
	size_t offset = 0;

	mbedtls_aes_init(&ctx);
	ret = mbedtls_aes_setkey_enc(&ctx, key, key_len * 8);
	if (ret < 0) {
		goto cleanup;
	}
	ret = mbedtls_aes_crypt_ctr(&ctx, data_len, &offset, (u8 *)nonce,
				    stream_block, data, data);
cleanup:
	mbedtls_aes_free(&ctx);
	return ret;
}

int aes_128_ctr_encrypt(const u8 *key, const u8 *nonce,
			u8 *data, size_t data_len)
{
	return aes_ctr_encrypt(key, 16, nonce, data, data_len);
}


#ifdef MBEDTLS_NIST_KW_C
int aes_wrap(const u8 *kek, size_t kek_len, int n, const u8 *plain, u8 *cipher)
{
	mbedtls_nist_kw_context ctx;
	size_t olen;
	int ret = 0;
	mbedtls_nist_kw_init(&ctx);

	ret = mbedtls_nist_kw_setkey(&ctx, MBEDTLS_CIPHER_ID_AES,
			kek, kek_len * 8, 1);
	if (ret != 0) {
		return ret;
	}

	ret = mbedtls_nist_kw_wrap(&ctx, MBEDTLS_KW_MODE_KW, plain,
			n * 8, cipher, &olen, (n + 1) * 8);

	mbedtls_nist_kw_free(&ctx);
	return ret;
}

int aes_unwrap(const u8 *kek, size_t kek_len, int n, const u8 *cipher,
	       u8 *plain)
{
	mbedtls_nist_kw_context ctx;
	size_t olen;
	int ret = 0;
	mbedtls_nist_kw_init(&ctx);

	ret = mbedtls_nist_kw_setkey(&ctx, MBEDTLS_CIPHER_ID_AES,
			kek, kek_len * 8, 0);
	if (ret != 0) {
		return ret;
	}

	ret = mbedtls_nist_kw_unwrap(&ctx, MBEDTLS_KW_MODE_KW, cipher,
			(n + 1) * 8, plain, &olen, (n * 8));

	mbedtls_nist_kw_free(&ctx);
	return ret;
}
#endif

int crypto_mod_exp(const uint8_t *base, size_t base_len,
		   const uint8_t *power, size_t power_len,
		   const uint8_t *modulus, size_t modulus_len,
		   uint8_t *result, size_t *result_len)
{
	mbedtls_mpi bn_base, bn_exp, bn_modulus, bn_result, bn_rinv;
	int ret = 0;

	mbedtls_mpi_init(&bn_base);
	mbedtls_mpi_init(&bn_exp);
	mbedtls_mpi_init(&bn_modulus);
	mbedtls_mpi_init(&bn_result);
	mbedtls_mpi_init(&bn_rinv);

	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&bn_base, base, base_len));
	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&bn_exp, power, power_len));
	MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&bn_modulus, modulus, modulus_len));

	MBEDTLS_MPI_CHK(mbedtls_mpi_exp_mod(&bn_result, &bn_base, &bn_exp, &bn_modulus,
					    &bn_rinv));

	ret = mbedtls_mpi_write_binary(&bn_result, result, *result_len);

cleanup:
	mbedtls_mpi_free(&bn_base);
	mbedtls_mpi_free(&bn_exp);
	mbedtls_mpi_free(&bn_modulus);
	mbedtls_mpi_free(&bn_result);
	mbedtls_mpi_free(&bn_rinv);

	return ret;
}

int pbkdf2_sha1(const char *passphrase, const u8 *ssid, size_t ssid_len,
		int iterations, u8 *buf, size_t buflen)
{
#ifdef CONFIG_FAST_PBKDF2
	fastpbkdf2_hmac_sha1((const u8 *) passphrase, os_strlen(passphrase),
			     ssid, ssid_len, iterations, buf, buflen);
	return 0;
#else
	int ret = mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA1, (const u8 *) passphrase,
					os_strlen(passphrase) , ssid,
					ssid_len, iterations, buflen, buf);
	if (ret != 0) {
		ret = -1;
		goto cleanup;
	}

cleanup:
	return ret;
#endif
}

#ifdef MBEDTLS_DES_C
int des_encrypt(const u8 *clear, const u8 *key, u8 *cypher)
{
	int ret;
	mbedtls_des_context des;
	u8 pkey[8], next, tmp;
	int i;

	/* Add parity bits to the key */
	next = 0;
	for (i = 0; i < 7; i++) {
		tmp = key[i];
		pkey[i] = (tmp >> i) | next | 1;
		next = tmp << (7 - i);
	}
	pkey[i] = next | 1;

	mbedtls_des_init(&des);
	ret = mbedtls_des_setkey_enc(&des, pkey);
	if (ret < 0) {
		return ret;
	}
	ret = mbedtls_des_crypt_ecb(&des, clear, cypher);
	mbedtls_des_free(&des);

	return ret;
}
#endif

/* Only enable this if all other ciphers are using MbedTLS implementation */
#if defined(MBEDTLS_CCM_C) && defined(MBEDTLS_CMAC_C) && defined(MBEDTLS_NIST_KW_C)
int aes_ccm_ae(const u8 *key, size_t key_len, const u8 *nonce,
	       size_t M, const u8 *plain, size_t plain_len,
	       const u8 *aad, size_t aad_len, u8 *crypt, u8 *auth)
{
	int ret;
	mbedtls_ccm_context ccm;

	mbedtls_ccm_init(&ccm);

	ret = mbedtls_ccm_setkey(&ccm, MBEDTLS_CIPHER_ID_AES,
				 key, key_len * 8);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "mbedtls_ccm_setkey failed");
		goto cleanup;
	}

	ret = mbedtls_ccm_encrypt_and_tag(&ccm, plain_len, nonce, 13, aad,
					  aad_len, plain, crypt, auth, M);

cleanup:
	mbedtls_ccm_free(&ccm);

	return ret;
}

int aes_ccm_ad(const u8 *key, size_t key_len, const u8 *nonce,
	       size_t M, const u8 *crypt, size_t crypt_len,
	       const u8 *aad, size_t aad_len, const u8 *auth,
	       u8 *plain)
{
	int ret;
	mbedtls_ccm_context ccm;

	mbedtls_ccm_init(&ccm);

	ret = mbedtls_ccm_setkey(&ccm, MBEDTLS_CIPHER_ID_AES,
				 key, key_len * 8);
	if (ret < 0) {
		goto cleanup;;
	}

	ret = mbedtls_ccm_star_auth_decrypt(&ccm, crypt_len,
					    nonce, 13, aad, aad_len,
					    crypt, plain, auth, M);

cleanup:
	mbedtls_ccm_free(&ccm);

	return ret;
}
#endif

#ifdef MBEDTLS_CMAC_C
int omac1_aes_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	const mbedtls_cipher_info_t *cipher_info;
	int i, ret = 0;
	mbedtls_cipher_type_t cipher_type;
	mbedtls_cipher_context_t ctx;

	switch (key_len) {
	case 16:
		cipher_type = MBEDTLS_CIPHER_AES_128_ECB;
		break;
	case 24:
		cipher_type = MBEDTLS_CIPHER_AES_192_ECB;
		break;
	case 32:
		cipher_type = MBEDTLS_CIPHER_AES_256_ECB;
		break;
	default:
		cipher_type = MBEDTLS_CIPHER_NONE;
		break;
	}
	cipher_info = mbedtls_cipher_info_from_type(cipher_type);
	if (cipher_info == NULL) {
		/* Failing at this point must be due to a build issue */
		ret = MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
		goto cleanup;
	}

	if (key == NULL ||  mac == NULL) {
		return -1;
	}

	mbedtls_cipher_init(&ctx);

	ret = mbedtls_cipher_setup(&ctx, cipher_info);
	if (ret != 0) {
		goto cleanup;
	}

	ret = mbedtls_cipher_cmac_starts(&ctx, key, key_len * 8);
	if (ret != 0) {
		goto cleanup;
	}

	for (i = 0 ; i < num_elem; i++) {
		ret = mbedtls_cipher_cmac_update(&ctx, addr[i], len[i]);
		if (ret != 0) {
			goto cleanup;
		}
	}

	ret = mbedtls_cipher_cmac_finish(&ctx, mac);
cleanup:
	mbedtls_cipher_free(&ctx);
	return(ret);
}

int omac1_aes_128_vector(const u8 *key, size_t num_elem,
			 const u8 *addr[], const size_t *len, u8 *mac)
{
	return omac1_aes_vector(key, 16, num_elem, addr, len, mac);
}

int omac1_aes_128(const u8 *key, const u8 *data, size_t data_len, u8 *mac)
{
	return omac1_aes_128_vector(key, 1, &data, &data_len, mac);
}
#endif

int crypto_dh_init(u8 generator, const u8 *prime, size_t prime_len, u8 *privkey,
		   u8 *pubkey)
{
	size_t pubkey_len, pad;

	if (os_get_random(privkey, prime_len) < 0) {
		return -1;
	}
	if (os_memcmp(privkey, prime, prime_len) > 0) {
		/* Make sure private value is smaller than prime */
		privkey[0] = 0;
	}

	pubkey_len = prime_len;
	if (crypto_mod_exp(&generator, 1, privkey, prime_len, prime, prime_len,
				pubkey, &pubkey_len) < 0) {
		return -1;
	}
	if (pubkey_len < prime_len) {
		pad = prime_len - pubkey_len;
		os_memmove(pubkey + pad, pubkey, pubkey_len);
		os_memset(pubkey, 0, pad);
	}

	return 0;
}

int crypto_global_init(void)
{
	return 0;
}

void crypto_global_deinit(void)
{
}
