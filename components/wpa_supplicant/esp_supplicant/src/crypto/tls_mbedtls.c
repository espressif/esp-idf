/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "utils/includes.h"
#include "utils/common.h"

#include "crypto/tls.h"
#include "crypto/sha1.h"
#include "crypto/md5.h"
#include "crypto/sha256.h"
#include "crypto/sha384.h"

/* TODO: Remove this once the appropriate solution is found
 *
 * ssl_misc.h header uses private elements from
 * mbedtls, which become undefined if the following flag
 * is not defined
 */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

// located at mbedtls/library/ssl_misc.h
#include "ssl_misc.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/debug.h"
#include "mbedtls/oid.h"
#ifdef ESPRESSIF_USE
#include "mbedtls/esp_debug.h"
#include "mbedtls/esp_config.h"
#else
#include "mbedtls/config.h"
#endif
#include "mbedtls/platform.h"
#include "eap_peer/eap.h"


#define TLS_RANDOM_LEN 32
#define TLS_MASTER_SECRET_LEN 48
#define MAX_CIPHERSUITE 32



/* Throw a compilation error if basic requirements in mbedtls are not enabled */
#if !defined(MBEDTLS_SSL_TLS_C)
#error "TLS not enabled in mbedtls config"
#endif

#if !defined(MBEDTLS_SHA256_C)
#error "SHA256 is disabled in mbedtls config"
#endif

#if !defined(MBEDTLS_AES_C)
#error "AES support is disabled in mbedtls config"
#endif

uint32_t tls_instance_count;
struct tls_data {
	/* Data for mbedlts */
	struct wpabuf *in_data;
	/* Data from mbedtls */
	struct wpabuf *out_data;
};

typedef struct tls_context {
	mbedtls_ssl_context ssl;            /*!< TLS/SSL context */
	mbedtls_entropy_context entropy;    /*!< mbedTLS entropy context structure */
	mbedtls_ctr_drbg_context ctr_drbg;  /*!< mbedTLS ctr drbg context structure */
	mbedtls_ssl_config conf;            /*!< TLS/SSL config to be shared structures */
	mbedtls_x509_crt cacert;            /*!< Container for X.509 CA certificate */
	mbedtls_x509_crt *cacert_ptr;       /*!< Pointer to the cacert being used. */
	mbedtls_x509_crt clientcert;        /*!< Container for X.509 client certificate */
	mbedtls_pk_context clientkey;       /*!< Private key of client certificate */
	int ciphersuite[MAX_CIPHERSUITE];
} tls_context_t;

struct tls_connection {
	tls_context_t *tls;
	struct tls_data tls_io_data;
	unsigned char master_secret[TLS_MASTER_SECRET_LEN];
	unsigned char randbytes[2 * TLS_RANDOM_LEN];
        mbedtls_tls_prf_types tls_prf_type;
	mbedtls_md_type_t mac;
};

static void tls_mbedtls_cleanup(tls_context_t *tls)
{
	if (!tls) {
		return;
	}
	tls->cacert_ptr = NULL;
	mbedtls_x509_crt_free(&tls->cacert);
	mbedtls_x509_crt_free(&tls->clientcert);
	mbedtls_pk_free(&tls->clientkey);
	mbedtls_entropy_free(&tls->entropy);
	mbedtls_ssl_config_free(&tls->conf);
	mbedtls_ctr_drbg_free(&tls->ctr_drbg);
	mbedtls_ssl_free(&tls->ssl);
}

static void tls_mbedtls_conn_delete(tls_context_t *tls)
{
	if (tls != NULL) {
		tls_mbedtls_cleanup(tls);
	}
}

static int tls_mbedtls_write(void *ctx, const unsigned char *buf, size_t len)
{
	struct tls_connection *conn = (struct tls_connection *)ctx;
	struct tls_data *data = &conn->tls_io_data;

	if (wpabuf_resize(&data->out_data, len) < 0)
		return 0;

	wpabuf_put_data(data->out_data, buf, len);

	return len;
}

