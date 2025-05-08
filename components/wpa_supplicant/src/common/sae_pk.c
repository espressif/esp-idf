/*
 * SAE-PK
 * Copyright (c) 2020, The Linux Foundation
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifdef CONFIG_SAE_PK

#include "utils/includes.h"
#include "utils/common.h"
#include "common/ieee802_11_defs.h"
#include "common/wpa_supplicant_i.h"
#include "common/ieee802_11_common.h"
#include "esp_common_i.h"
#include "sae.h"
#include "utils/base64.h"
#include "crypto/crypto.h"
#include "crypto/aes.h"
#include "crypto/aes_siv.h"


extern struct wpa_supplicant g_wpa_supp;

/* RFC 4648 base 32 alphabet with lowercase characters */
static const char *sae_pk_base32_table = "abcdefghijklmnopqrstuvwxyz234567";


static const u8 d_mult_table[] = {
	0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  0,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 16,
	2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,
	18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17,
	3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,
	19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18,
	4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19,
	5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20,
	6,  7,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21,
	7,  8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,
	23, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22,
	8,  9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7,
	24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23,
	9, 10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  8,
	25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24,
	10, 11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
	26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	11, 12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
	27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	12, 13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
	28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
	13, 14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
	29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
	14, 15,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13,
	30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
	15,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	31, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
	16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17,
	0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,
	17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18,
	1,  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,
	18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19,
	2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,
	19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,
	3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,
	20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21,
	4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,
	21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22,
	5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,
	22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23,
	6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8,  7,
	23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24,
	7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,  8,
	24, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25,
	8,  7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,  9,
	25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26,
	9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11, 10,
	26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27,
	10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12, 11,
	27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28,
	11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13, 12,
	28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29,
	12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 15, 14, 13,
	29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30,
	13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 15, 14,
	30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 31,
	14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 15,
	31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
	15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0
};


static const u8 d_perm_table[] = {
	7,  2,  1, 30, 16, 20, 27, 11, 31,  6,  8, 13, 29,  5, 10, 21,
	22,  3, 24,  0, 23, 25, 12,  9, 28, 14,  4, 15, 17, 18, 19, 26
};


static u8 d_permute(u8 val, unsigned int iter)
{
	if (iter == 0) {
		return val;
	}
	return d_permute(d_perm_table[val], iter - 1);
}


static u8 d_invert(u8 val)
{
	if (val > 0 && val < 16) {
		return 16 - val;
	}
	return val;
}


static char d_check_char(const char *str, size_t len)
{
	size_t i;
	u8 val = 0;
	u8 dtable[256];
	unsigned int iter = 1;
	int j;

	os_memset(dtable, 0x80, 256);
	for (i = 0; sae_pk_base32_table[i]; i++) {
		dtable[(u8) sae_pk_base32_table[i]] = i;
	}

	for (j = len - 1; j >= 0; j--) {
		u8 c, p;

		c = dtable[(u8) str[j]];
		if (c == 0x80) {
			continue;
		}
		p = d_permute(c, iter);
		iter++;
		val = d_mult_table[val * 32 + p];
	}

	return sae_pk_base32_table[d_invert(val)];
}


