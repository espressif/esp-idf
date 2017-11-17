/*
 * Crypto wrapper for internal crypto implementation
 * Copyright (c) 2006-2011, Jouni Malinen <j@w1.fi>
 *
 * Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "crypto/includes.h"
#include "crypto/common.h"
#include "crypto/crypto.h"
#include "crypto/sha1_i.h"
#include "crypto/md5_i.h"
#include "mbedtls/sha256.h"


#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

struct fast_crypto_hash {
	enum crypto_hash_alg alg;
	union {
		struct MD5Context md5;
		struct SHA1Context sha1;
#ifdef CONFIG_SHA256
                mbedtls_sha256_context sha256;
#endif /* CONFIG_SHA256 */
	} u;
	u8 key[64];
	size_t key_len;
};

struct crypto_hash *  fast_crypto_hash_init(enum crypto_hash_alg alg, const u8 *key,
				      size_t key_len)
{
	struct fast_crypto_hash *ctx;
	u8 k_pad[64];
	u8 tk[32];
	size_t i;

	ctx = (struct fast_crypto_hash *)os_zalloc(sizeof(*ctx));
	if (ctx == NULL)
		return NULL;

	ctx->alg = alg;

	switch (alg) {
	case CRYPTO_HASH_ALG_MD5:
		MD5Init(&ctx->u.md5);
		break;
	case CRYPTO_HASH_ALG_SHA1:
		SHA1Init(&ctx->u.sha1);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_SHA256:
                mbedtls_sha256_init(&ctx->u.sha256);
                mbedtls_sha256_starts(&ctx->u.sha256, 0);
		break;
#endif /* CONFIG_SHA256 */
	case CRYPTO_HASH_ALG_HMAC_MD5:
		if (key_len > sizeof(k_pad)) {
			MD5Init(&ctx->u.md5);
			MD5Update(&ctx->u.md5, key, key_len);
			MD5Final(tk, &ctx->u.md5);
			key = tk;
			key_len = 16;
		}
		os_memcpy(ctx->key, key, key_len);
		ctx->key_len = key_len;

		os_memcpy(k_pad, key, key_len);
		if (key_len < sizeof(k_pad))
			os_memset(k_pad + key_len, 0, sizeof(k_pad) - key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x36;
		MD5Init(&ctx->u.md5);
		MD5Update(&ctx->u.md5, k_pad, sizeof(k_pad));
		break;
	case CRYPTO_HASH_ALG_HMAC_SHA1:
		if (key_len > sizeof(k_pad)) {
			SHA1Init(&ctx->u.sha1);
			SHA1Update(&ctx->u.sha1, key, key_len);
			SHA1Final(tk, &ctx->u.sha1);
			key = tk;
			key_len = 20;
		}
		os_memcpy(ctx->key, key, key_len);
		ctx->key_len = key_len;

		os_memcpy(k_pad, key, key_len);
		if (key_len < sizeof(k_pad))
			os_memset(k_pad + key_len, 0, sizeof(k_pad) - key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x36;
		SHA1Init(&ctx->u.sha1);
		SHA1Update(&ctx->u.sha1, k_pad, sizeof(k_pad));
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_HMAC_SHA256:
		if (key_len > sizeof(k_pad)) {
                    mbedtls_sha256_init(&ctx->u.sha256);
                    mbedtls_sha256_starts(&ctx->u.sha256, 0);
                    mbedtls_sha256_update(&ctx->u.sha256, key, key_len);
                    mbedtls_sha256_finish(&ctx->u.sha256, tk);
                    mbedtls_sha256_free(&ctx->u.sha256);
		    key = tk;
		    key_len = 32;
		}
		os_memcpy(ctx->key, key, key_len);
		ctx->key_len = key_len;

		os_memcpy(k_pad, key, key_len);
		if (key_len < sizeof(k_pad))
			os_memset(k_pad + key_len, 0, sizeof(k_pad) - key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x36;
                mbedtls_sha256_init(&ctx->u.sha256);
                mbedtls_sha256_starts(&ctx->u.sha256, 0);
                mbedtls_sha256_update(&ctx->u.sha256, k_pad, sizeof(k_pad));
		break;
#endif /* CONFIG_SHA256 */
	default:
		os_free(ctx);
		return NULL;
	}

	return (struct crypto_hash *)ctx;
}


void  fast_crypto_hash_update(struct crypto_hash *ctx, const u8 *data, size_t len)
{
        
        struct fast_crypto_hash *fast_ctx;
        fast_ctx = (struct fast_crypto_hash *)ctx;        
 
	if (fast_ctx == NULL)
		return;

	switch (fast_ctx->alg) {
	case CRYPTO_HASH_ALG_MD5:
	case CRYPTO_HASH_ALG_HMAC_MD5:
		MD5Update(&fast_ctx->u.md5, data, len);
		break;
	case CRYPTO_HASH_ALG_SHA1:
	case CRYPTO_HASH_ALG_HMAC_SHA1:
		SHA1Update(&fast_ctx->u.sha1, data, len);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_SHA256:
	case CRYPTO_HASH_ALG_HMAC_SHA256:
                mbedtls_sha256_update(&fast_ctx->u.sha256, data, len);
		break;
#endif /* CONFIG_SHA256 */
	default:
		break;
	}
}


int  fast_crypto_hash_finish(struct crypto_hash *ctx, u8 *mac, size_t *len)
{
        u8 k_pad[64];
	size_t i;
        struct fast_crypto_hash *fast_ctx;

	if (ctx == NULL)
		return -2;

        fast_ctx = (struct fast_crypto_hash *)ctx;

	if (mac == NULL || len == NULL) {
		os_free(fast_ctx);
		return 0;
	}

	switch (fast_ctx->alg) {
	case CRYPTO_HASH_ALG_MD5:
		if (*len < 16) {
			*len = 16;
			os_free(fast_ctx);
			return -1;
		}
		*len = 16;
		MD5Final(mac, &fast_ctx->u.md5);
		break;
	case CRYPTO_HASH_ALG_SHA1:
		if (*len < 20) {
			*len = 20;
			os_free(fast_ctx);
			return -1;
		}
		*len = 20;
		SHA1Final(mac, &fast_ctx->u.sha1);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_SHA256:
		if (*len < 32) {
			*len = 32;
			os_free(fast_ctx);
			return -1;
		}
		*len = 32;
                mbedtls_sha256_finish(&fast_ctx->u.sha256, mac);
                mbedtls_sha256_free(&fast_ctx->u.sha256);
		break;
#endif /* CONFIG_SHA256 */
	case CRYPTO_HASH_ALG_HMAC_MD5:
		if (*len < 16) {
			*len = 16;
			os_free(fast_ctx);
			return -1;
		}
		*len = 16;

		MD5Final(mac, &fast_ctx->u.md5);

		os_memcpy(k_pad, fast_ctx->key, fast_ctx->key_len);
		os_memset(k_pad + fast_ctx->key_len, 0,
			  sizeof(k_pad) - fast_ctx->key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x5c;
		MD5Init(&fast_ctx->u.md5);
		MD5Update(&fast_ctx->u.md5, k_pad, sizeof(k_pad));
		MD5Update(&fast_ctx->u.md5, mac, 16);
		MD5Final(mac, &fast_ctx->u.md5);
		break;
	case CRYPTO_HASH_ALG_HMAC_SHA1:
		if (*len < 20) {
			*len = 20;
			os_free(ctx);
			return -1;
		}
		*len = 20;

		SHA1Final(mac, &fast_ctx->u.sha1);
		os_memcpy(k_pad, fast_ctx->key, fast_ctx->key_len);
		os_memset(k_pad + fast_ctx->key_len, 0,
			  sizeof(k_pad) - fast_ctx->key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x5c;
		SHA1Init(&fast_ctx->u.sha1);
		SHA1Update(&fast_ctx->u.sha1, k_pad, sizeof(k_pad));
		SHA1Update(&fast_ctx->u.sha1, mac, 20);
		SHA1Final(mac, &fast_ctx->u.sha1);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_HMAC_SHA256:
		if (*len < 32) {
			*len = 32;
			os_free(fast_ctx);
			return -1;
		}
		*len = 32;
                mbedtls_sha256_finish(&fast_ctx->u.sha256, mac);
                mbedtls_sha256_free(&fast_ctx->u.sha256);

		os_memcpy(k_pad, fast_ctx->key, fast_ctx->key_len);
		os_memset(k_pad + fast_ctx->key_len, 0,
			  sizeof(k_pad) - fast_ctx->key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x5c;
                mbedtls_sha256_init(&fast_ctx->u.sha256);
                mbedtls_sha256_starts(&fast_ctx->u.sha256, 0);
                mbedtls_sha256_update(&fast_ctx->u.sha256, k_pad, sizeof(k_pad));
                mbedtls_sha256_update(&fast_ctx->u.sha256, mac, 32);
                mbedtls_sha256_finish(&fast_ctx->u.sha256, mac);
                mbedtls_sha256_free(&fast_ctx->u.sha256);
		break;
#endif /* CONFIG_SHA256 */
	default:
		os_free(fast_ctx);
		return -1;
	}

	os_free(fast_ctx);

	return 0;
}