static int tls_mbedtls_read(void *ctx, unsigned char *buf, size_t len)
{
	struct tls_connection *conn = (struct tls_connection *)ctx;
	struct tls_data *data = &conn->tls_io_data;
	struct wpabuf *local_buf;

	if (data->in_data == NULL || len > wpabuf_len(data->in_data)) {
		/* We don't have suffient buffer available for read */
		wpa_printf(MSG_INFO, "len=%zu not available in input", len);
		return MBEDTLS_ERR_SSL_WANT_READ;
	}

	os_memcpy(buf, wpabuf_head(data->in_data), len);
	/* adjust buffer */
	if (len < wpabuf_len(data->in_data)) {
		/* TODO optimize this operation */
		local_buf = wpabuf_alloc_copy(wpabuf_mhead_u8(data->in_data) + len,
					      wpabuf_len(data->in_data) - len);
		wpabuf_free(data->in_data);
		data->in_data = local_buf;
	} else {
		wpabuf_free(data->in_data);
		data->in_data = NULL;
	}

	return len;
}

static int set_pki_context(tls_context_t *tls, const struct tls_connection_params *cfg)
{
	int ret;

	if (cfg->client_cert_blob == NULL || cfg->private_key_blob  == NULL) {
		wpa_printf(MSG_ERROR, "%s: config not correct", __func__);
		return -1;
	}

	mbedtls_x509_crt_init(&tls->clientcert);
	mbedtls_pk_init(&tls->clientkey);

	ret = mbedtls_x509_crt_parse(&tls->clientcert,
				     cfg->client_cert_blob, cfg->client_cert_blob_len);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "mbedtls_x509_crt_parse returned -0x%x", -ret);
		return ret;
	}

	ret = mbedtls_pk_parse_key(&tls->clientkey, cfg->private_key_blob, cfg->private_key_blob_len,
				   (const unsigned char *)cfg->private_key_passwd,
				   cfg->private_key_passwd ? os_strlen(cfg->private_key_passwd) : 0, mbedtls_ctr_drbg_random, &tls->ctr_drbg);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "mbedtls_pk_parse_keyfile returned -0x%x", -ret);
		return ret;
	}

	ret = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->clientcert, &tls->clientkey);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "mbedtls_ssl_conf_own_cert returned -0x%x", -ret);
		return ret;
	}

	return 0;
}

static int set_ca_cert(tls_context_t *tls, const unsigned char *cacert, size_t cacert_len)
{
	tls->cacert_ptr = &tls->cacert;
	mbedtls_x509_crt_init(tls->cacert_ptr);
	int ret = mbedtls_x509_crt_parse(tls->cacert_ptr, cacert, cacert_len);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "mbedtls_x509_crt_parse returned -0x%x", -ret);
		return ret;
	}
	mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
	return 0;
}

#ifdef CONFIG_SUITEB192
static uint16_t tls_sig_algs_for_suiteb[] = {
#if defined(MBEDTLS_SHA512_C)
#if defined(MBEDTLS_ECDSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA512 ),
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA384 ),
#endif
#if defined(MBEDTLS_RSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA512 ),
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA384 ),
#endif
#endif /* MBEDTLS_SHA512_C */
    MBEDTLS_TLS_SIG_NONE
};

const mbedtls_x509_crt_profile suiteb_mbedtls_x509_crt_profile =
{
#if defined(MBEDTLS_SHA512_C)
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ) |
#endif
	0,
	0xFFFFFFF, /* Any PK alg    */
	MBEDTLS_X509_ID_FLAG(MBEDTLS_ECP_DP_SECP384R1),
	1024,
};

static void tls_set_suiteb_config(tls_context_t *tls)
{
	const mbedtls_x509_crt_profile *crt_profile = &suiteb_mbedtls_x509_crt_profile;
	mbedtls_ssl_conf_cert_profile(&tls->conf, crt_profile);
	mbedtls_ssl_conf_sig_algs(&tls->conf, tls_sig_algs_for_suiteb);
}
#endif

static uint16_t tls_sig_algs_for_eap[] = {
#if defined(MBEDTLS_SHA512_C)
#if defined(MBEDTLS_ECDSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA512 ),
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA384 ),
#endif
#if defined(MBEDTLS_RSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA512 ),
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA384 ),
#endif
#endif /* MBEDTLS_SHA512_C */
#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_ECDSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA256 ),
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA224 ),
#endif
#if defined(MBEDTLS_RSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA256 ),
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA224 ),
#endif
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA1_C)
#if defined(MBEDTLS_ECDSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_ECDSA, MBEDTLS_SSL_HASH_SHA1 ),
#endif
#if defined(MBEDTLS_RSA_C)
    MBEDTLS_SSL_TLS12_SIG_AND_HASH_ALG( MBEDTLS_SSL_SIG_RSA, MBEDTLS_SSL_HASH_SHA1 ),
