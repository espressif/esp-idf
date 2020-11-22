/*
 * AES functions
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef AES_H
#define AES_H

#define AES_BLOCK_SIZE 16

void * aes_encrypt_init(const u8 *key, size_t len);
void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);
void * aes_decrypt_init(const u8 *key, size_t len);
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);

int omac1_aes_128(const u8 *key, const u8 *data, size_t data_len, u8 *mac);

int aes_ccm_ae(const u8 *key, size_t key_len, const u8 *nonce,
               size_t M, const u8 *plain, size_t plain_len,
               const u8 *aad, size_t aad_len, u8 *crypt, u8 *auth);
int aes_ccm_ad(const u8 *key, size_t key_len, u8 *nonce,
               size_t M, const u8 *crypt, size_t crypt_len,
               const u8 *aad, size_t aad_len, const u8 *auth,
               u8 *plain, bool skip_auth);
#endif /* AES_H */
