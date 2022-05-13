/*
 * ASN.1 DER parsing
 * Copyright (c) 2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef ASN1_H
#define ASN1_H

#define ASN1_TAG_EOC		0x00 /* not used with DER */
#define ASN1_TAG_BOOLEAN	0x01
#define ASN1_TAG_INTEGER	0x02
#define ASN1_TAG_BITSTRING	0x03
#define ASN1_TAG_OCTETSTRING	0x04
#define ASN1_TAG_NULL		0x05
#define ASN1_TAG_OID		0x06
#define ASN1_TAG_OBJECT_DESCRIPTOR	0x07 /* not yet parsed */
#define ASN1_TAG_EXTERNAL	0x08 /* not yet parsed */
#define ASN1_TAG_REAL		0x09 /* not yet parsed */
#define ASN1_TAG_ENUMERATED	0x0A /* not yet parsed */
#define ASN1_TAG_EMBEDDED_PDV	0x0B /* not yet parsed */
#define ASN1_TAG_UTF8STRING	0x0C /* not yet parsed */
#define ANS1_TAG_RELATIVE_OID	0x0D
#define ASN1_TAG_TIME		0x0E
#define ASN1_TAG_SEQUENCE	0x10 /* shall be constructed */
#define ASN1_TAG_SET		0x11
#define ASN1_TAG_NUMERICSTRING	0x12 /* not yet parsed */
#define ASN1_TAG_PRINTABLESTRING	0x13
#define ASN1_TAG_T61STRING	0x14 /* not yet parsed */
#define ASN1_TAG_VIDEOTEXSTRING	0x15 /* not yet parsed */
#define ASN1_TAG_IA5STRING	0x16
#define ASN1_TAG_UTCTIME	0x17
#define ASN1_TAG_GENERALIZEDTIME	0x18 /* not yet parsed */
#define ASN1_TAG_GRAPHICSTRING	0x19 /* not yet parsed */
#define ASN1_TAG_VISIBLESTRING	0x1A
#define ASN1_TAG_GENERALSTRING	0x1B /* not yet parsed */
#define ASN1_TAG_UNIVERSALSTRING	0x1C /* not yet parsed */
#define ASN1_TAG_CHARACTERSTRING	0x1D /* not yet parsed */
#define ASN1_TAG_BMPSTRING	0x1E /* not yet parsed */

#define ASN1_CLASS_UNIVERSAL		0
#define ASN1_CLASS_APPLICATION		1
#define ASN1_CLASS_CONTEXT_SPECIFIC	2
#define ASN1_CLASS_PRIVATE		3


struct asn1_hdr {
	const u8 *payload;
	u8 identifier, class, constructed;
	unsigned int tag, length;
};

#define ASN1_MAX_OID_LEN 20
struct asn1_oid {
	unsigned long oid[ASN1_MAX_OID_LEN];
	size_t len;
};


int asn1_get_next(const u8 *buf, size_t len, struct asn1_hdr *hdr);
void asn1_print_hdr(const struct asn1_hdr *hdr, const char *title);
void asn1_unexpected(const struct asn1_hdr *hdr, const char *title);
int asn1_parse_oid(const u8 *buf, size_t len, struct asn1_oid *oid);
int asn1_get_oid(const u8 *buf, size_t len, struct asn1_oid *oid,
		 const u8 **next);
void asn1_oid_to_str(const struct asn1_oid *oid, char *buf, size_t len);
unsigned long asn1_bit_string_to_long(const u8 *buf, size_t len);
int asn1_oid_equal(const struct asn1_oid *a, const struct asn1_oid *b);
int asn1_get_integer(const u8 *buf, size_t len, int *integer, const u8 **next);
int asn1_get_sequence(const u8 *buf, size_t len, struct asn1_hdr *hdr,
		      const u8 **next);
int asn1_get_alg_id(const u8 *buf, size_t len, struct asn1_oid *oid,
		    const u8 **params, size_t *params_len, const u8 **next);
void asn1_put_integer(struct wpabuf *buf, int val);
void asn1_put_octet_string(struct wpabuf *buf, const struct wpabuf *val);
void asn1_put_oid(struct wpabuf *buf, const struct asn1_oid *oid);
void asn1_put_hdr(struct wpabuf *buf, u8 class, int constructed, u8 tag,
		  size_t len);
