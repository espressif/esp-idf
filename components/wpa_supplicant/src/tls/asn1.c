/*
 * ASN.1 DER parsing
 * Copyright (c) 2006-2014, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "utils/wpabuf.h"
#include "asn1.h"

const struct asn1_oid asn1_sha1_oid = {
	.oid = { 1, 3, 14, 3, 2, 26 },
	.len = 6
};

const struct asn1_oid asn1_sha256_oid = {
	.oid = { 2, 16, 840, 1, 101, 3, 4, 2, 1 },
	.len = 9
};

const struct asn1_oid asn1_ec_public_key_oid = {
	.oid = { 1, 2, 840, 10045, 2, 1 },
	.len = 6
};

const struct asn1_oid asn1_prime256v1_oid = {
	.oid = { 1, 2, 840, 10045, 3, 1, 7 },
	.len = 7
};

const struct asn1_oid asn1_secp384r1_oid = {
	.oid = { 1, 3, 132, 0, 34 },
	.len = 5
};

const struct asn1_oid asn1_secp521r1_oid = {
	.oid = { 1, 3, 132, 0, 35 },
	.len = 5
};

const struct asn1_oid asn1_brainpoolP256r1_oid = {
	.oid = { 1, 3, 36, 3, 3, 2, 8, 1, 1, 7 },
	.len = 10
};

const struct asn1_oid asn1_brainpoolP384r1_oid = {
	.oid = { 1, 3, 36, 3, 3, 2, 8, 1, 1, 11 },
	.len = 10
};

const struct asn1_oid asn1_brainpoolP512r1_oid = {
	.oid = { 1, 3, 36, 3, 3, 2, 8, 1, 1, 13 },
	.len = 10
};

const struct asn1_oid asn1_aes_siv_cmac_aead_256_oid = {
	.oid = { 1, 2, 840, 113549, 1, 9, 16, 3, 22 },
	.len = 9
};

const struct asn1_oid asn1_aes_siv_cmac_aead_384_oid = {
	.oid = { 1, 2, 840, 113549, 1, 9, 16, 3, 23 },
	.len = 9
};

const struct asn1_oid asn1_aes_siv_cmac_aead_512_oid = {
	.oid = { 1, 2, 840, 113549, 1, 9, 16, 3, 24 },
	.len = 9
};

const struct asn1_oid asn1_pbkdf2_oid = {
	.oid = { 1, 2, 840, 113549, 1, 5, 12 },
	.len = 7
};

const struct asn1_oid asn1_pbkdf2_hmac_sha256_oid = {
	.oid = { 1, 2, 840, 113549, 2, 9 },
	.len = 6
};

const struct asn1_oid asn1_pbkdf2_hmac_sha384_oid = {
	.oid = { 1, 2, 840, 113549, 2, 10 },
	.len = 6
};

const struct asn1_oid asn1_pbkdf2_hmac_sha512_oid = {
	.oid = { 1, 2, 840, 113549, 2, 11 },
	.len = 6
};

const struct asn1_oid asn1_dpp_config_params_oid = {
	.oid = { 1, 3, 6, 1, 4, 1, 40808, 1, 2, 1 },
	.len = 10
};

const struct asn1_oid asn1_dpp_asymmetric_key_package_oid = {
	.oid = { 1, 3, 6, 1, 4, 1, 40808, 1, 2, 2 },
	.len = 10
};


static int asn1_valid_der_boolean(struct asn1_hdr *hdr)
{
	/* Enforce DER requirements for a single way of encoding a BOOLEAN */
	if (hdr->length != 1) {
		wpa_printf(MSG_DEBUG, "ASN.1: Unexpected BOOLEAN length (%u)",
			   hdr->length);
		return 0;
	}

	if (hdr->payload[0] != 0 && hdr->payload[0] != 0xff) {
		wpa_printf(MSG_DEBUG,
			   "ASN.1: Invalid BOOLEAN value 0x%x (DER requires 0 or 0xff)",
			   hdr->payload[0]);
		return 0;
	}

	return 1;
}