bool sae_pk_valid_password(const char *pw)
{
	int pos;
	size_t i, pw_len = os_strlen(pw);
	u8 sec_1b;
	u8 dtable[256];

	os_memset(dtable, 0x80, 256);
	for (i = 0; sae_pk_base32_table[i]; i++) {
		dtable[(u8) sae_pk_base32_table[i]] = i;
	}

	/* SAE-PK password has at least three four character components
	 * separated by hyphens. */
	if (pw_len < 14 || pw_len % 5 != 4) {
		wpa_printf(MSG_DEBUG, "SAE-PK: Not a valid password (length)");
		return false;
	}

	for (pos = 0; pw[pos]; pos++) {
		if (pos && pos % 5 == 4) {
			if (pw[pos] != '-') {
				wpa_printf(MSG_DEBUG,
						   "SAE-PK: Not a valid password (separator)");
				return false;
			}
			continue;
		}
		if (dtable[(u8) pw[pos]] == 0x80) {
			wpa_printf(MSG_DEBUG,
					   "SAE-PK: Not a valid password (character)");
			return false;
		}
	}

	/* Verify that the checksum character is valid */
	if (pw[pw_len - 1] != d_check_char(pw, pw_len - 1)) {
		wpa_printf(MSG_DEBUG,
				   "SAE-PK: Not a valid password (checksum)");
		return false;
	}

	/* Verify that Sec_1b bits match */
	sec_1b = dtable[(u8) pw[0]] & BIT(4);
	for (i = 5; i < pw_len; i += 5) {
		if (sec_1b != (dtable[(u8) pw[i]] & BIT(4))) {
			wpa_printf(MSG_DEBUG,
					   "SAE-PK: Not a valid password (Sec_1b)");
			return false;
		}
	}
	return true;
}


static char *add_char(const char *start, char *pos, u8 idx, size_t *bits)
{
	if (*bits == 0) {
		return pos;
	}
	if (*bits > 5) {
		*bits -= 5;
	} else {
		*bits = 0;
	}

	if ((pos - start) % 5 == 4) {
		*pos++ = '-';
	}
	*pos++ = sae_pk_base32_table[idx];
	return pos;
}


/* Base32 encode a password and add hyper separators and checksum */
char *sae_pk_base32_encode(const u8 *src, size_t len_bits)
{
	char *out, *pos;
	size_t olen, extra_pad, i;
	u64 block = 0;
	u8 val;
	size_t len = (len_bits + 7) / 8;
	size_t left = len_bits;
	int j;

	if (len == 0 || len >= SIZE_MAX / 8) {
		return NULL;
	}
	olen = len * 8 / 5 + 1;
	olen += olen / 4; /* hyphen separators */
	pos = out = os_zalloc(olen + 2); /* include room for ChkSum and nul */
	if (!out) {
		return NULL;
	}

	extra_pad = (5 - len % 5) % 5;
	for (i = 0; i < len + extra_pad; i++) {
		val = i < len ? src[i] : 0;
		block <<= 8;
		block |= val;
		if (i % 5 == 4) {
			for (j = 7; j >= 0; j--)
				pos = add_char(out, pos,
							   (block >> j * 5) & 0x1f, &left);
			block = 0;
		}
	}

	*pos = d_check_char(out, os_strlen(out));

	return out;
}


u8 *sae_pk_base32_decode(const char *src, size_t len, size_t *out_len)
{
	u8 dtable[256], *out, *pos, tmp;
	u64 block = 0;
	size_t i, count, olen;
	int pad = 0;
	size_t extra_pad;

	os_memset(dtable, 0x80, 256);
	for (i = 0; sae_pk_base32_table[i]; i++) {
		dtable[(u8) sae_pk_base32_table[i]] = i;
	}
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[(u8) src[i]] != 0x80) {
			count++;
		}
	}

	if (count == 0) {
		return NULL;
	}
	extra_pad = (8 - count % 8) % 8;

	olen = (count + extra_pad) / 8 * 5;
	pos = out = os_malloc(olen);
	if (!out) {
		return NULL;
	}

	count = 0;
	for (i = 0; i < len + extra_pad; i++) {
		u8 val;

		if (i >= len) {
			val = '=';
		} else {
			val = src[i];
		}
		tmp = dtable[val];
		if (tmp == 0x80) {
			continue;
		}

		if (val == '=') {
			pad++;
		}
		block <<= 5;
		block |= tmp;
		count++;
		if (count == 8) {
			*pos++ = (block >> 32) & 0xff;
			*pos++ = (block >> 24) & 0xff;
			*pos++ = (block >> 16) & 0xff;
			*pos++ = (block >> 8) & 0xff;
			*pos++ = block & 0xff;
			count = 0;
			block = 0;
			if (pad) {
				/* Leave in all the available bits with zero
				 * padding to full octets from right. */
				pos -= pad * 5 / 8;
				break;
			}
		}
	}

	*out_len = pos - out;
	return out;
}