#endif
#endif /* MBEDTLS_SHA1_C */
    MBEDTLS_TLS_SIG_NONE
};

const mbedtls_x509_crt_profile eap_mbedtls_x509_crt_profile =
{
#if defined(MBEDTLS_SHA1_C)
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
#endif
#if defined(MBEDTLS_SHA256_C)
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
#endif
#if defined(MBEDTLS_SHA512_C)
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
	MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ) |
#endif
	0,
	0xFFFFFFF, /* Any PK alg    */
	0xFFFFFFF, /* Any curve     */
	1024,
};

static void tls_enable_sha1_config(tls_context_t *tls)
{
	const mbedtls_x509_crt_profile *crt_profile = &eap_mbedtls_x509_crt_profile;
	mbedtls_ssl_conf_cert_profile(&tls->conf, crt_profile);
	mbedtls_ssl_conf_sig_algs(&tls->conf, tls_sig_algs_for_eap);
}
#ifdef CONFIG_ESP_WIFI_DISABLE_KEY_USAGE_CHECK
static int tls_disable_key_usages(void *data, mbedtls_x509_crt *cert, int depth, uint32_t *flags)
{
	cert->MBEDTLS_PRIVATE(ext_types) &= ~MBEDTLS_X509_EXT_KEY_USAGE;
	cert->MBEDTLS_PRIVATE(ext_types) &= ~MBEDTLS_X509_EXT_EXTENDED_KEY_USAGE;
	return 0;
}
#endif /*CONFIG_ESP_WIFI_DISABLE_KEY_USAGE_CHECK*/

static const int eap_ciphersuite_preference[] =
{
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED)
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256,
	MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA,
#endif

#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CCM_8,
#endif
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED)
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CCM,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CCM_8,

	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CCM,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CCM_8,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_RSA_WITH_AES_256_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256,
	MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_RSA_WITH_AES_256_CCM_8,
#endif

#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_RSA_WITH_AES_128_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA,
#endif
#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA,
#endif
#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_RSA_WITH_AES_128_CCM_8,
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_RSA_PSK_WITH_AES_128_GCM_SHA256,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA,
#endif
	/* The PSK suites */
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_256_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_256_CCM_8,
#endif

#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_128_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8,
#endif
#endif
	0
};

#ifdef CONFIG_SUITEB192
static const int suiteb_rsa_ciphersuite_preference[] =
{
#if defined(MBEDTLS_GCM_C)
#if defined(MBEDTLS_SHA512_C)
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
#endif
#endif
	0
};

static const int suiteb_ecc_ciphersuite_preference[] =
{
#if defined(MBEDTLS_GCM_C)
#if defined(MBEDTLS_SHA512_C)
	MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
#endif
#endif
	0
};
static const int suiteb_ciphersuite_preference[] =
{
#if defined(MBEDTLS_GCM_C)
#if defined(MBEDTLS_SHA512_C)
	MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
	MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
#endif
#endif
	0
};
#endif

static void tls_set_ciphersuite(const struct tls_connection_params *cfg, tls_context_t *tls)
{
	/* Only set ciphersuite if cert's key length is high or ciphersuites are set by user */
#ifdef CONFIG_SUITEB192
	if (cfg->flags & TLS_CONN_SUITEB) {
		/* cipher suites will be set based on certificate */
		mbedtls_pk_type_t pk_alg = mbedtls_pk_get_type(&tls->clientkey);
		if (pk_alg == MBEDTLS_PK_RSA || pk_alg == MBEDTLS_PK_RSASSA_PSS) {
			mbedtls_ssl_conf_ciphersuites(&tls->conf,
						      suiteb_rsa_ciphersuite_preference);
		} else if (pk_alg == MBEDTLS_PK_ECDSA ||
			   pk_alg == MBEDTLS_PK_ECKEY ||
			   pk_alg == MBEDTLS_PK_ECKEY_DH) {
			mbedtls_ssl_conf_ciphersuites(&tls->conf,
						      suiteb_ecc_ciphersuite_preference);
		} else {
			mbedtls_ssl_conf_ciphersuites(&tls->conf,
						      suiteb_ciphersuite_preference);
		}
	} else
#endif
	if (tls->ciphersuite[0]) {
		mbedtls_ssl_conf_ciphersuites(&tls->conf, tls->ciphersuite);
	} else if (mbedtls_pk_get_bitlen(&tls->clientkey) > 2048 ||
		(tls->cacert_ptr && mbedtls_pk_get_bitlen(&tls->cacert_ptr->pk) > 2048)) {
		mbedtls_ssl_conf_ciphersuites(&tls->conf, eap_ciphersuite_preference);
	}
}

