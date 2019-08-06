/*
 * Crypto wrapper for internal crypto implementation
 * Copyright (c) 2006-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Hardware crypto support Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "sha1_i.h"
#include "md5_i.h"
#ifdef USE_MBEDTLS_CRYPTO
#include "mbedtls/sha256.h"
#endif

struct crypto_hash {
	enum crypto_hash_alg alg;
	union {
		struct MD5Context md5;
		struct SHA1Context sha1;
#ifdef CONFIG_SHA256
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_context sha256;
#else /* USE_MBEDTLS_CRYPTO */
		struct sha256_state sha256;
#endif /* USE_MBEDTLS_CRYPTO */
#endif /* CONFIG_SHA256 */
	} u;
	u8 key[64];
	size_t key_len;
};


struct crypto_hash *  crypto_hash_init(enum crypto_hash_alg alg, const u8 *key,
				      size_t key_len)
{
	struct crypto_hash *ctx;
	u8 k_pad[64];
	u8 tk[32];
	size_t i;

	ctx = (struct crypto_hash *)os_zalloc(sizeof(*ctx));
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
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_init(&ctx->u.sha256);
		mbedtls_sha256_starts(&ctx->u.sha256, 0);
#else /* USE_MBEDTLS_CRYPTO */
		sha256_init(&ctx->u.sha256);
#endif /* USE_MBEDTLS_CRYPTO */
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
#ifdef USE_MBEDTLS_CRYPTO
			mbedtls_sha256_init(&ctx->u.sha256);
			mbedtls_sha256_starts(&ctx->u.sha256, 0);
			mbedtls_sha256_update(&ctx->u.sha256, key, key_len);
			mbedtls_sha256_finish(&ctx->u.sha256, tk);
			mbedtls_sha256_free(&ctx->u.sha256);
#else /* USE_MBEDTLS_CRYPTO */
			sha256_init(&ctx->u.sha256);
			sha256_process(&ctx->u.sha256, key, key_len);
			sha256_done(&ctx->u.sha256, tk);
#endif /* USE_MBEDTLS_CRYPTO */
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
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_init(&ctx->u.sha256);
		mbedtls_sha256_starts(&ctx->u.sha256, 0);
		mbedtls_sha256_update(&ctx->u.sha256, k_pad, sizeof(k_pad));
#else /* USE_MBEDTLS_CRYPTO */
		sha256_init(&ctx->u.sha256);
		sha256_process(&ctx->u.sha256, k_pad, sizeof(k_pad));
#endif /* USE_MBEDTLS_CRYPTO */
		break;
#endif /* CONFIG_SHA256 */
	default:
		os_free(ctx);
		return NULL;
	}

	return ctx;
}


void  crypto_hash_update(struct crypto_hash *ctx, const u8 *data, size_t len)
{
	if (ctx == NULL)
		return;

	switch (ctx->alg) {
	case CRYPTO_HASH_ALG_MD5:
	case CRYPTO_HASH_ALG_HMAC_MD5:
		MD5Update(&ctx->u.md5, data, len);
		break;
	case CRYPTO_HASH_ALG_SHA1:
	case CRYPTO_HASH_ALG_HMAC_SHA1:
		SHA1Update(&ctx->u.sha1, data, len);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_SHA256:
	case CRYPTO_HASH_ALG_HMAC_SHA256:
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_update(&ctx->u.sha256, data, len);
#else /* USE_MBEDTLS_CRYPTO */
		sha256_process(&ctx->u.sha256, data, len);
#endif /* USE_MBEDTLS_CRYPTO */
		break;
#endif /* CONFIG_SHA256 */
	default:
		break;
	}
}