u32 sae_pk_get_be19(const u8 *buf)
{
	return (buf[0] << 11) | (buf[1] << 3) | (buf[2] >> 5);
}


/* shift left by two octets and three bits; fill in zeros from right;
 * len must be at least three */
void sae_pk_buf_shift_left_19(u8 *buf, size_t len)
{
	u8 *dst, *src, *end;

	dst = buf;
	src = buf + 2;
	end = buf + len;

	while (src + 1 < end) {
		*dst++ = (src[0] << 3) | (src[1] >> 5);
		src++;
	}
	*dst++ = *src << 3;
	*dst++ = 0;
	*dst++ = 0;
}


static void sae_pk_buf_shift_left_1(u8 *buf, size_t len)
{
	u8 *dst, *src, *end;

	dst = buf;
	src = buf;
	end = buf + len;

	while (src + 1 < end) {
		*dst++ = (src[0] << 1) | (src[1] >> 7);
		src++;
	}
	*dst++ = *src << 1;
}


int sae_pk_set_password(struct sae_data *sae, const char *password)
{
	struct sae_temporary_data *tmp = sae->tmp;
	size_t len, pw_len;
	u8 *pw, *pos;
	int bits;
	u32 val = 0, val19;
	unsigned int val_bits = 0;

	if (!tmp) {
		return -1;
	}

	os_memset(tmp->fingerprint, 0, sizeof(tmp->fingerprint));
	tmp->fingerprint_bytes = tmp->fingerprint_bits = 0;

	len = os_strlen(password);
	if (len < 1 || !sae_pk_valid_password(password)) {
		return -1;
	}

	pw = sae_pk_base32_decode(password, len, &pw_len);
	if (!pw) {
		return -1;
	}

	tmp->sec = (pw[0] & BIT(7)) ? 3 : 5;
	tmp->lambda = len - len / 5;
	tmp->fingerprint_bits = 8 * tmp->sec + 19 * tmp->lambda / 4 - 5;
	wpa_printf(MSG_DEBUG, "SAE-PK: Sec=%u Lambda=%zu fingerprint_bits=%zu",
			   tmp->sec, tmp->lambda, tmp->fingerprint_bits);

	/* Construct Fingerprint from PasswordBase by prefixing with Sec zero
	 * octets and skipping the Sec_1b bits */
	pos = &tmp->fingerprint[tmp->sec];
	bits = tmp->fingerprint_bits - 8 * tmp->sec;
	wpa_hexdump_key(MSG_DEBUG, "SAE-PK: PasswordBase", pw, pw_len);
	while (bits > 0) {
		if (val_bits < 8) {
			sae_pk_buf_shift_left_1(pw, pw_len); /* Sec_1b */
			val19 = sae_pk_get_be19(pw);
			sae_pk_buf_shift_left_19(pw, pw_len);
			val = (val << 19) | val19;
			val_bits += 19;
		}
		if (val_bits >= 8) {
			if (bits < 8) {
				break;
			}
			*pos++ = (val >> (val_bits - 8)) & 0xff;
			val_bits -= 8;
			bits -= 8;
		}
	}
	if (bits > 0) {
		val >>= val_bits - bits;
		*pos++ = val << (8 - bits);
	}
	tmp->fingerprint_bytes = pos - tmp->fingerprint;
	wpa_hexdump_key(MSG_DEBUG, "SAE-PK: Fingerprint",
					tmp->fingerprint, tmp->fingerprint_bytes);
	bin_clear_free(pw, pw_len);
	return 0;
}