static int set_client_config(const struct tls_connection_params *cfg, tls_context_t *tls)
{
	int ret;
	int preset = MBEDTLS_SSL_PRESET_DEFAULT;
	assert(cfg != NULL);
	assert(tls != NULL);

#ifdef CONFIG_SUITEB192
	if (cfg->flags & TLS_CONN_SUITEB)
		preset = MBEDTLS_SSL_PRESET_SUITEB;
#endif
	ret = mbedtls_ssl_config_defaults(&tls->conf,
					MBEDTLS_SSL_IS_CLIENT,
					MBEDTLS_SSL_TRANSPORT_STREAM,
					preset);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_ssl_config_defaults returned -0x%x", -ret);
		return ret;
	}

	if (preset != MBEDTLS_SSL_PRESET_SUITEB) {
		/* Enable SHA1 support since it's not enabled by default in mbedtls */
		tls_enable_sha1_config(tls);
#ifdef CONFIG_SUITEB192
	} else {
		tls_set_suiteb_config(tls);
#endif
	}

	if (cfg->ca_cert_blob != NULL) {
		ret = set_ca_cert(tls, cfg->ca_cert_blob, cfg->ca_cert_blob_len);
		if (ret != 0) {
			return ret;
		}
	} else {
		mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
	}

	if (cfg->client_cert_blob != NULL && cfg->private_key_blob != NULL) {
		ret = set_pki_context(tls, cfg);
		if (ret != 0) {
			wpa_printf(MSG_ERROR, "Failed to set client pki context");
			return ret;
		}
	}

	/* Usages of default ciphersuites can take a lot of time on low end device
	 * and can cause watchdog. Enabling the ciphers which are secured enough
	 * but doesn't take that much processing power */
	tls_set_ciphersuite(cfg, tls);

#ifdef CONFIG_ESP_WIFI_DISABLE_KEY_USAGE_CHECK
	mbedtls_ssl_set_verify( &tls->ssl, tls_disable_key_usages, NULL );
#endif /*CONFIG_ESP_WIFI_DISABLE_KEY_USAGE_CHECK*/

#ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
	if (cfg->flags & TLS_CONN_USE_DEFAULT_CERT_BUNDLE) {
		wpa_printf(MSG_INFO, "Using default cert bundle");
		if (esp_crt_bundle_attach_fn) {
			ret = (*esp_crt_bundle_attach_fn)(&tls->conf);
		}
		if (ret != 0) {
			wpa_printf(MSG_ERROR, "Failed to set default cert bundle");
			return ret;
		}
	}
#endif

	return 0;
}

static void  tls_key_derivation(void *ctx,
				mbedtls_ssl_key_export_type secret_type,
				const unsigned char *secret,
				size_t secret_len,
				const unsigned char client_random[TLS_RANDOM_LEN],
				const unsigned char server_random[TLS_RANDOM_LEN],
				mbedtls_tls_prf_types tls_prf_type)
{
	struct tls_connection *conn = (struct tls_connection *)ctx;

	os_memcpy(conn->master_secret, secret, sizeof(conn->master_secret));
	os_memcpy(conn->randbytes, client_random, TLS_RANDOM_LEN);
	os_memcpy(conn->randbytes + 32, server_random, TLS_RANDOM_LEN);
	conn->tls_prf_type = tls_prf_type;
}

static int tls_create_mbedtls_handle(struct tls_connection *conn,
				     const struct tls_connection_params *params,
				     tls_context_t *tls)
{
	int ret;

	assert(params != NULL);
	assert(tls != NULL);

	mbedtls_ssl_init(&tls->ssl);
	mbedtls_ctr_drbg_init(&tls->ctr_drbg);
	mbedtls_ssl_config_init(&tls->conf);
	mbedtls_entropy_init(&tls->entropy);

	ret = set_client_config(params, tls);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "Failed to set client configurations");
		goto exit;
	}

	ret = mbedtls_ctr_drbg_seed(&tls->ctr_drbg, mbedtls_entropy_func,
				    &tls->entropy, NULL, 0);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_ctr_drbg_seed returned -0x%x", -ret);
		goto exit;
	}

	mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);

	ret = mbedtls_ssl_setup(&tls->ssl, &tls->conf);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_ssl_setup returned -0x%x", -ret);
		goto exit;
	}
	mbedtls_ssl_set_export_keys_cb(&tls->ssl, tls_key_derivation, conn);
