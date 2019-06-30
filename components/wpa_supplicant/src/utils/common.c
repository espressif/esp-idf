/*
 * wpa_supplicant/hostapd / common helper functions, etc.
 * Copyright (c) 2002-2007, Jouni Malinen <j@w1.fi>
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

#include "utils/includes.h"

#include "utils/common.h"

/**
 * inc_byte_array - Increment arbitrary length byte array by one
 * @counter: Pointer to byte array
 * @len: Length of the counter in bytes
 *
 * This function increments the last byte of the counter by one and continues
 * rolling over to more significant bytes if the byte was incremented from
 * 0xff to 0x00.
 */
void inc_byte_array(u8 *counter, size_t len)
{
	int pos = len - 1;
	while (pos >= 0) {
		counter[pos]++;
		if (counter[pos] != 0)
			break;
		pos--;
	}
}

static int hex2num(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}


int hex2byte(const char *hex)
{
	int a, b;
	a = hex2num(*hex++);
	if (a < 0)
		return -1;
	b = hex2num(*hex++);
	if (b < 0)
		return -1;
	return (a << 4) | b;
}


/**
 * hexstr2bin - Convert ASCII hex string into binary data
 * @hex: ASCII hex string (e.g., "01ab")
 * @buf: Buffer for the binary data
 * @len: Length of the text to convert in bytes (of buf); hex will be double
 * this size
 * Returns: 0 on success, -1 on failure (invalid hex string)
 */
int hexstr2bin(const char *hex, u8 *buf, size_t len)
{
	size_t i;
	int a;
	const char *ipos = hex;
	u8 *opos = buf;

	for (i = 0; i < len; i++) {
		a = hex2byte(ipos);
		if (a < 0)
			return -1;
		*opos++ = a;
		ipos += 2;
	}
	return 0;
}

void wpa_get_ntp_timestamp(u8 *buf)
{
	struct os_time now;
	u32 sec, usec;
	be32 tmp;

	/* 64-bit NTP timestamp (time from 1900-01-01 00:00:00) */
	os_get_time(&now);
	sec = now.sec + 2208988800U; /* Epoch to 1900 */
	/* Estimate 2^32/10^6 = 4295 - 1/32 - 1/512 */
	usec = now.usec;
	usec = 4295 * usec - (usec >> 5) - (usec >> 9);
	tmp = host_to_be32(sec);
	memcpy(buf, (u8 *) &tmp, 4);
	tmp = host_to_be32(usec);
	memcpy(buf + 4, (u8 *) &tmp, 4);
}

char * wpa_config_parse_string(const char *value, size_t *len)
{
	if (*value == '"' && (strlen(value) == 7 || strlen(value) == 15)) {
		const char *pos;
		char *str;
		value++;
		pos = (char *)strrchr(value, '"');
		if (pos == NULL)
			return NULL;
		*len = pos - value;
		str = (char *)os_malloc(*len + 1);
		if (str == NULL)
			return NULL;
		memcpy(str, value, *len);
		str[*len] = '\0';
		return str;
	} else {
		u8 *str;
		size_t tlen, hlen = strlen(value);
		if (hlen == 5 || hlen == 13) {
			*len = hlen;
			str = (u8 *)os_malloc(*len + 1);
			if (str == NULL) {
				return NULL;
			}
			memcpy(str, value, *len);
			str[*len] = '\0';
		} else if (hlen == 10 || hlen == 26) {
			tlen = hlen / 2;
			str = (u8 *)os_malloc(tlen + 1);
			if (str == NULL)
				return NULL;
			if (hexstr2bin(value, str, tlen)) {
				os_free(str);
				return NULL;
			}
			str[tlen] = '\0';
			*len = tlen;
		} else {
			return NULL;
		}
		return (char *) str;
	}
}

char * dup_binstr(const void *src, size_t len)
{
        char *res;

        if (src == NULL)
                return NULL;
        res = os_malloc(len + 1);
        if (res == NULL)
                return NULL;
        memcpy(res, src, len);
        res[len] = '\0';

        return res;
}