static size_t sae_group_2_hash_len(int group)
{
	switch (group) {
	case 19:
		return 32;
	case 20:
		return 48;
	case 21:
		return 64;
	}

	return 0;
}


int sae_hash(size_t hash_len, const u8 *data, size_t len, u8 *hash)
{
	if (hash_len == 32) {
		return sha256_vector(1, &data, &len, hash);
	}
#ifdef CONFIG_SHA384
	if (hash_len == 48) {
		return sha384_vector(1, &data, &len, hash);
	}
#endif /* CONFIG_SHA384 */
#ifdef CONFIG_SHA512
	if (hash_len == 64) {
		return sha512_vector(1, &data, &len, hash);
	}
#endif /* CONFIG_SHA512 */
	return -1;
}


static int sae_pk_hash_sig_data(struct sae_data *sae, size_t hash_len,
								bool ap, const u8 *m, size_t m_len,
								const u8 *pubkey, size_t pubkey_len, u8 *hash)
{
	struct sae_temporary_data *tmp = sae->tmp;
	struct wpabuf *sig_data;
	u8 *pos;
	int ret = -1;

	/* Signed data for KeyAuth: eleAP || eleSTA || scaAP || scaSTA ||
	 * M || K_AP || AP-BSSID || STA-MAC */
	sig_data = wpabuf_alloc(tmp->prime_len * 6 + m_len + pubkey_len +
							2 * ETH_ALEN);
	if (!sig_data) {
		goto fail;
	}
	pos = wpabuf_put(sig_data, 2 * tmp->prime_len);
	if (crypto_ec_point_to_bin(tmp->ec, ap ? tmp->own_commit_element_ecc :
							   tmp->peer_commit_element_ecc,
							   pos, pos + tmp->prime_len) < 0) {
		goto fail;
	}
	pos = wpabuf_put(sig_data, 2 * tmp->prime_len);
	if (crypto_ec_point_to_bin(tmp->ec, ap ? tmp->peer_commit_element_ecc :
							   tmp->own_commit_element_ecc,
							   pos, pos + tmp->prime_len) < 0) {
		goto fail;
	}
	if (crypto_bignum_to_bin(ap ? tmp->own_commit_scalar :
							 sae->peer_commit_scalar,
							 wpabuf_put(sig_data, tmp->prime_len),
							 tmp->prime_len, tmp->prime_len) < 0 ||
			crypto_bignum_to_bin(ap ? sae->peer_commit_scalar :
								 tmp->own_commit_scalar,
								 wpabuf_put(sig_data, tmp->prime_len),
								 tmp->prime_len, tmp->prime_len) < 0) {
		goto fail;
	}
	wpabuf_put_data(sig_data, m, m_len);
	wpabuf_put_data(sig_data, pubkey, pubkey_len);
	wpabuf_put_data(sig_data, ap ? tmp->own_addr : tmp->peer_addr,
					ETH_ALEN);
	wpabuf_put_data(sig_data, ap ? tmp->peer_addr : tmp->own_addr,
					ETH_ALEN);
	wpa_hexdump_buf_key(MSG_DEBUG, "SAE-PK: Data to be signed for KeyAuth",
						sig_data);
	if (sae_hash(hash_len, wpabuf_head(sig_data), wpabuf_len(sig_data),
				 hash) < 0) {
		goto fail;
	}
	wpa_hexdump(MSG_DEBUG, "SAE-PK: hash(data to be signed)",
				hash, hash_len);
	ret = 0;
fail:
	wpabuf_free(sig_data);
	return ret;
}