#if defined(MBEDTLS_SSL_CBC_RECORD_SPLITTING)
	/* Disable BEAST attack countermeasures for Windows 2008 interoperability */
	mbedtls_ssl_conf_cbc_record_splitting(&tls->conf, MBEDTLS_SSL_CBC_RECORD_SPLITTING_DISABLED);
#endif

	/* Enable debug prints in case supplicant's prints are enabled */
#if defined(CONFIG_MBEDTLS_DEBUG) && defined(ESPRESSIF_USE)
	mbedtls_esp_enable_debug_log(&tls->conf, 2);
#endif
	return 0;

exit:
	tls_mbedtls_cleanup(tls);
	return ret;
}

void *tls_init(const struct tls_config *conf)
{
	tls_instance_count++;
	return &tls_instance_count;
}

void tls_deinit(void *tls_ctx)
{
	tls_instance_count--;
}

struct tls_connection * tls_connection_init(void *tls_ctx)
{
	struct tls_connection *conn = os_zalloc(sizeof(*conn));
	if (!conn) {
		wpa_printf(MSG_ERROR, "TLS: Failed to allocate connection memory");
		return NULL;
	}
	return conn;
}


void tls_connection_deinit(void *tls_ctx, struct tls_connection *conn)
{
	/* case: tls init failed */
	if (!conn) {
		return;
	}
	/* Free ssl ctx and data */
	tls_mbedtls_conn_delete((tls_context_t *) conn->tls);
	os_free(conn->tls);
	conn->tls = NULL;
	/* Data in in ssl ctx, free connection */
	os_free(conn);
}

int tls_get_errors(void *tls_ctx)
{
	return 0;
}

int tls_connection_established(void *tls_ctx, struct tls_connection *conn)
{
	mbedtls_ssl_context *ssl = &conn->tls->ssl;

	return mbedtls_ssl_is_handshake_over(ssl);
}

int tls_global_set_verify(void *tls_ctx, int check_crl, int strict)
{
	wpa_printf(MSG_INFO, "TLS: global settings are not supported");
	return -1;
}

int tls_connection_set_verify(void *tls_ctx, struct tls_connection *conn,
			      int verify_peer, unsigned int flags,
			      const u8 *session_ctx, size_t session_ctx_len)
{
	wpa_printf(MSG_INFO, "TLS: tls_connection_set_verify not supported");
	return -1;
}

#ifdef CONFIG_ESP_WIFI_ENT_FREE_DYNAMIC_BUFFER
static void esp_mbedtls_free_dhm(mbedtls_ssl_context *ssl)
{
#ifdef CONFIG_MBEDTLS_DHM_C
	const mbedtls_ssl_config *conf = mbedtls_ssl_context_get_config(ssl);
	mbedtls_mpi_free((mbedtls_mpi *)&conf->MBEDTLS_PRIVATE(dhm_P));
	mbedtls_mpi_free((mbedtls_mpi *)&conf->MBEDTLS_PRIVATE(dhm_G));
#endif /* CONFIG_MBEDTLS_DHM_C */
}

static void esp_mbedtls_free_keycert(mbedtls_ssl_context *ssl)
{
	mbedtls_ssl_config *conf = (mbedtls_ssl_config * )mbedtls_ssl_context_get_config(ssl);
	mbedtls_ssl_key_cert *keycert = conf->MBEDTLS_PRIVATE(key_cert), *next;

	while (keycert) {
		next = keycert->next;

		if (keycert) {
			mbedtls_free(keycert);
		}

		keycert = next;
	}

	conf->MBEDTLS_PRIVATE(key_cert) = NULL;
}

static void esp_mbedtls_free_keycert_key(mbedtls_ssl_context *ssl)
{
	const mbedtls_ssl_config *conf = mbedtls_ssl_context_get_config(ssl);
	mbedtls_ssl_key_cert *keycert = conf->MBEDTLS_PRIVATE(key_cert);

	while (keycert) {
		if (keycert->key) {
			mbedtls_pk_free(keycert->key);
			keycert->key = NULL;
		}
		keycert = keycert->next;
	}
}

