/*
 * Crypto wrapper for internal crypto implementation - Cipher wrappers
 * Copyright (c) 2006-2009, Jouni Malinen <j@w1.fi>
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

#include "utils/common.h"
#include "utils/includes.h"
#include "crypto.h"
#include "aes.h"
#if defined(CONFIG_DES) || defined(CONFIG_DES3)
#include "des_i.h"
#endif
#ifdef USE_MBEDTLS_CRYPTO
#include "mbedtls/aes.h"
#endif /* USE_MBEDTLS_CRYPTO */

struct crypto_cipher {
	enum crypto_cipher_alg alg;
	union {
		struct {
			size_t used_bytes;
			u8 key[16];
			size_t keylen;
		} rc4;
		struct {
			u8 cbc[32];
#ifdef USE_MBEDTLS_CRYPTO
			mbedtls_aes_context ctx_enc;
			mbedtls_aes_context ctx_dec;
#else /* USE_MBEDTLS_CRYPTO */
			void *ctx_enc;
			void *ctx_dec;
#endif /* USE_MBEDTLS_CRYPTO */
		} aes;
#ifdef CONFIG_DES3
		struct {
			struct des3_key_s key;
			u8 cbc[8];
		} des3;
#endif
#ifdef CONFIG_DES
		struct {
			u32 ek[32];
			u32 dk[32];
			u8 cbc[8];
		} des;
#endif
	} u;
};


struct crypto_cipher *  crypto_cipher_init(enum crypto_cipher_alg alg,
					  const u8 *iv, const u8 *key,
					  size_t key_len)
{
	struct crypto_cipher *ctx;

	ctx = (struct crypto_cipher *)os_zalloc(sizeof(*ctx));
	if (ctx == NULL)
		return NULL;

	ctx->alg = alg;

	switch (alg) {
	case CRYPTO_CIPHER_ALG_RC4:
		if (key_len > sizeof(ctx->u.rc4.key)) {
			os_free(ctx);
			return NULL;
		}
		ctx->u.rc4.keylen = key_len;
		os_memcpy(ctx->u.rc4.key, key, key_len);
		break;
	case CRYPTO_CIPHER_ALG_AES:
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_aes_init(&(ctx->u.aes.ctx_enc));
		mbedtls_aes_setkey_enc(&(ctx->u.aes.ctx_enc), key, key_len * 8);
		mbedtls_aes_init(&(ctx->u.aes.ctx_dec));
		mbedtls_aes_setkey_dec(&(ctx->u.aes.ctx_dec), key, key_len * 8);
#else /* USE_MBEDTLS_CRYPTO */
		ctx->u.aes.ctx_enc = aes_encrypt_init(key, key_len);
		if (ctx->u.aes.ctx_enc == NULL) {
			os_free(ctx);
			return NULL;
		}
		ctx->u.aes.ctx_dec = aes_decrypt_init(key, key_len);
		if (ctx->u.aes.ctx_dec == NULL) {
			aes_encrypt_deinit(ctx->u.aes.ctx_enc);
			os_free(ctx);
			return NULL;
		}
#endif /* USE_MBEDTLS_CRYPTO */
		os_memcpy(ctx->u.aes.cbc, iv, AES_BLOCK_SIZE);
		break;
#ifdef CONFIG_DES3
	case CRYPTO_CIPHER_ALG_3DES:
		if (key_len != 24) {
			os_free(ctx);
			return NULL;
		}
		des3_key_setup(key, &ctx->u.des3.key);
		os_memcpy(ctx->u.des3.cbc, iv, 8);
		break;
#endif
#ifdef CONFIG_DES
	case CRYPTO_CIPHER_ALG_DES:
		if (key_len != 8) {
			os_free(ctx);
			return NULL;
		}
		des_key_setup(key, ctx->u.des.ek, ctx->u.des.dk);
		os_memcpy(ctx->u.des.cbc, iv, 8);
		break;
#endif
	default:
		os_free(ctx);
		return NULL;
	}

	return ctx;
}