static int asn1_valid_der(struct asn1_hdr *hdr)
{
	if (hdr->class != ASN1_CLASS_UNIVERSAL)
		return 1;
	if (hdr->tag == ASN1_TAG_BOOLEAN && !asn1_valid_der_boolean(hdr))
		return 0;
	if (hdr->tag == ASN1_TAG_NULL && hdr->length != 0)
		return 0;

	/* Check for allowed primitive/constructed values */
	if (hdr->constructed &&
	    (hdr->tag == ASN1_TAG_BOOLEAN ||
	     hdr->tag == ASN1_TAG_INTEGER ||
	     hdr->tag == ASN1_TAG_NULL ||
	     hdr->tag == ASN1_TAG_OID ||
	     hdr->tag == ANS1_TAG_RELATIVE_OID ||
	     hdr->tag == ASN1_TAG_REAL ||
	     hdr->tag == ASN1_TAG_ENUMERATED ||
	     hdr->tag == ASN1_TAG_BITSTRING ||
	     hdr->tag == ASN1_TAG_OCTETSTRING ||
	     hdr->tag == ASN1_TAG_NUMERICSTRING ||
	     hdr->tag == ASN1_TAG_PRINTABLESTRING ||
	     hdr->tag == ASN1_TAG_T61STRING ||
	     hdr->tag == ASN1_TAG_VIDEOTEXSTRING ||
	     hdr->tag == ASN1_TAG_VISIBLESTRING ||
	     hdr->tag == ASN1_TAG_IA5STRING ||
	     hdr->tag == ASN1_TAG_GRAPHICSTRING ||
	     hdr->tag == ASN1_TAG_GENERALSTRING ||
	     hdr->tag == ASN1_TAG_UNIVERSALSTRING ||
	     hdr->tag == ASN1_TAG_UTF8STRING ||
	     hdr->tag == ASN1_TAG_BMPSTRING ||
	     hdr->tag == ASN1_TAG_CHARACTERSTRING ||
	     hdr->tag == ASN1_TAG_UTCTIME ||
	     hdr->tag == ASN1_TAG_GENERALIZEDTIME ||
	     hdr->tag == ASN1_TAG_TIME))
		return 0;
	if (!hdr->constructed &&
	    (hdr->tag == ASN1_TAG_SEQUENCE ||
	     hdr->tag == ASN1_TAG_SET))
		return 0;

	return 1;
}


int asn1_get_next(const u8 *buf, size_t len, struct asn1_hdr *hdr)
{
	const u8 *pos, *end;
	u8 tmp;

	os_memset(hdr, 0, sizeof(*hdr));
	pos = buf;
	end = buf + len;

	if (pos >= end) {
		wpa_printf(MSG_DEBUG, "ASN.1: No room for Identifier");
		return -1;
	}
	hdr->identifier = *pos++;
	hdr->class = hdr->identifier >> 6;
	hdr->constructed = !!(hdr->identifier & (1 << 5));

	if ((hdr->identifier & 0x1f) == 0x1f) {
		size_t ext_len = 0;

		hdr->tag = 0;
		if (pos == end || (*pos & 0x7f) == 0) {
			wpa_printf(MSG_DEBUG,
				   "ASN.1: Invalid extended tag (first octet has to be included with at least one nonzero bit for the tag value)");
			return -1;
		}
		do {
			if (pos >= end) {
				wpa_printf(MSG_DEBUG, "ASN.1: Identifier "
					   "underflow");
				return -1;
			}
			ext_len++;
			tmp = *pos++;
			wpa_printf(MSG_MSGDUMP, "ASN.1: Extended tag data: "
				   "0x%02x", tmp);
			hdr->tag = (hdr->tag << 7) | (tmp & 0x7f);
		} while (tmp & 0x80);
		wpa_printf(MSG_MSGDUMP, "ASN.1: Extended Tag: 0x%x (len=%zu)",
			   hdr->tag, ext_len);
		if ((hdr->class != ASN1_CLASS_PRIVATE && hdr->tag < 31) ||
		    ext_len * 7 > sizeof(hdr->tag) * 8) {
			wpa_printf(MSG_DEBUG,
				   "ASN.1: Invalid or unsupported (too large) extended Tag: 0x%x (len=%zu)",
				   hdr->tag, ext_len);
			return -1;
		}
	} else
		hdr->tag = hdr->identifier & 0x1f;

	if (pos >= end) {
		wpa_printf(MSG_DEBUG, "ASN.1: No room for Length");
		return -1;
	}
	tmp = *pos++;
	if (tmp & 0x80) {
		if (tmp == 0xff) {
			wpa_printf(MSG_DEBUG, "ASN.1: Reserved length "
				   "value 0xff used");
			return -1;
		}
		tmp &= 0x7f; /* number of subsequent octets */
		hdr->length = 0;
		if (tmp == 0 || pos == end || *pos == 0) {
			wpa_printf(MSG_DEBUG,
				   "ASN.1: Definite long form of the length does not start with a nonzero value");
			return -1;
		}
		if (tmp > 4) {
			wpa_printf(MSG_DEBUG, "ASN.1: Too long length field");
			return -1;
		}
		while (tmp--) {
			if (pos >= end) {
				wpa_printf(MSG_DEBUG, "ASN.1: Length "
					   "underflow");
				return -1;
			}
			hdr->length = (hdr->length << 8) | *pos++;
		}
		if (hdr->length < 128) {
			wpa_printf(MSG_DEBUG,
				   "ASN.1: Definite long form of the length used with too short length");
			return -1;
		}
	} else {
		/* Short form - length 0..127 in one octet */
		hdr->length = tmp;
	}

	if (end < pos || hdr->length > (unsigned int) (end - pos)) {
		wpa_printf(MSG_DEBUG, "ASN.1: Contents underflow");
		return -1;
	}

	hdr->payload = pos;

	if (!asn1_valid_der(hdr)) {
		asn1_print_hdr(hdr, "ASN.1: Invalid DER encoding: ");
		return -1;
	}
	return 0;
}


