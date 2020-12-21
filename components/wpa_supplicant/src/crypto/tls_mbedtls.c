/**
 * Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "utils/includes.h"
#include "utils/common.h"

#include "tls/tls.h"
#include "crypto/sha1.h"
#include "crypto/md5.h"
#include "crypto/sha256.h"
#include "crypto/sha384.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/debug.h"
#ifdef ESPRESSIF_USE
#include "mbedtls/esp_debug.h"
#include "mbedtls/esp_config.h"
#else
#include "mbedtls/config.h"
#endif

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
	unsigned char randbytes[2 * TLS_RANDOM_LEN];
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

	if (data->out_data) {
		wpabuf_resize(&data->out_data, len);
	} else {
		data->out_data = wpabuf_alloc(len);
	}

	wpabuf_put_data(data->out_data, buf, len);

	return len;
}

static int tls_mbedtls_read(void *ctx, unsigned char *buf, size_t len)
{
	struct tls_connection *conn = (struct tls_connection *)ctx;
	struct tls_data *data = &conn->tls_io_data;
	struct wpabuf *local_buf;
	size_t data_len = len;

	if (len > wpabuf_len(data->in_data)) {
		wpa_printf(MSG_ERROR, "don't have suffient data\n");
		data_len = wpabuf_len(data->in_data);
	}

	os_memcpy(buf, wpabuf_head(data->in_data), data_len);
	/* adjust buffer */
	if (len < wpabuf_len(data->in_data)) {
		local_buf = wpabuf_alloc_copy(wpabuf_head(data->in_data) + len,
					      wpabuf_len(data->in_data) - len);
		wpabuf_free(data->in_data);
		data->in_data = local_buf;
	} else {
		wpabuf_free(data->in_data);
		data->in_data = NULL;
	}

	return data_len;
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
				   cfg->private_key_passwd ? os_strlen(cfg->private_key_passwd) : 0);
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

static int tls_sig_hashes_for_eap[] = {
#if defined(MBEDTLS_SHA512_C)
	MBEDTLS_MD_SHA512,
	MBEDTLS_MD_SHA384,
#endif
#if defined(MBEDTLS_SHA256_C)
	MBEDTLS_MD_SHA256,
	MBEDTLS_MD_SHA224,
#endif
#if defined(MBEDTLS_SHA1_C)
	MBEDTLS_MD_SHA1,
#endif
	MBEDTLS_MD_NONE
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
	mbedtls_ssl_conf_sig_hashes(&tls->conf, tls_sig_hashes_for_eap);
}

static const int eap_ciphersuite_preference[] =
{
#if defined(MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED)
#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
	MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
#endif
#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384,
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
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CCM,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CCM_8,

	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CCM,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256,
	MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CCM_8,
#endif
#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_RSA_WITH_AES_256_GCM_SHA384,
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
#if defined(MBEDTLS_GCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384,
#endif
#if defined(MBEDTLS_CCM_C)
	MBEDTLS_TLS_PSK_WITH_AES_256_CCM,
#endif
#if defined(MBEDTLS_CIPHER_MODE_CBC)
	MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA384,
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

#if defined(MBEDTLS_DES_C)
	/* 3DES suites */
	MBEDTLS_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA,
	MBEDTLS_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA,
	MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA,
	MBEDTLS_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA,
	MBEDTLS_TLS_PSK_WITH_3DES_EDE_CBC_SHA,
#endif
#if defined(MBEDTLS_ARC4_C)
	/* RC4 suites */
	MBEDTLS_TLS_DHE_PSK_WITH_RC4_128_SHA,
	MBEDTLS_TLS_RSA_WITH_RC4_128_SHA,
	MBEDTLS_TLS_RSA_WITH_RC4_128_MD5,
	MBEDTLS_TLS_RSA_PSK_WITH_RC4_128_SHA,
	MBEDTLS_TLS_PSK_WITH_RC4_128_SHA,
#endif
};

