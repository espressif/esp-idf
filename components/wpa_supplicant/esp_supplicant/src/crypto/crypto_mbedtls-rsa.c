/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef ESP_PLATFORM
#include "mbedtls/bignum.h"
#endif

#include "utils/includes.h"
#include "utils/common.h"
#include "crypto.h"
#include "common/defs.h"

#ifdef CONFIG_CRYPTO_MBEDTLS
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#include <mbedtls/error.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/platform.h>
#include <mbedtls/sha256.h>

/* Dummy structures; these are just typecast to struct crypto_rsa_key */
struct crypto_public_key;
struct crypto_private_key;

#ifdef DEBUG_PRINT
static void crypto_dump_verify_info(u32 flags)
{
	char dump_buffer[1024];

	mbedtls_x509_crt_verify_info(dump_buffer, 1024, "  ! ", flags );
	wpa_printf(MSG_ERROR, "%s", dump_buffer);
}
#else
static void crypto_dump_verify_info(u32 flags) { }
#endif

static int crypto_rng_wrapper(void *ctx, unsigned char *buf, size_t len)
{
	return os_get_random(buf, len);
}

int crypto_verify_cert(const u8 *cert_start, int certlen, const u8 *ca_cert_start, int ca_certlen)
{
	int ret;
	u32 flags = 0;

	mbedtls_x509_crt *cert = os_zalloc(sizeof(mbedtls_x509_crt));
	mbedtls_x509_crt *ca_cert = os_zalloc(sizeof(mbedtls_x509_crt));

	if (!cert || !ca_cert) {
		if (cert)
			os_free(cert);
		if (ca_cert)
			os_free(ca_cert);
		wpa_printf(MSG_ERROR, "%s: memory allocation failed", __func__);
		return -1;
	}
	mbedtls_x509_crt_init(cert);
	mbedtls_x509_crt_init(ca_cert);
	ret = mbedtls_x509_crt_parse(cert, cert_start, certlen);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "peer cert parsing failed");
		goto cleanup;
	}
	ret = mbedtls_x509_crt_parse(ca_cert, ca_cert_start, ca_certlen);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "CA cert parsing failed");
		goto cleanup;
	}

	ret = mbedtls_x509_crt_verify(cert, ca_cert, NULL, NULL, &flags, NULL, NULL );

	/* Certification is failed, try to get some more info */
	if (ret != 0)
		crypto_dump_verify_info(flags);

cleanup:
	mbedtls_x509_crt_free(cert);
	mbedtls_x509_crt_free(ca_cert);

	os_free(cert);
	os_free(ca_cert);

	return ret;
}

struct crypto_public_key *  crypto_public_key_import(const u8 *key, size_t len)
{
	int ret;
	mbedtls_pk_context *pkey = os_zalloc(sizeof(*pkey));

	if (!pkey)
		return NULL;

	mbedtls_pk_init(pkey);
	ret = mbedtls_pk_parse_public_key(pkey, key, len);

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "failed to parse public key");
		os_free(pkey);
		return NULL;
	}

	return (struct crypto_public_key *)pkey;
}

struct crypto_private_key *  crypto_private_key_import(const u8 *key,
		size_t len,
		const char *passwd)
{
	int ret;
	mbedtls_pk_context *pkey = os_zalloc(sizeof(mbedtls_pk_context));
	if (!pkey)
		return NULL;

	mbedtls_pk_init(pkey);

	ret = mbedtls_pk_parse_key(pkey, key, len, (const unsigned char *)passwd,
			passwd ? os_strlen(passwd) : 0, crypto_rng_wrapper, NULL);

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "failed to parse private key");
		os_free(pkey);
		pkey = NULL;
	}

	return (struct crypto_private_key *)pkey;
}

struct crypto_public_key *crypto_public_key_from_cert(const u8 *buf,
		size_t len)
{
	int ret;
	mbedtls_x509_crt *cert;
	mbedtls_pk_context *kctx = os_zalloc(sizeof(*kctx));

	if (!kctx) {
		wpa_printf(MSG_ERROR, "failed to allocate memory");
		return NULL;
	}

	cert = os_zalloc(sizeof(mbedtls_x509_crt));
	if (!cert) {
		wpa_printf(MSG_ERROR, "failed to allocate memory");
		goto fail;
	}
	mbedtls_x509_crt_init(cert);

	ret = mbedtls_x509_crt_parse(cert, buf, len);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "cert parsing failed");
		goto fail;
	}

	mbedtls_pk_init(kctx);

	if(mbedtls_pk_setup(kctx, mbedtls_pk_info_from_type(mbedtls_pk_get_type(&cert->pk))) != 0) {
		wpa_printf(MSG_ERROR, "key setup failed");
		goto fail;
	}
	ret = mbedtls_rsa_copy(mbedtls_pk_rsa(*kctx), mbedtls_pk_rsa(cert->pk));

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "key copy failed");
		goto fail;
	}