static void esp_mbedtls_free_cacert(mbedtls_ssl_context *ssl)
{
	if (ssl->MBEDTLS_PRIVATE(conf)->MBEDTLS_PRIVATE(ca_chain)) {
		mbedtls_ssl_config *conf = (mbedtls_ssl_config * )mbedtls_ssl_context_get_config(ssl);

		mbedtls_x509_crt_free(conf->MBEDTLS_PRIVATE(ca_chain));
		conf->MBEDTLS_PRIVATE(ca_chain) = NULL;
	}
}
#endif

struct wpabuf * tls_connection_handshake(void *tls_ctx,
					 struct tls_connection *conn,
					 const struct wpabuf *in_data,
					 struct wpabuf **appl_data)
{
	tls_context_t *tls = conn->tls;
	int ret = 0;
	struct wpabuf *resp;
	int cli_state;

	/* data freed by sender */
	conn->tls_io_data.out_data = NULL;
	if (wpabuf_len(in_data)) {
		conn->tls_io_data.in_data = wpabuf_dup(in_data);
	}

	/* Multiple reads */
	while (!mbedtls_ssl_is_handshake_over(&tls->ssl)) {
		cli_state = tls->ssl.MBEDTLS_PRIVATE(state);
		if (cli_state == MBEDTLS_SSL_CLIENT_CERTIFICATE) {
			/* Read random data before session completes, not present after handshake */
			if (tls->ssl.MBEDTLS_PRIVATE(handshake)) {
				os_memcpy(conn->randbytes, tls->ssl.MBEDTLS_PRIVATE(handshake)->randbytes,
					  TLS_RANDOM_LEN * 2);
				conn->mac = tls->ssl.MBEDTLS_PRIVATE(handshake)->ciphersuite_info->mac;
			}
		}
		ret = mbedtls_ssl_handshake_step(&tls->ssl);

		if (ret < 0) {
			break;
		}
#ifdef CONFIG_ESP_WIFI_ENT_FREE_DYNAMIC_BUFFER
		if (mbedtls_ssl_get_version_number(&tls->ssl) == MBEDTLS_SSL_VERSION_TLS1_2) {
			if (cli_state == MBEDTLS_SSL_SERVER_CERTIFICATE) {
				esp_mbedtls_free_cacert(&tls->ssl);
			} else if (cli_state == MBEDTLS_SSL_CERTIFICATE_VERIFY) {
				esp_mbedtls_free_dhm(&tls->ssl);
				esp_mbedtls_free_keycert_key(&tls->ssl);
				esp_mbedtls_free_keycert(&tls->ssl);
			}
		}
#endif
	}
	if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ) {
		wpa_printf(MSG_INFO, "%s: ret is %d line:%d", __func__, ret, __LINE__);
		goto end;
	}

	if (!conn->tls_io_data.out_data) {
		wpa_printf(MSG_INFO, "application data is null, adding one byte for ack");
		u8 *dummy = os_zalloc(1);
		conn->tls_io_data.out_data = wpabuf_alloc_ext_data(dummy, 0);
	}

end:
	resp = conn->tls_io_data.out_data;
	conn->tls_io_data.out_data = NULL;
	return resp;
}

struct wpabuf * tls_connection_server_handshake(void *tls_ctx,
						struct tls_connection *conn,
						const struct wpabuf *in_data,
						struct wpabuf **appl_data)
{
	wpa_printf(MSG_ERROR, "%s: not supported %d", __func__, __LINE__);
	return NULL;
}


struct wpabuf * tls_connection_encrypt(void *tls_ctx,
				       struct tls_connection *conn,
				       const struct wpabuf *in_data)
{
	struct wpabuf *resp;
	size_t ret;

	/* Reset dangling pointer */
	conn->tls_io_data.out_data = NULL;
	ret = mbedtls_ssl_write(&conn->tls->ssl,
			(unsigned char*) wpabuf_head(in_data),  wpabuf_len(in_data));

	if (ret < wpabuf_len(in_data)) {
		wpa_printf(MSG_ERROR, "%s:%d, not able to write whole data",
			   __func__, __LINE__);
	}

	resp = conn->tls_io_data.out_data;
	conn->tls_io_data.out_data = NULL;
	return resp;
}


