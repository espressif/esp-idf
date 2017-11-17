/*
 * SHA256 hash implementation and interface functions
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

#ifndef SHA256_H
#define SHA256_H

#define SHA256_MAC_LEN 32

void hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
		      const u8 *addr[], const size_t *len, u8 *mac);
void hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
		 size_t data_len, u8 *mac);
void sha256_prf(const u8 *key, size_t key_len, const char *label,
	      const u8 *data, size_t data_len, u8 *buf, size_t buf_len);

void fast_hmac_sha256_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		             const uint8_t *addr[], const size_t *len, uint8_t *mac);
void fast_hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data,
		      size_t data_len, uint8_t *mac);
void fast_sha256_prf(const uint8_t *key, size_t key_len, const char *label,
	             const uint8_t *data, size_t data_len, uint8_t *buf, size_t buf_len);
#endif /* SHA256_H */