cleanup:
	mbedtls_x509_crt_free(cert);
	os_free(cert);
	return (struct crypto_public_key *)kctx;
fail:
	os_free(kctx);
	kctx = NULL;
	goto cleanup;
}

int crypto_public_key_encrypt_pkcs1_v15(struct crypto_public_key *key,
		const u8 *in, size_t inlen,
		u8 *out, size_t *outlen)
{
	int ret;
	mbedtls_pk_context *pkey  = (mbedtls_pk_context *)key;
	const char *pers = "rsa_encrypt";
	mbedtls_entropy_context *entropy = os_zalloc(sizeof(*entropy));
	mbedtls_ctr_drbg_context *ctr_drbg = os_zalloc(sizeof(*ctr_drbg));

	if (!pkey || !entropy || !ctr_drbg) {
		if (entropy)
			os_free(entropy);
		if (ctr_drbg)
			os_free(ctr_drbg);
		wpa_printf(MSG_ERROR, "failed to allocate memory");
		return -1;
	}

	mbedtls_entropy_init( entropy );
	mbedtls_ctr_drbg_init( ctr_drbg );

	ret = mbedtls_ctr_drbg_seed( ctr_drbg, mbedtls_entropy_func,
			entropy, (const unsigned char *) pers,
			strlen( pers ) );
	if( ret != 0 ) {
		wpa_printf(MSG_ERROR, " failed  ! mbedtls_ctr_drbg_seed returned %d",
				ret );
		goto cleanup;
	}

	ret = mbedtls_rsa_pkcs1_encrypt(mbedtls_pk_rsa(*pkey), mbedtls_ctr_drbg_random,
					ctr_drbg, inlen, in, out);

	if(ret != 0) {
		wpa_printf(MSG_ERROR, " failed  !  mbedtls_rsa_pkcs1_encrypt returned -0x%04x", -ret);
		goto cleanup;
	}
	*outlen = mbedtls_pk_rsa(*pkey)->MBEDTLS_PRIVATE(len);

cleanup:
	mbedtls_ctr_drbg_free( ctr_drbg );
	mbedtls_entropy_free( entropy );
	os_free(entropy);
	os_free(ctr_drbg);

	return ret;
}


int  crypto_private_key_decrypt_pkcs1_v15(struct crypto_private_key *key,
		const u8 *in, size_t inlen,
		u8 *out, size_t *outlen)
{
	int ret;
	size_t i;
	mbedtls_pk_context *pkey  = (mbedtls_pk_context *)key;
	const char *pers = "rsa_decrypt";
	mbedtls_entropy_context *entropy = os_malloc(sizeof(*entropy));
	mbedtls_ctr_drbg_context *ctr_drbg = os_malloc(sizeof(*ctr_drbg));

	if (!pkey || !entropy || !ctr_drbg) {
		if (entropy)
			os_free(entropy);
		if (ctr_drbg)
			os_free(ctr_drbg);
		return -1;
	}
	mbedtls_ctr_drbg_init( ctr_drbg );
	mbedtls_entropy_init( entropy );
	ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func,
			entropy, (const unsigned char *) pers,
			strlen(pers));

	if (ret < 0)
		goto cleanup;

	i =  mbedtls_pk_rsa(*pkey)->MBEDTLS_PRIVATE(len);
	ret = mbedtls_rsa_rsaes_pkcs1_v15_decrypt(mbedtls_pk_rsa(*pkey), mbedtls_ctr_drbg_random,
			ctr_drbg, &i, in, out, *outlen);

	*outlen = i;

cleanup:
	mbedtls_ctr_drbg_free( ctr_drbg );
	mbedtls_entropy_free( entropy );
	os_free(entropy);
	os_free(ctr_drbg);

	return ret;
}