struct wpabuf *tls_connection_decrypt(void *tls_ctx,
				      struct tls_connection *conn,
				      const struct wpabuf *in_data)
{
#define MAX_PHASE2_BUFFER 1536
	struct wpabuf *out = NULL;
	int ret;
	unsigned char *buf = os_malloc(MAX_PHASE2_BUFFER);

	if (!buf) {
		return NULL;
	}
	/* Reset dangling output buffer before setting data, data was freed by caller */
	conn->tls_io_data.out_data = NULL;

	conn->tls_io_data.in_data = wpabuf_dup(in_data);

	if (!conn->tls_io_data.in_data) {
		goto cleanup;
	}
	ret = mbedtls_ssl_read(&conn->tls->ssl, buf, MAX_PHASE2_BUFFER);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "%s:%d, not able to read data",
				__func__, __LINE__);
		goto cleanup;
	}
	out = wpabuf_alloc_copy(buf, ret);
cleanup:
	/* there may be some error written in output buffer */
	if (conn->tls_io_data.out_data) {
		os_free(conn->tls_io_data.out_data);
		conn->tls_io_data.out_data = NULL;
	}

	os_free(buf);

	return out;
#undef MAX_PHASE2_BUFFER
}


int tls_connection_resumed(void *tls_ctx, struct tls_connection *conn)
{
	if (conn && conn->tls && conn->tls->ssl.MBEDTLS_PRIVATE(handshake)) {
		return conn->tls->ssl.MBEDTLS_PRIVATE(handshake)->resume;
	}

	return 0;
}

/* cipher array should contain cipher number in mbedtls num as per IANA
 * Please see cipherlist is u8, therefore only initial ones are supported */
int tls_connection_set_cipher_list(void *tls_ctx, struct tls_connection *conn,
				   u8 *ciphers)
{
	int i = 0;

	while (*ciphers != 0 && i < MAX_CIPHERSUITE) {
		conn->tls->ciphersuite[i] = ciphers[i];
		i++;
	}
	return 0;
}

int tls_get_version(void *tls_ctx, struct tls_connection *conn,
		    char *buf, size_t buflen)
{
	const char *name;

	if (conn == NULL) {
		return -1;
	}

	name = mbedtls_ssl_get_version(&conn->tls->ssl);
	if (name == NULL) {
		return -1;
	}

	os_strlcpy(buf, name, buflen);

	return 0;
}

int tls_get_cipher(void *tls_ctx, struct tls_connection *conn,
		   char *buf, size_t buflen)
{
	const char *name;
	if (conn == NULL) {
		return -1;
	}

	name = mbedtls_ssl_get_ciphersuite(&conn->tls->ssl);
	if (name == NULL) {
		return -1;
	}

	os_strlcpy(buf, name, buflen);

	return 0;
}


int tls_connection_enable_workaround(void *tls_ctx,
				     struct tls_connection *conn)
{
	wpa_printf(MSG_ERROR, "%s: not supported %d", __func__, __LINE__);
	return -1;
}

int tls_connection_get_failed(void *tls_ctx, struct tls_connection *conn)
{
	return 0;
}

int tls_connection_get_read_alerts(void *tls_ctx, struct tls_connection *conn)
{
	wpa_printf(MSG_ERROR, "%s: not supported %d", __func__, __LINE__);
	return 0;
}

int tls_connection_get_write_alerts(void *tls_ctx, struct tls_connection *conn)
{
	wpa_printf(MSG_ERROR, "%s: not supported %d", __func__, __LINE__);
	return 0;
}

int tls_connection_set_params(void *tls_ctx, struct tls_connection *conn,
			      const struct tls_connection_params *params)
{
	int ret = 0;
	tls_context_t *tls = (tls_context_t *)os_zalloc(sizeof(tls_context_t));

	if (!tls) {
		wpa_printf(MSG_ERROR, "failed to allocate tls context");
		return -1;
	}
	if (!params) {
		wpa_printf(MSG_ERROR, "configuration is null");
		ret = -1;
		goto err;
	}

	ret = tls_create_mbedtls_handle(conn, params, tls);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "failed to create ssl handle");
		goto err;
	}
	mbedtls_ssl_set_bio(&tls->ssl, conn, tls_mbedtls_write, tls_mbedtls_read, NULL);
	conn->tls = (tls_context_t *)tls;

	return ret;
err:
	os_free(tls);
	return ret;
}