void asn1_print_hdr(const struct asn1_hdr *hdr, const char *title)
{
	wpa_printf(MSG_DEBUG, "%sclass %d constructed %d tag 0x%x",
		   title, hdr->class, hdr->constructed, hdr->tag);
}


void asn1_unexpected(const struct asn1_hdr *hdr, const char *title)
{
	wpa_printf(MSG_DEBUG, "%s - found class %d constructed %d tag 0x%x",
		   title, hdr->class, hdr->constructed, hdr->tag);
}


int asn1_parse_oid(const u8 *buf, size_t len, struct asn1_oid *oid)
{
	const u8 *pos, *end;
	unsigned long val;
	u8 tmp;

	os_memset(oid, 0, sizeof(*oid));

	pos = buf;
	end = buf + len;

	while (pos < end) {
		val = 0;

		do {
			if (pos >= end)
				return -1;
			tmp = *pos++;
			val = (val << 7) | (tmp & 0x7f);
		} while (tmp & 0x80);

		if (oid->len >= ASN1_MAX_OID_LEN) {
			wpa_printf(MSG_DEBUG, "ASN.1: Too long OID value");
			return -1;
		}
		if (oid->len == 0) {
			/*
			 * The first octet encodes the first two object
			 * identifier components in (X*40) + Y formula.
			 * X = 0..2.
			 */
			oid->oid[0] = val / 40;
			if (oid->oid[0] > 2)
				oid->oid[0] = 2;
			oid->oid[1] = val - oid->oid[0] * 40;
			oid->len = 2;
		} else
			oid->oid[oid->len++] = val;
	}

	return 0;
}


int asn1_get_oid(const u8 *buf, size_t len, struct asn1_oid *oid,
		 const u8 **next)
{
	struct asn1_hdr hdr;

	if (asn1_get_next(buf, len, &hdr) < 0 || hdr.length == 0 ||
	    !asn1_is_oid(&hdr)) {
		asn1_unexpected(&hdr, "ASN.1: Expected OID");
		return -1;
	}

	*next = hdr.payload + hdr.length;

	return asn1_parse_oid(hdr.payload, hdr.length, oid);
}


void asn1_oid_to_str(const struct asn1_oid *oid, char *buf, size_t len)
{
	char *pos = buf;
	size_t i;
	int ret;

	if (len == 0)
		return;

	buf[0] = '\0';

	for (i = 0; i < oid->len; i++) {
		ret = os_snprintf(pos, buf + len - pos,
				  "%s%lu",
				  i == 0 ? "" : ".", oid->oid[i]);
		if (os_snprintf_error(buf + len - pos, ret))
			break;
		pos += ret;
	}
	buf[len - 1] = '\0';
}