void asn1_put_sequence(struct wpabuf *buf, const struct wpabuf *payload);
void asn1_put_set(struct wpabuf *buf, const struct wpabuf *payload);
void asn1_put_utf8string(struct wpabuf *buf, const char *val);
struct wpabuf * asn1_build_alg_id(const struct asn1_oid *oid,
				  const struct wpabuf *params);
struct wpabuf * asn1_encaps(struct wpabuf *buf, u8 class, u8 tag);

static inline bool asn1_is_oid(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_OID;
}

static inline bool asn1_is_boolean(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_BOOLEAN;
}

static inline bool asn1_is_integer(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_INTEGER;
}

static inline bool asn1_is_enumerated(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_ENUMERATED;
}

static inline bool asn1_is_sequence(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_SEQUENCE;
}

static inline bool asn1_is_set(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_SET;
}

static inline bool asn1_is_octetstring(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_OCTETSTRING;
}

static inline bool asn1_is_bitstring(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_BITSTRING;
}

static inline bool asn1_is_utctime(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_UTCTIME;
}

static inline bool asn1_is_generalizedtime(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_GENERALIZEDTIME;
}

static inline bool asn1_is_string_type(const struct asn1_hdr *hdr)
{
	if (hdr->class != ASN1_CLASS_UNIVERSAL || hdr->constructed)
		return false;
	return hdr->tag == ASN1_TAG_UTF8STRING ||
		hdr->tag == ASN1_TAG_NUMERICSTRING ||
		hdr->tag == ASN1_TAG_PRINTABLESTRING ||
		hdr->tag == ASN1_TAG_T61STRING ||
		hdr->tag == ASN1_TAG_VIDEOTEXSTRING ||
		hdr->tag == ASN1_TAG_IA5STRING ||
		hdr->tag == ASN1_TAG_GRAPHICSTRING ||
		hdr->tag == ASN1_TAG_VISIBLESTRING ||
		hdr->tag == ASN1_TAG_GENERALSTRING ||
		hdr->tag == ASN1_TAG_UNIVERSALSTRING ||
		hdr->tag == ASN1_TAG_CHARACTERSTRING ||
		hdr->tag == ASN1_TAG_BMPSTRING;
}

static inline bool asn1_is_bmpstring(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_BMPSTRING;
}

static inline bool asn1_is_utf8string(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_UTF8STRING;
}

static inline bool asn1_is_null(const struct asn1_hdr *hdr)
{
	return hdr->class == ASN1_CLASS_UNIVERSAL &&
		hdr->tag == ASN1_TAG_NULL;
}

static inline bool asn1_is_cs_tag(const struct asn1_hdr *hdr, unsigned int tag)
{
	return hdr->class == ASN1_CLASS_CONTEXT_SPECIFIC &&
		hdr->tag == tag;
}

extern const struct asn1_oid asn1_sha1_oid;
extern const struct asn1_oid asn1_sha256_oid;
extern const struct asn1_oid asn1_ec_public_key_oid;
extern const struct asn1_oid asn1_prime256v1_oid;
extern const struct asn1_oid asn1_secp384r1_oid;
extern const struct asn1_oid asn1_secp521r1_oid;
extern const struct asn1_oid asn1_brainpoolP256r1_oid;
extern const struct asn1_oid asn1_brainpoolP384r1_oid;
extern const struct asn1_oid asn1_brainpoolP512r1_oid;
extern const struct asn1_oid asn1_aes_siv_cmac_aead_256_oid;
extern const struct asn1_oid asn1_aes_siv_cmac_aead_384_oid;
extern const struct asn1_oid asn1_aes_siv_cmac_aead_512_oid;
extern const struct asn1_oid asn1_aes_siv_cmac_aead_256_oid;
extern const struct asn1_oid asn1_aes_siv_cmac_aead_384_oid;
extern const struct asn1_oid asn1_aes_siv_cmac_aead_512_oid;
extern const struct asn1_oid asn1_pbkdf2_oid;
extern const struct asn1_oid asn1_pbkdf2_hmac_sha256_oid;
extern const struct asn1_oid asn1_pbkdf2_hmac_sha384_oid;
extern const struct asn1_oid asn1_pbkdf2_hmac_sha512_oid;
extern const struct asn1_oid asn1_dpp_config_params_oid;
extern const struct asn1_oid asn1_dpp_asymmetric_key_package_oid;

#endif /* ASN1_H */