static bool sae_pk_valid_fingerprint(struct sae_data *sae,
									 const u8 *m, size_t m_len,
									 const u8 *k_ap, size_t k_ap_len, int group)
{
	struct sae_temporary_data *tmp = sae->tmp;
	u8 *hash_data, *pos;
	size_t hash_len, hash_data_len;
	u8 hash[SAE_MAX_HASH_LEN];
	int res;

	if (!tmp->fingerprint_bytes) {
		wpa_printf(MSG_DEBUG,
				   "SAE-PK: No PW available for K_AP fingerprint check");
		return false;
	}

	/* Fingerprint = L(Hash(SSID || M || K_AP), 0, 8*Sec + 19*Lambda/4 - 5)
	 */

	hash_len = sae_group_2_hash_len(group);
	hash_data_len = tmp->ssid_len + m_len + k_ap_len;
	hash_data = os_malloc(hash_data_len);
	if (!hash_data) {
		return false;
	}
	pos = hash_data;
	os_memcpy(pos, tmp->ssid, tmp->ssid_len);
	pos += tmp->ssid_len;
	os_memcpy(pos, m, m_len);
	pos += m_len;
	os_memcpy(pos, k_ap, k_ap_len);

	wpa_hexdump_key(MSG_DEBUG, "SAE-PK: SSID || M || K_AP",
					hash_data, hash_data_len);
	res = sae_hash(hash_len, hash_data, hash_data_len, hash);
	bin_clear_free(hash_data, hash_data_len);
	if (res < 0) {
		return false;
	}
	wpa_hexdump(MSG_DEBUG, "SAE-PK: Hash(SSID || M || K_AP)",
				hash, hash_len);

	if (tmp->fingerprint_bits > hash_len * 8) {
		wpa_printf(MSG_INFO,
				   "SAE-PK: Not enough hash output bits for the fingerprint");
		return false;
	}
	if (tmp->fingerprint_bits % 8) {
		size_t extra;

		/* Zero out the extra bits in the last octet */
		extra = 8 - tmp->fingerprint_bits % 8;
		pos = &hash[tmp->fingerprint_bits / 8];
		*pos = (*pos >> extra) << extra;
	}
	wpa_hexdump(MSG_DEBUG, "SAE-PK: Fingerprint", hash,
				tmp->fingerprint_bytes);
	res = os_memcmp_const(hash, tmp->fingerprint, tmp->fingerprint_bytes);
	if (res) {
		wpa_printf(MSG_DEBUG, "SAE-PK: K_AP fingerprint mismatch");
		wpa_hexdump(MSG_DEBUG, "SAE-PK: Expected fingerprint",
					tmp->fingerprint, tmp->fingerprint_bytes);
		return false;
	}

	wpa_printf(MSG_DEBUG, "SAE-PK: Valid K_AP fingerprint");
	return true;
}