int crypto_private_key_sign_pkcs1(struct crypto_private_key *key,
		const u8 *in, size_t inlen,
		u8 *out, size_t *outlen)
{
	int ret;
	const char *pers = "rsa_encrypt";
	mbedtls_pk_context *pkey  = (mbedtls_pk_context *)key;
	mbedtls_entropy_context *entropy = os_malloc(sizeof(*entropy));
	mbedtls_ctr_drbg_context *ctr_drbg = os_malloc(sizeof(*ctr_drbg));

	if (!pkey || !entropy || !ctr_drbg) {
		if (entropy)
			os_free(entropy);
		if (ctr_drbg)
			os_free(ctr_drbg);
		return -1;
	}
	mbedtls_ctr_drbg_init( ctr_drbg );
	mbedtls_entropy_init( entropy );
	ret = mbedtls_ctr_drbg_seed(ctr_drbg, mbedtls_entropy_func,
			entropy, (const unsigned char *) pers,
			strlen(pers));

	if((ret = mbedtls_rsa_pkcs1_sign(mbedtls_pk_rsa(*pkey), mbedtls_ctr_drbg_random, ctr_drbg,
					(mbedtls_pk_rsa(*pkey))->MBEDTLS_PRIVATE(hash_id),
					inlen, in, out)) != 0 ) {
		wpa_printf(MSG_ERROR, " failed  ! mbedtls_rsa_pkcs1_sign returned %d", ret );
		goto cleanup;
	}
	*outlen = mbedtls_pk_rsa(*pkey)->MBEDTLS_PRIVATE(len);
cleanup:
	mbedtls_ctr_drbg_free( ctr_drbg );
	mbedtls_entropy_free( entropy );
	os_free(entropy);
	os_free(ctr_drbg);
	return ret;
}


void  crypto_public_key_free(struct crypto_public_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	if (!pkey)
		return;

	mbedtls_pk_free(pkey);
	os_free(pkey);
}


void  crypto_private_key_free(struct crypto_private_key *key)
{
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	if (!pkey)
		return;

	mbedtls_pk_free(pkey);
	os_free(pkey);
}

int  crypto_public_key_decrypt_pkcs1(struct crypto_public_key *key,
		const u8 *crypt, size_t crypt_len,
		u8 *plain, size_t *plain_len)
{
	size_t len;
	u8 *pos;
	mbedtls_pk_context *pkey = (mbedtls_pk_context *)key;
	len = mbedtls_pk_rsa(*pkey)->MBEDTLS_PRIVATE(len);
	if (len != crypt_len) {
		return -1;
	}

	if (mbedtls_rsa_public(mbedtls_pk_rsa(*pkey), crypt, plain) < 0)
		return -1;

	/*
	 * PKCS #1 v1.5, 8.1:
	 *
	 * EB = 00 || BT || PS || 00 || D
	 * BT = 00 or 01
	 * PS = k-3-||D|| times (00 if BT=00) or (FF if BT=01)
	 * k = length of modulus in octets
	 *
	 * Based on 10.1.3, "The block type shall be 01" for a signature.
	 */

	if (len < 3 + 8 + 16 /* min hash len */ ||
	    plain[0] != 0x00 || plain[1] != 0x01) {
		wpa_printf(MSG_INFO, "LibTomCrypt: Invalid signature EB "
			   "structure");
		wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
		return -1;
	}

	pos = plain + 3;
	/* BT = 01 */
	if (plain[2] != 0xff) {
		wpa_printf(MSG_INFO, "LibTomCrypt: Invalid signature "
			   "PS (BT=01)");
		wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
		return -1;
	}
	while (pos < plain + len && *pos == 0xff)
		pos++;

	if (pos - plain - 2 < 8) {
		/* PKCS #1 v1.5, 8.1: At least eight octets long PS */
		wpa_printf(MSG_INFO, "LibTomCrypt: Too short signature "
			   "padding");
		wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
		return -1;
	}

	if (pos + 16 /* min hash len */ >= plain + len || *pos != 0x00) {
		wpa_printf(MSG_INFO, "LibTomCrypt: Invalid signature EB "
			   "structure (2)");
		wpa_hexdump_key(MSG_DEBUG, "Signature EB", plain, len);
		return -1;
	}
	pos++;
	len -= pos - plain;

	/* Strip PKCS #1 header */
	os_memmove(plain, pos, len);
	*plain_len = len;

	return 0;
}
#endif