static u8 rotate_bits(u8 octet)
{
	int i;
	u8 res;

	res = 0;
	for (i = 0; i < 8; i++) {
		res <<= 1;
		if (octet & 1)
			res |= 1;
		octet >>= 1;
	}

	return res;
}


unsigned long asn1_bit_string_to_long(const u8 *buf, size_t len)
{
	unsigned long val = 0;
	const u8 *pos = buf;

	/* BER requires that unused bits are zero, so we can ignore the number
	 * of unused bits */
	pos++;

	if (len >= 2)
		val |= rotate_bits(*pos++);
	if (len >= 3)
		val |= ((unsigned long) rotate_bits(*pos++)) << 8;
	if (len >= 4)
		val |= ((unsigned long) rotate_bits(*pos++)) << 16;
	if (len >= 5)
		val |= ((unsigned long) rotate_bits(*pos++)) << 24;
	if (len >= 6)
		wpa_printf(MSG_DEBUG, "X509: %s - some bits ignored "
			   "(BIT STRING length %lu)",
			   __func__, (unsigned long) len);

	return val;
}


int asn1_oid_equal(const struct asn1_oid *a, const struct asn1_oid *b)
{
	size_t i;

	if (a->len != b->len)
		return 0;

	for (i = 0; i < a->len; i++) {
		if (a->oid[i] != b->oid[i])
			return 0;
	}

	return 1;
}


int asn1_get_integer(const u8 *buf, size_t len, int *integer, const u8 **next)
{
	struct asn1_hdr hdr;
	size_t left;
	const u8 *pos;
	int value;

	if (asn1_get_next(buf, len, &hdr) < 0 || hdr.length == 0 ||
	    !asn1_is_integer(&hdr)) {
		asn1_unexpected(&hdr, "ASN.1: Expected INTEGER");
		return -1;
	}

	*next = hdr.payload + hdr.length;
	pos = hdr.payload;
	left = hdr.length;
	if (left > sizeof(value)) {
		wpa_printf(MSG_DEBUG, "ASN.1: Too large INTEGER (len %u)",
			   hdr.length);
		return -1;
	}
	value = 0;
	while (left) {
		value <<= 8;
		value |= *pos++;
		left--;
	}

	*integer = value;
	return 0;
}


int asn1_get_sequence(const u8 *buf, size_t len, struct asn1_hdr *hdr,
		      const u8 **next)
{
	if (asn1_get_next(buf, len, hdr) < 0 || !asn1_is_sequence(hdr)) {
		asn1_unexpected(hdr, "ASN.1: Expected SEQUENCE");
		return -1;
	}

	if (next)
		*next = hdr->payload + hdr->length;
	return 0;
}


int asn1_get_alg_id(const u8 *buf, size_t len, struct asn1_oid *oid,
		    const u8 **params, size_t *params_len, const u8 **next)
{
	const u8 *pos = buf, *end = buf + len;
	struct asn1_hdr hdr;

	/*
	 * AlgorithmIdentifier ::= SEQUENCE {
	 *     algorithm            OBJECT IDENTIFIER,
	 *     parameters           ANY DEFINED BY algorithm OPTIONAL}
	 */
	if (asn1_get_sequence(pos, end - pos, &hdr, next) < 0 ||
	    asn1_get_oid(hdr.payload, hdr.length, oid, &pos) < 0)
		return -1;

	if (params && params_len) {
		*params = pos;
		*params_len = hdr.payload + hdr.length - pos;
	}

	return 0;
}


void asn1_put_integer(struct wpabuf *buf, int val)
{
	u8 bin[4];
	int zeros;

	WPA_PUT_BE32(bin, val);
	zeros = 0;
	while (zeros < 3 && bin[zeros] == 0)
		zeros++;
	wpabuf_put_u8(buf, ASN1_TAG_INTEGER);
	wpabuf_put_u8(buf, 4 - zeros);
	wpabuf_put_data(buf, &bin[zeros], 4 - zeros);
}