int tls_global_set_params(void *tls_ctx,
			  const struct tls_connection_params *params)
{
	wpa_printf(MSG_INFO, "TLS: Global parameters not supported");
	return -1;
}

int tls_connection_set_session_ticket_cb(void *tls_ctx,
					 struct tls_connection *conn,
					 tls_session_ticket_cb cb,
					 void *ctx)
{
	wpa_printf(MSG_ERROR, "TLS: %s not supported", __func__);
	return -1;
}

static int tls_connection_prf(void *tls_ctx, struct tls_connection *conn,
		       const char *label, int server_random_first,
		       u8 *out, size_t out_len)
{
	int ret;
	u8 seed[2 * TLS_RANDOM_LEN];
	mbedtls_ssl_context *ssl = &conn->tls->ssl;

	if (!ssl) {
		wpa_printf(MSG_ERROR, "TLS: %s, session ingo is null", __func__);
		return -1;
	}
	if (!mbedtls_ssl_is_handshake_over(ssl)) {
		wpa_printf(MSG_ERROR, "TLS: %s, incorrect tls state=%d", __func__, ssl->MBEDTLS_PRIVATE(state));
		return -1;
	}

	if (server_random_first) {
		os_memcpy(seed, conn->randbytes + TLS_RANDOM_LEN, TLS_RANDOM_LEN);
		os_memcpy(seed + TLS_RANDOM_LEN, conn->randbytes, TLS_RANDOM_LEN);
	} else {
		os_memcpy(seed, conn->randbytes, 2 * TLS_RANDOM_LEN);
	}

	wpa_hexdump_key(MSG_MSGDUMP, "random", seed, 2 * TLS_RANDOM_LEN);
	wpa_hexdump_key(MSG_MSGDUMP, "master",  ssl->MBEDTLS_PRIVATE(session)->MBEDTLS_PRIVATE(master), TLS_MASTER_SECRET_LEN);

	ret = mbedtls_ssl_tls_prf(conn->tls_prf_type, conn->master_secret, TLS_MASTER_SECRET_LEN,
				label, seed, 2 * TLS_RANDOM_LEN, out, out_len);

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "prf failed, ret=%d", ret);
	}
	wpa_hexdump_key(MSG_MSGDUMP, "key", out, out_len);

	return ret;
}

int tls_connection_export_key(void *tls_ctx, struct tls_connection *conn,
			      const char *label, const u8 *context,
			      size_t context_len, u8 *out, size_t out_len)
{
	return tls_connection_prf(tls_ctx, conn, label, 0, out, out_len);
}

int tls_connection_get_eap_fast_key(void *tls_ctx, struct tls_connection *conn,
				    u8 *out, size_t out_len)
{
	wpa_printf(MSG_INFO, "TLS: tls_connection_get_eap_fast_key not supported, please unset mbedtls crypto and try again");
	return -1;
}

int tls_connection_client_hello_ext(void *tls_ctx, struct tls_connection *conn,
				    int ext_type, const u8 *data,
				    size_t data_len)
{
	wpa_printf(MSG_INFO, "TLS: tls_connection_client_hello_ext not supported, please unset mbedtls crypto and try again");
	return -1;
}

int tls_connection_shutdown(void *tls_ctx, struct tls_connection *conn)
{
	if (conn->tls_io_data.in_data) {
		wpabuf_free(conn->tls_io_data.in_data);
	}
	conn->tls_io_data.in_data = NULL;

	/* outdata may have dangling pointer */
	conn->tls_io_data.out_data = NULL;

	return mbedtls_ssl_session_reset(&conn->tls->ssl);
}

int tls_connection_get_random(void *tls_ctx, struct tls_connection *conn,
			    struct tls_random *data)
{
	mbedtls_ssl_context *ssl = &conn->tls->ssl;

	os_memset(data, 0, sizeof(*data));
	if (ssl->MBEDTLS_PRIVATE(state) == MBEDTLS_SSL_CLIENT_HELLO) {
		return -1;
	}

	data->client_random = conn->randbytes;
	data->client_random_len = TLS_RANDOM_LEN;

	if (ssl->MBEDTLS_PRIVATE(state) != MBEDTLS_SSL_SERVER_HELLO) {
		data->server_random = conn->randbytes + TLS_RANDOM_LEN;
		data->server_random_len = TLS_RANDOM_LEN;
	}

	return 0;
}
