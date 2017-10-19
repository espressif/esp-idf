/*
 * MD5 hash implementation and interface functions
 * Copyright (c) 2003-2009, Jouni Malinen <j@w1.fi>
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

#ifndef MD5_H
#define MD5_H

#define MD5_MAC_LEN 16

int hmac_md5_vector(const uint8_t *key, size_t key_len, size_t num_elem,
		    const uint8_t *addr[], const size_t *len, uint8_t *mac);
int hmac_md5(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len,
	     uint8_t *mac);
#ifdef CONFIG_FIPS
int hmac_md5_vector_non_fips_allow(const uint8_t *key, size_t key_len,
				   size_t num_elem, const uint8_t *addr[],
				   const size_t *len, uint8_t *mac);
int hmac_md5_non_fips_allow(const uint8_t *key, size_t key_len, const uint8_t *data,
			    size_t data_len, uint8_t *mac);
#else /* CONFIG_FIPS */
#define hmac_md5_vector_non_fips_allow hmac_md5_vector
#define hmac_md5_non_fips_allow hmac_md5
#endif /* CONFIG_FIPS */

#endif /* MD5_H */