static void asn1_put_len(struct wpabuf *buf, size_t len)
{
	if (len <= 0x7f) {
		wpabuf_put_u8(buf, len);
	} else if (len <= 0xff) {
		wpabuf_put_u8(buf, 0x80 | 1);
		wpabuf_put_u8(buf, len);
	} else if (len <= 0xffff) {
		wpabuf_put_u8(buf, 0x80 | 2);
		wpabuf_put_be16(buf, len);
	} else if (len <= 0xffffff) {
		wpabuf_put_u8(buf, 0x80 | 3);
		wpabuf_put_be24(buf, len);
	} else {
		wpabuf_put_u8(buf, 0x80 | 4);
		wpabuf_put_be32(buf, len);
	}
}


void asn1_put_octet_string(struct wpabuf *buf, const struct wpabuf *val)
{
	wpabuf_put_u8(buf, ASN1_TAG_OCTETSTRING);
	asn1_put_len(buf, wpabuf_len(val));
	wpabuf_put_buf(buf, val);
}


void asn1_put_oid(struct wpabuf *buf, const struct asn1_oid *oid)
{
	u8 *len;
	size_t i;

	if (oid->len < 2)
		return;
	wpabuf_put_u8(buf, ASN1_TAG_OID);
	len = wpabuf_put(buf, 1);
	wpabuf_put_u8(buf, 40 * oid->oid[0] + oid->oid[1]);
	for (i = 2; i < oid->len; i++) {
		unsigned long val = oid->oid[i];
		u8 bytes[8];
		int idx = 0;

		while (val) {
			bytes[idx] = (idx ? 0x80 : 0x00) | (val & 0x7f);
			idx++;
			val >>= 7;
		}
		if (idx == 0) {
			bytes[idx] = 0;
			idx = 1;
		}
		while (idx > 0) {
			idx--;
			wpabuf_put_u8(buf, bytes[idx]);
		}
	}
	*len = (u8 *) wpabuf_put(buf, 0) - len - 1;
}


void asn1_put_hdr(struct wpabuf *buf, u8 class, int constructed, u8 tag,
		  size_t len)
{
	wpabuf_put_u8(buf, class << 6 | (constructed ? 0x20 : 0x00) | tag);
	asn1_put_len(buf, len);
}


void asn1_put_sequence(struct wpabuf *buf, const struct wpabuf *payload)
{
	asn1_put_hdr(buf, ASN1_CLASS_UNIVERSAL, 1, ASN1_TAG_SEQUENCE,
		     wpabuf_len(payload));
	wpabuf_put_buf(buf, payload);
}


void asn1_put_set(struct wpabuf *buf, const struct wpabuf *payload)
{
	asn1_put_hdr(buf, ASN1_CLASS_UNIVERSAL, 1, ASN1_TAG_SET,
		     wpabuf_len(payload));
	wpabuf_put_buf(buf, payload);
}


void asn1_put_utf8string(struct wpabuf *buf, const char *val)
{
	asn1_put_hdr(buf, ASN1_CLASS_UNIVERSAL, 0, ASN1_TAG_UTF8STRING,
		     os_strlen(val));
	wpabuf_put_str(buf, val);
}


struct wpabuf * asn1_build_alg_id(const struct asn1_oid *oid,
				  const struct wpabuf *params)
{
	struct wpabuf *buf;
	size_t len;

	/*
	 * AlgorithmIdentifier ::= SEQUENCE {
	 *    algorithm		OBJECT IDENTIFIER,
	 *    parameters	ANY DEFINED BY algorithm OPTIONAL}
	 */

	len = 100;
	if (params)
		len += wpabuf_len(params);
	buf = wpabuf_alloc(len);
	if (!buf)
		return NULL;
	asn1_put_oid(buf, oid);
	if (params)
		wpabuf_put_buf(buf, params);
	return asn1_encaps(buf, ASN1_CLASS_UNIVERSAL, ASN1_TAG_SEQUENCE);
}


struct wpabuf * asn1_encaps(struct wpabuf *buf, u8 class, u8 tag)
{
	struct wpabuf *res;

	if (!buf)
		return NULL;
	res = wpabuf_alloc(10 + wpabuf_len(buf));
	if (res) {
		asn1_put_hdr(res, class, 1, tag, wpabuf_len(buf));
		wpabuf_put_buf(res, buf);
	}
	wpabuf_clear_free(buf);
	return res;
}