static void tls_set_ciphersuite(tls_context_t *tls)
{
	/* Only set ciphersuite if cert's key length is high or ciphersuites are set by user */
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
	assert(cfg != NULL);
	assert(tls != NULL);

	ret = mbedtls_ssl_config_defaults(&tls->conf,
					MBEDTLS_SSL_IS_CLIENT,
					MBEDTLS_SSL_TRANSPORT_STREAM,
					MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0) {
		wpa_printf(MSG_ERROR, "mbedtls_ssl_config_defaults returned -0x%x", -ret);
		return ret;
	}

	/* Enable SHA1 support since it's not enabled by default in mbedtls */
	tls_enable_sha1_config(tls);

	if (cfg->ca_cert_blob != NULL) {
		ret = set_ca_cert(tls, cfg->ca_cert_blob, cfg->ca_cert_blob_len);
		if (ret != 0) {
			return ret;
		}
		mbedtls_ssl_conf_ca_chain(&tls->conf, tls->cacert_ptr, NULL);
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
	tls_set_ciphersuite(tls);

	return 0;
}

static int tls_create_mbedtls_handle(const struct tls_connection_params *params,
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

	/* Enable debug prints in case supplicant's prints are enabled */
#if defined(DEBUG_PRINT) && defined(CONFIG_MBEDTLS_DEBUG) && defined(ESPRESSIF_USE)
	mbedtls_esp_enable_debug_log(&tls->conf, 2);
#endif
	return 0;

exit:
	tls_mbedtls_cleanup(tls);
	return ret;
}

void *tls_init()
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

	if (ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER) {
		return 1;
	}

	return 0;
}

int tls_global_set_verify(void *tls_ctx, int check_crl)
{
	wpa_printf(MSG_INFO, "TLS: global settings are not supported");
	return -1;
}

int tls_connection_set_verify(void *tls_ctx, struct tls_connection *conn,
			      int verify_peer)
{
	wpa_printf(MSG_INFO, "TLS: tls_connection_set_verify not supported");
	return -1;
}

struct wpabuf * tls_connection_handshake(void *tls_ctx,
					 struct tls_connection *conn,
					 const struct wpabuf *in_data,
					 struct wpabuf **appl_data)
{
	tls_context_t *tls = conn->tls;
	int ret = 0;

	/* data freed by sender */
	conn->tls_io_data.out_data = NULL;
	if (wpabuf_len(in_data)) {
		conn->tls_io_data.in_data = wpabuf_dup(in_data);
	}
	ret = mbedtls_ssl_handshake_step(&tls->ssl);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "%s:%d", __func__, __LINE__);
		goto end;
	}

	/* Multiple reads */
	while (conn->tls_io_data.in_data) {
		ret = mbedtls_ssl_handshake_step(&tls->ssl);
		if (ret < 0)
			break;
	}

	/* State machine just started, get client hello */
	if (tls->ssl.state == MBEDTLS_SSL_CLIENT_HELLO) {
		ret = mbedtls_ssl_handshake_step(&tls->ssl);
	}

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "%s:%d", __func__, __LINE__);
		goto end;
	}

	/* Already read sever data till hello done */
	if (tls->ssl.state == MBEDTLS_SSL_CLIENT_CERTIFICATE) {
		/* Read random data before session completes, not present after handshake */
		if (tls->ssl.handshake) {
			os_memcpy(conn->randbytes, tls->ssl.handshake->randbytes,
				  TLS_RANDOM_LEN * 2);
		}

		/* trigger state machine multiple times to reach till finish */
		while (tls->ssl.state <= MBEDTLS_SSL_CLIENT_FINISHED) {
			ret = mbedtls_ssl_handshake_step(&tls->ssl);
			if (ret < 0) {
				break;
			}
		}
	}

	/* Trigger state machine till handshake is complete or error occures */
	if (tls->ssl.state == MBEDTLS_SSL_FLUSH_BUFFERS) {
		while (tls->ssl.state <= MBEDTLS_SSL_HANDSHAKE_OVER) {
			ret = mbedtls_ssl_handshake_step(&tls->ssl);
			if (ret < 0) {
				break;
			}
		}
	}

	if (!conn->tls_io_data.out_data) {
		wpa_printf(MSG_INFO, "application data is null, adding one byte for ack");
		u8 *dummy = os_zalloc(1);
		conn->tls_io_data.out_data = wpabuf_alloc_ext_data(dummy, 0);
	}

