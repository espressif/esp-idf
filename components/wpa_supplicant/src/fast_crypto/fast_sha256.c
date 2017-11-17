/*
 * SHA-256 hash implementation and interface functions
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

#include "crypto/includes.h"

#include "crypto/common.h"
#include "crypto/sha256.h"
#include "crypto/crypto.h"


/**
 * fast_hmac_sha256_vector - HMAC-SHA256 over data vector (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash (32 bytes)
 */
void 
fast_hmac_sha256_vector(const uint8_t *key, size_t key_len, size_t num_elem,
			const uint8_t *addr[], const size_t *len, uint8_t *mac)
{
    uint8_t k_pad[64]; /* padding - key XORd with ipad/opad */
    uint8_t tk[32];
    const uint8_t *_addr[6];
    size_t _len[6], i;

    if (num_elem > 5) {
        /*
         * Fixed limit on the number of fragments to avoid having to
         * allocate memory (which could fail).
         */
        return;
    }

    /* if key is longer than 64 bytes reset it to key = SHA256(key) */
    if (key_len > 64) {
        fast_sha256_vector(1, &key, &key_len, tk);
        key = tk;
        key_len = 32;
    }

    /* the HMAC_SHA256 transform looks like:
     *
     * SHA256(K XOR opad, SHA256(K XOR ipad, text))
     *
     * where K is an n byte key
     * ipad is the byte 0x36 repeated 64 times
     * opad is the byte 0x5c repeated 64 times
     * and text is the data being protected 
     */

    /* start out by storing key in ipad */
    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);
    /* XOR key with ipad values */
    for (i = 0; i < 64; i++) {
        k_pad[i] ^= 0x36;
    }

    /* perform inner SHA256 */
    _addr[0] = k_pad;
    _len[0] = 64;
    for (i = 0; i < num_elem; i++) {
        _addr[i + 1] = addr[i];
        _len[i + 1] = len[i];
    }
    fast_sha256_vector(1 + num_elem, _addr, _len, mac);

    os_memset(k_pad, 0, sizeof(k_pad));
    os_memcpy(k_pad, key, key_len);
    /* XOR key with opad values */
    for (i = 0; i < 64; i++) {
        k_pad[i] ^= 0x5c;
    }

    /* perform outer SHA256 */
    _addr[0] = k_pad;
    _len[0] = 64;
    _addr[1] = mac;
    _len[1] = SHA256_MAC_LEN;
    fast_sha256_vector(2, _addr, _len, mac);
}


/**
 * fast_hmac_sha256 - HMAC-SHA256 over data buffer (RFC 2104)
 * @key: Key for HMAC operations
 * @key_len: Length of the key in bytes
 * @data: Pointers to the data area
 * @data_len: Length of the data area
 * @mac: Buffer for the hash (20 bytes)
 */
void 
fast_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data,
		 size_t data_len, uint8_t *mac)
{
    fast_hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}


/**
 * fast_sha256_prf - SHA256-based Pseudo-Random Function (IEEE 802.11r, 8.5.1.5.2)
 * @key: Key for PRF
 * @key_len: Length of the key in bytes
 * @label: A unique label for each purpose of the PRF
 * @data: Extra data to bind into the key
 * @data_len: Length of the data
 * @buf: Buffer for the generated pseudo-random key
 * @buf_len: Number of bytes of key to generate
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key.
 */
void 
fast_sha256_prf(const uint8_t *key, size_t key_len, const char *label,
		const uint8_t *data, size_t data_len, uint8_t *buf, size_t buf_len)
{
    uint16_t counter = 1;
    size_t pos, plen;
    uint8_t hash[SHA256_MAC_LEN];
    const uint8_t *addr[4];
    size_t len[4];
    uint8_t counter_le[2], length_le[2];

    addr[0] = counter_le;
    len[0] = 2;
    addr[1] = (uint8_t *) label;
    len[1] = os_strlen(label);
    addr[2] = data;
    len[2] = data_len;
    addr[3] = length_le;
    len[3] = sizeof(length_le);

    WPA_PUT_LE16(length_le, buf_len * 8);
    pos = 0;
    while (pos < buf_len) {
        plen = buf_len - pos;
        WPA_PUT_LE16(counter_le, counter);
        if (plen >= SHA256_MAC_LEN) {
            fast_hmac_sha256_vector(key, key_len, 4, addr, len,
		                    &buf[pos]);
            pos += SHA256_MAC_LEN;
        } else {
            fast_hmac_sha256_vector(key, key_len, 4, addr, len, hash);
            os_memcpy(&buf[pos], hash, plen);
            break;
        }
        counter++;
    }
}