int sae_check_confirm_pk(struct sae_data *sae, const u8 *ies, size_t ies_len)
{
	struct sae_temporary_data *tmp = sae->tmp;
	const u8 *k_ap;
	u8 m[SAE_PK_M_LEN];
	size_t k_ap_len;
	struct crypto_ec_key *key;
	int res;
	u8 hash[SAE_MAX_HASH_LEN];
	size_t hash_len;
	int group;
	struct wpa_supplicant *wpa_s = &g_wpa_supp;
	struct sae_pk_elems elems;
	int ret = 0;

	if (!tmp) {
		return -1;
	}
	if (!sae->pk || tmp->ap_pk) {
		return 0;
	}

	if (tmp->kek_len != 32 && tmp->kek_len != 48 && tmp->kek_len != 64) {
		wpa_printf(MSG_INFO,
				   "SAE-PK: No KEK available for checking confirm");
		return -1;
	}

	if (!tmp->ec) {
		/* Only ECC groups are supported for SAE-PK in the current
		 * implementation. */
		wpa_printf(MSG_INFO,
				   "SAE-PK: SAE commit did not use an ECC group");
		return -1;
	}

	wpa_hexdump(MSG_DEBUG, "SAE-PK: Received confirm IEs", ies, ies_len);
	ieee802_11_parse_elems(wpa_s, ies, ies_len);

	elems = wpa_s->sae_pk_elems;

	if (!elems.fils_pk || !elems.fils_key_confirm || !elems.sae_pk) {
		wpa_printf(MSG_INFO,
				   "SAE-PK: Not all mandatory IEs included in confirm");
		ret = -1;
		goto done;
	}

	/* TODO: Fragment reassembly */

	if (elems.sae_pk_len < SAE_PK_M_LEN + AES_BLOCK_SIZE) {
		wpa_printf(MSG_INFO,
				   "SAE-PK: No room for EncryptedModifier in SAE-PK element");
		ret = -1;
		goto done;
	}

	wpa_hexdump(MSG_DEBUG, "SAE-PK: EncryptedModifier",
				elems.sae_pk, SAE_PK_M_LEN + AES_BLOCK_SIZE);

	if (aes_siv_decrypt(tmp->kek, tmp->kek_len,
						elems.sae_pk, SAE_PK_M_LEN + AES_BLOCK_SIZE,
						0, NULL, NULL, m) < 0) {
		wpa_printf(MSG_INFO,
				   "SAE-PK: Failed to decrypt EncryptedModifier");
		ret = -1;
		goto done;
	}
	wpa_hexdump_key(MSG_DEBUG, "SAE-PK: Modifier M", m, SAE_PK_M_LEN);

	if (elems.fils_pk[0] != 2) {
		wpa_printf(MSG_INFO, "SAE-PK: Unsupported public key type %u",
				   elems.fils_pk[0]);
		ret = -1;
		goto done;
	}
	k_ap_len = elems.fils_pk_len - 1;
	k_ap = elems.fils_pk + 1;
	wpa_hexdump(MSG_DEBUG, "SAE-PK: Received K_AP", k_ap, k_ap_len);
	/* TODO: Check against the public key, if one is stored in the network
	 * profile */
	key = crypto_ec_key_parse_pub(k_ap, k_ap_len);
	if (!key) {
		wpa_printf(MSG_INFO, "SAE-PK: Failed to parse K_AP");
		ret = -1;
		goto done;
	}
	group = crypto_ec_key_group(key);
	if (!sae_pk_valid_fingerprint(sae, m, SAE_PK_M_LEN, k_ap, k_ap_len,
								  group)) {
		crypto_ec_key_deinit(key);
		ret = -1;
		goto done;
	}

	wpa_hexdump(MSG_DEBUG, "SAE-PK: Received KeyAuth",
				elems.fils_key_confirm, elems.fils_key_confirm_len);

	hash_len = sae_group_2_hash_len(group);
	if (sae_pk_hash_sig_data(sae, hash_len, false, m, SAE_PK_M_LEN,
							 k_ap, k_ap_len, hash) < 0) {
		crypto_ec_key_deinit(key);
		ret = -1;
		goto done;
	}

	res = crypto_ec_key_verify_signature(key, hash, hash_len,
										 elems.fils_key_confirm,
										 elems.fils_key_confirm_len);
	crypto_ec_key_deinit(key);

	if (res != 1) {
		wpa_printf(MSG_INFO,
				   "SAE-PK: Invalid or incorrect signature in KeyAuth");
		ret = -1;
		goto done;
	}

	wpa_printf(MSG_DEBUG, "SAE-PK: Valid KeyAuth signature received");

	/* TODO: Store validated public key into network profile */
done:
	if (wpa_s->sae_pk_elems.fils_pk) {
		os_free(wpa_s->sae_pk_elems.fils_pk);
	}
	if (wpa_s->sae_pk_elems.sae_pk) {
		os_free(wpa_s->sae_pk_elems.sae_pk);
	}
	if (wpa_s->sae_pk_elems.fils_key_confirm) {
		os_free(wpa_s->sae_pk_elems.fils_key_confirm);
	}
	os_memset(&wpa_s->sae_pk_elems, 0, sizeof(wpa_s->sae_pk_elems));

	return ret;
}
#endif /* CONFIG_SAE_PK */
