/*
 * AES-128/192/256 CTR
 *
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#include "utils/includes.h"
#include "utils/common.h"

#include "crypto/aes.h"
#include "crypto/aes_wrap.h"
#ifdef USE_MBEDTLS_CRYPTO
#include "mbedtls/aes.h"
#endif

/**
 * aes_ctr_encrypt - AES-128/192/256 CTR mode encryption
 * @key: Key for encryption (key_len bytes)
 * @key_len: Length of the key (16, 24, or 32 bytes)
 * @nonce: Nonce for counter mode (16 bytes)
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes
 * Returns: 0 on success, Negative value on failure
 */
int aes_ctr_encrypt(const u8 *key, size_t key_len, const u8 *nonce,
		    u8 *data, size_t data_len)
{
#ifdef USE_MBEDTLS_CRYPTO
	int ret;
	mbedtls_aes_context ctx;
	uint8_t stream_block[AES_BLOCK_SIZE];
	size_t offset = 0;

	mbedtls_aes_init(&ctx);
	ret = mbedtls_aes_setkey_enc(&ctx, key, key_len * 8);
	if (ret < 0)
		goto cleanup;
	ret = mbedtls_aes_crypt_ctr(&ctx, data_len, &offset, (u8 *)nonce, stream_block, data, data);
cleanup:
	mbedtls_aes_free(&ctx);
	return ret;
#else
	void *ctx;
	size_t j, len, left = data_len;
	int i;
	u8 *pos = data;
	u8 counter[AES_BLOCK_SIZE], buf[AES_BLOCK_SIZE];

	ctx = aes_encrypt_init(key, key_len);
	if (ctx == NULL)
		return -1;
	os_memcpy(counter, nonce, AES_BLOCK_SIZE);

	while (left > 0) {
		aes_encrypt(ctx, counter, buf);

		len = (left < AES_BLOCK_SIZE) ? left : AES_BLOCK_SIZE;
		for (j = 0; j < len; j++)
			pos[j] ^= buf[j];
		pos += len;
		left -= len;

		for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
			counter[i]++;
			if (counter[i])
				break;
		}
	}
	aes_encrypt_deinit(ctx);
	return 0;
#endif
}


/**
 * aes_128_ctr_encrypt - AES-128 CTR mode encryption
 * @key: Key for encryption (key_len bytes)
 * @nonce: Nonce for counter mode (16 bytes)
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes
 * Returns: 0 on success, -1 on failure
 */
int aes_128_ctr_encrypt(const u8 *key, const u8 *nonce,
			u8 *data, size_t data_len)
{
	return aes_ctr_encrypt(key, 16, nonce, data, data_len);
}