end:
	return conn->tls_io_data.out_data;
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
	/* Reset dangling pointer */
	conn->tls_io_data.out_data = NULL;

	ssize_t ret = mbedtls_ssl_write(&conn->tls->ssl,
			(unsigned char*) wpabuf_head(in_data),  wpabuf_len(in_data));

	if (ret < wpabuf_len(in_data)) {
		wpa_printf(MSG_ERROR, "%s:%d, not able to write whole data",
			   __func__, __LINE__);
	}

	return conn->tls_io_data.out_data;
}


struct wpabuf * tls_connection_decrypt(void *tls_ctx,
		struct tls_connection *conn,
		const struct wpabuf *in_data)
{
	unsigned char buf[1200];
	int ret;
	conn->tls_io_data.in_data = wpabuf_dup(in_data);
	ret = mbedtls_ssl_read(&conn->tls->ssl, buf, 1200);
	if (ret < 0) {
		wpa_printf(MSG_ERROR, "%s:%d, not able to write whole data",
				__func__, __LINE__);
		return NULL;
	}

	struct wpabuf *out = wpabuf_alloc_copy(buf, ret);

	return out;
}


int tls_connection_resumed(void *tls_ctx, struct tls_connection *conn)
{
	if (conn && conn->tls && conn->tls->ssl.handshake) {
		return conn->tls->ssl.handshake->resume;
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

	ret = tls_create_mbedtls_handle(params, tls);
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
	mbedtls_ssl_transform *transform = ssl->transform;

	if (!ssl || !transform) {
		wpa_printf(MSG_ERROR, "TLS: %s, session ingo is null", __func__);
		return -1;
	}
	if (ssl->state != MBEDTLS_SSL_HANDSHAKE_OVER) {
		wpa_printf(MSG_ERROR, "TLS: %s, incorrect tls state=%d", __func__, ssl->state);
		return -1;
	}

	if (server_random_first) {
		os_memcpy(seed, conn->randbytes + TLS_RANDOM_LEN, TLS_RANDOM_LEN);
		os_memcpy(seed + TLS_RANDOM_LEN, conn->randbytes, TLS_RANDOM_LEN);
	} else {
		os_memcpy(seed, conn->randbytes, 2 * TLS_RANDOM_LEN);
	}

	wpa_hexdump_key(MSG_MSGDUMP, "random", seed, 2 * TLS_RANDOM_LEN);
	wpa_hexdump_key(MSG_MSGDUMP, "master", ssl->session->master, TLS_MASTER_SECRET_LEN);

	if (transform->ciphersuite_info->mac == MBEDTLS_MD_SHA384) {
		ret = tls_prf_sha384(ssl->session->master, TLS_MASTER_SECRET_LEN,
				label, seed, 2 * TLS_RANDOM_LEN, out, out_len);
	} else if (transform->ciphersuite_info->mac == MBEDTLS_MD_SHA256) {
		ret = tls_prf_sha256(ssl->session->master, TLS_MASTER_SECRET_LEN,
				label, seed, 2 * TLS_RANDOM_LEN, out, out_len);
	} else {
		ret = tls_prf_sha1_md5(ssl->session->master, TLS_MASTER_SECRET_LEN,
				label, seed, 2 * TLS_RANDOM_LEN, out, out_len);
	}

	if (ret < 0) {
		wpa_printf(MSG_ERROR, "prf failed, ret=%d\n", ret);
	}
	wpa_hexdump_key(MSG_MSGDUMP, "key", out, out_len);

	return ret;
}

int tls_connection_export_key(void *tls_ctx, struct tls_connection *conn,
			      const char *label, u8 *out, size_t out_len)
{
	return tls_connection_prf(tls_ctx, conn, label, 0, out, out_len);
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
	if (ssl->state == MBEDTLS_SSL_CLIENT_HELLO) {
		return -1;
	}

	data->client_random = conn->randbytes;
	data->client_random_len = TLS_RANDOM_LEN;

	if (ssl->state != MBEDTLS_SSL_SERVER_HELLO) {
		data->server_random = conn->randbytes + TLS_RANDOM_LEN;
		data->server_random_len = TLS_RANDOM_LEN;
	}

	return 0;
}