int  crypto_hash_finish(struct crypto_hash *ctx, u8 *mac, size_t *len)
{
	u8 k_pad[64];
	size_t i;

	if (ctx == NULL)
		return -2;

	if (mac == NULL || len == NULL) {
		os_free(ctx);
		return 0;
	}

	switch (ctx->alg) {
	case CRYPTO_HASH_ALG_MD5:
		if (*len < 16) {
			*len = 16;
			os_free(ctx);
			return -1;
		}
		*len = 16;
		MD5Final(mac, &ctx->u.md5);
		break;
	case CRYPTO_HASH_ALG_SHA1:
		if (*len < 20) {
			*len = 20;
			os_free(ctx);
			return -1;
		}
		*len = 20;
		SHA1Final(mac, &ctx->u.sha1);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_SHA256:
		if (*len < 32) {
			*len = 32;
			os_free(ctx);
			return -1;
		}
		*len = 32;
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_finish(&ctx->u.sha256, mac);
		mbedtls_sha256_free(&ctx->u.sha256);
#else /* USE_MBEDTLS_CRYPTO */
		sha256_done(&ctx->u.sha256, mac);
#endif /* USE_MBEDTLS_CRYPTO */
		break;
#endif /* CONFIG_SHA256 */
	case CRYPTO_HASH_ALG_HMAC_MD5:
		if (*len < 16) {
			*len = 16;
			os_free(ctx);
			return -1;
		}
		*len = 16;

		MD5Final(mac, &ctx->u.md5);

		os_memcpy(k_pad, ctx->key, ctx->key_len);
		os_memset(k_pad + ctx->key_len, 0,
			  sizeof(k_pad) - ctx->key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x5c;
		MD5Init(&ctx->u.md5);
		MD5Update(&ctx->u.md5, k_pad, sizeof(k_pad));
		MD5Update(&ctx->u.md5, mac, 16);
		MD5Final(mac, &ctx->u.md5);
		break;
	case CRYPTO_HASH_ALG_HMAC_SHA1:
		if (*len < 20) {
			*len = 20;
			os_free(ctx);
			return -1;
		}
		*len = 20;

		SHA1Final(mac, &ctx->u.sha1);

		os_memcpy(k_pad, ctx->key, ctx->key_len);
		os_memset(k_pad + ctx->key_len, 0,
			  sizeof(k_pad) - ctx->key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x5c;
		SHA1Init(&ctx->u.sha1);
		SHA1Update(&ctx->u.sha1, k_pad, sizeof(k_pad));
		SHA1Update(&ctx->u.sha1, mac, 20);
		SHA1Final(mac, &ctx->u.sha1);
		break;
#ifdef CONFIG_SHA256
	case CRYPTO_HASH_ALG_HMAC_SHA256:
		if (*len < 32) {
			*len = 32;
			os_free(ctx);
			return -1;
		}
		*len = 32;

#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_finish(&ctx->u.sha256, mac);
		mbedtls_sha256_free(&ctx->u.sha256);
#else /* USE_MBEDTLS_CRYPTO */
		sha256_done(&ctx->u.sha256, mac);
#endif /* USE_MBEDTLS_CRYPTO */

		os_memcpy(k_pad, ctx->key, ctx->key_len);
		os_memset(k_pad + ctx->key_len, 0,
			  sizeof(k_pad) - ctx->key_len);
		for (i = 0; i < sizeof(k_pad); i++)
			k_pad[i] ^= 0x5c;
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_sha256_init(&ctx->u.sha256);
		mbedtls_sha256_starts(&ctx->u.sha256, 0);
		mbedtls_sha256_update(&ctx->u.sha256, k_pad, sizeof(k_pad));
		mbedtls_sha256_update(&ctx->u.sha256, mac, 32);
		mbedtls_sha256_finish(&ctx->u.sha256, mac);
		mbedtls_sha256_free(&ctx->u.sha256);
#else /* USE_MBEDTLS_CRYPTO */
		sha256_init(&ctx->u.sha256);
		sha256_process(&ctx->u.sha256, k_pad, sizeof(k_pad));
		sha256_process(&ctx->u.sha256, mac, 32);
		sha256_done(&ctx->u.sha256, mac);
#endif /* USE_MBEDTLS_CRYPTO */
		break;
#endif /* CONFIG_SHA256 */
	default:
		os_free(ctx);
		return -1;
	}

	os_free(ctx);

	return 0;
}


int  crypto_global_init(void)
{
	return 0;
}


void  crypto_global_deinit(void)
{
}