int  crypto_cipher_encrypt(struct crypto_cipher *ctx, const u8 *plain,
			  u8 *crypt, size_t len)
{
	size_t i, j, blocks;

	switch (ctx->alg) {
	case CRYPTO_CIPHER_ALG_RC4:
		if (plain != crypt)
			os_memcpy(crypt, plain, len);
		rc4_skip(ctx->u.rc4.key, ctx->u.rc4.keylen,
			 ctx->u.rc4.used_bytes, crypt, len);
		ctx->u.rc4.used_bytes += len;
		break;
	case CRYPTO_CIPHER_ALG_AES:
		if (len % AES_BLOCK_SIZE)
			return -1;
		blocks = len / AES_BLOCK_SIZE;
		for (i = 0; i < blocks; i++) {
			for (j = 0; j < AES_BLOCK_SIZE; j++)
				ctx->u.aes.cbc[j] ^= plain[j];
#ifdef USE_MBEDTLS_CRYPTO
			if (mbedtls_internal_aes_encrypt(&(ctx->u.aes.ctx_enc),
					ctx->u.aes.cbc, ctx->u.aes.cbc) != 0)
				return -1;
#else /* USE_MBEDTLS_CRYPTO */
			aes_encrypt(ctx->u.aes.ctx_enc, ctx->u.aes.cbc,
				    ctx->u.aes.cbc);
#endif /* USE_MBEDTLS_CRYPTO */
			os_memcpy(crypt, ctx->u.aes.cbc, AES_BLOCK_SIZE);
			plain += AES_BLOCK_SIZE;
			crypt += AES_BLOCK_SIZE;
		}
		break;
#ifdef CONFIG_DES3
	case CRYPTO_CIPHER_ALG_3DES:
		if (len % 8)
			return -1;
		blocks = len / 8;
		for (i = 0; i < blocks; i++) {
			for (j = 0; j < 8; j++)
				ctx->u.des3.cbc[j] ^= plain[j];
			des3_encrypt(ctx->u.des3.cbc, &ctx->u.des3.key,
				     ctx->u.des3.cbc);
			os_memcpy(crypt, ctx->u.des3.cbc, 8);
			plain += 8;
			crypt += 8;
		}
		break;
#endif
#ifdef CONFIG_DES
	case CRYPTO_CIPHER_ALG_DES:
		if (len % 8)
			return -1;
		blocks = len / 8;
		for (i = 0; i < blocks; i++) {
			for (j = 0; j < 8; j++)
				ctx->u.des3.cbc[j] ^= plain[j];
			des_block_encrypt(ctx->u.des.cbc, ctx->u.des.ek,
					  ctx->u.des.cbc);
			os_memcpy(crypt, ctx->u.des.cbc, 8);
			plain += 8;
			crypt += 8;
		}
		break;
#endif
	default:
		return -1;
	}

	return 0;
}


int  crypto_cipher_decrypt(struct crypto_cipher *ctx, const u8 *crypt,
			  u8 *plain, size_t len)
{
	size_t i, j, blocks;
	u8 tmp[32];

	switch (ctx->alg) {
	case CRYPTO_CIPHER_ALG_RC4:
		if (plain != crypt)
			os_memcpy(plain, crypt, len);
		rc4_skip(ctx->u.rc4.key, ctx->u.rc4.keylen,
			 ctx->u.rc4.used_bytes, plain, len);
		ctx->u.rc4.used_bytes += len;
		break;
	case CRYPTO_CIPHER_ALG_AES:
		if (len % AES_BLOCK_SIZE)
			return -1;
		blocks = len / AES_BLOCK_SIZE;
		for (i = 0; i < blocks; i++) {
			os_memcpy(tmp, crypt, AES_BLOCK_SIZE);
#ifdef USE_MBEDTLS_CRYPTO
			if (mbedtls_internal_aes_decrypt(&(ctx->u.aes.ctx_dec),
							 crypt, plain) != 0)
				return -1;
#else /* USE_MBEDTLS_CRYPTO */
			aes_decrypt(ctx->u.aes.ctx_dec, crypt, plain);
#endif /* USE_MBEDTLS_CRYPTO */
			for (j = 0; j < AES_BLOCK_SIZE; j++)
				plain[j] ^= ctx->u.aes.cbc[j];
			os_memcpy(ctx->u.aes.cbc, tmp, AES_BLOCK_SIZE);
			plain += AES_BLOCK_SIZE;
			crypt += AES_BLOCK_SIZE;
		}
		break;
#ifdef CONFIG_DES3
	case CRYPTO_CIPHER_ALG_3DES:
		if (len % 8)
			return -1;
		blocks = len / 8;
		for (i = 0; i < blocks; i++) {
			os_memcpy(tmp, crypt, 8);
			des3_decrypt(crypt, &ctx->u.des3.key, plain);
			for (j = 0; j < 8; j++)
				plain[j] ^= ctx->u.des3.cbc[j];
			os_memcpy(ctx->u.des3.cbc, tmp, 8);
			plain += 8;
			crypt += 8;
		}
		break;
#endif
#ifdef CONFIG_DES
	case CRYPTO_CIPHER_ALG_DES:
		if (len % 8)
			return -1;
		blocks = len / 8;
		for (i = 0; i < blocks; i++) {
			os_memcpy(tmp, crypt, 8);
			des_block_decrypt(crypt, ctx->u.des.dk, plain);
			for (j = 0; j < 8; j++)
				plain[j] ^= ctx->u.des.cbc[j];
			os_memcpy(ctx->u.des.cbc, tmp, 8);
			plain += 8;
			crypt += 8;
		}
		break;
#endif
	default:
		return -1;
	}

	return 0;
}


void  crypto_cipher_deinit(struct crypto_cipher *ctx)
{
	switch (ctx->alg) {
	case CRYPTO_CIPHER_ALG_AES:
#ifdef USE_MBEDTLS_CRYPTO
		mbedtls_aes_free(&(ctx->u.aes.ctx_enc));
		mbedtls_aes_free(&(ctx->u.aes.ctx_dec));
#else /* USE_MBEDTLS_CRYPTO */
		aes_encrypt_deinit(ctx->u.aes.ctx_enc);
		aes_decrypt_deinit(ctx->u.aes.ctx_dec);
#endif /* USE_MBEDTLS_CRYPTO */
		break;
#ifdef CONFIG_DES3
	case CRYPTO_CIPHER_ALG_3DES:
		break;
#endif
	default:
		break;
	}
	os_free(ctx);
}
