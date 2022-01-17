/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "esp_mbedtls_dynamic_impl.h"

int __real_mbedtls_ssl_write(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);
int __real_mbedtls_ssl_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);
void __real_mbedtls_ssl_free(mbedtls_ssl_context *ssl);
int __real_mbedtls_ssl_session_reset(mbedtls_ssl_context *ssl);
int __real_mbedtls_ssl_setup(mbedtls_ssl_context *ssl, const mbedtls_ssl_config *conf);
int __real_mbedtls_ssl_send_alert_message(mbedtls_ssl_context *ssl, unsigned char level, unsigned char message);
int __real_mbedtls_ssl_close_notify(mbedtls_ssl_context *ssl);

int __wrap_mbedtls_ssl_write(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);
int __wrap_mbedtls_ssl_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len);
void __wrap_mbedtls_ssl_free(mbedtls_ssl_context *ssl);
int __wrap_mbedtls_ssl_session_reset(mbedtls_ssl_context *ssl);
int __wrap_mbedtls_ssl_setup(mbedtls_ssl_context *ssl, const mbedtls_ssl_config *conf);
int __wrap_mbedtls_ssl_send_alert_message(mbedtls_ssl_context *ssl, unsigned char level, unsigned char message);
int __wrap_mbedtls_ssl_close_notify(mbedtls_ssl_context *ssl);

static const char *TAG = "SSL TLS";

static int tx_done(mbedtls_ssl_context *ssl)
{
    if (!ssl->out_left)
        return 1;

    return 0;
}

static int rx_done(mbedtls_ssl_context *ssl)
{
    if (!ssl->in_msglen) {
        return 1;
    }

    ESP_LOGD(TAG, "RX left %zu bytes", ssl->in_msglen);

    return 0;
}

static void ssl_update_checksum_start( mbedtls_ssl_context *ssl,
                                       const unsigned char *buf, size_t len )
{
#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
    mbedtls_md5_update_ret( &ssl->handshake->fin_md5 , buf, len );
    mbedtls_sha1_update_ret( &ssl->handshake->fin_sha1, buf, len );
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_SHA256_C)
    mbedtls_sha256_update_ret( &ssl->handshake->fin_sha256, buf, len );
#endif
#if defined(MBEDTLS_SHA512_C)
    mbedtls_sha512_update_ret( &ssl->handshake->fin_sha512, buf, len );
#endif
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
}

static void ssl_handshake_params_init( mbedtls_ssl_handshake_params *handshake )
{
    memset( handshake, 0, sizeof( mbedtls_ssl_handshake_params ) );

#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1) || \
    defined(MBEDTLS_SSL_PROTO_TLS1_1)
    mbedtls_md5_init(   &handshake->fin_md5  );
    mbedtls_sha1_init(   &handshake->fin_sha1 );
    mbedtls_md5_starts_ret( &handshake->fin_md5  );
    mbedtls_sha1_starts_ret( &handshake->fin_sha1 );
#endif
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_SHA256_C)
    mbedtls_sha256_init(   &handshake->fin_sha256    );
    mbedtls_sha256_starts_ret( &handshake->fin_sha256, 0 );
#endif
#if defined(MBEDTLS_SHA512_C)
    mbedtls_sha512_init(   &handshake->fin_sha512    );
    mbedtls_sha512_starts_ret( &handshake->fin_sha512, 1 );
#endif
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

    handshake->update_checksum = ssl_update_checksum_start;

#if defined(MBEDTLS_SSL_PROTO_TLS1_2) && \
    defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
    mbedtls_ssl_sig_hash_set_init( &handshake->hash_algs );
#endif

#if defined(MBEDTLS_DHM_C)
    mbedtls_dhm_init( &handshake->dhm_ctx );
#endif
#if defined(MBEDTLS_ECDH_C)
    mbedtls_ecdh_init( &handshake->ecdh_ctx );
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    mbedtls_ecjpake_init( &handshake->ecjpake_ctx );
#if defined(MBEDTLS_SSL_CLI_C)
    handshake->ecjpake_cache = NULL;
    handshake->ecjpake_cache_len = 0;
#endif
#endif

#if defined(MBEDTLS_SSL_ECP_RESTARTABLE)
    mbedtls_x509_crt_restart_init( &handshake->ecrs_ctx );
#endif

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    handshake->sni_authmode = MBEDTLS_SSL_VERIFY_UNSET;
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C) && \
    !defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    mbedtls_pk_init( &handshake->peer_pubkey );
#endif
}

static int ssl_handshake_init( mbedtls_ssl_context *ssl )
{
    /* Clear old handshake information if present */
    if( ssl->transform_negotiate )
        mbedtls_ssl_transform_free( ssl->transform_negotiate );
    if( ssl->session_negotiate )
        mbedtls_ssl_session_free( ssl->session_negotiate );
    if( ssl->handshake )
        mbedtls_ssl_handshake_free( ssl );

    /*
     * Either the pointers are now NULL or cleared properly and can be freed.
     * Now allocate missing structures.
     */
    if( ssl->transform_negotiate == NULL )
    {
        ssl->transform_negotiate = mbedtls_calloc( 1, sizeof(mbedtls_ssl_transform) );
    }

    if( ssl->session_negotiate == NULL )
    {
        ssl->session_negotiate = mbedtls_calloc( 1, sizeof(mbedtls_ssl_session) );
    }

    if( ssl->handshake == NULL )
    {
        ssl->handshake = mbedtls_calloc( 1, sizeof(mbedtls_ssl_handshake_params) );
    }

    /* All pointers should exist and can be directly freed without issue */
    if( ssl->handshake == NULL ||
        ssl->transform_negotiate == NULL ||
        ssl->session_negotiate == NULL )
    {
        ESP_LOGD(TAG, "alloc() of ssl sub-contexts failed");

        mbedtls_free( ssl->handshake );
        mbedtls_free( ssl->transform_negotiate );
        mbedtls_free( ssl->session_negotiate );

        ssl->handshake = NULL;
        ssl->transform_negotiate = NULL;
        ssl->session_negotiate = NULL;

        return( MBEDTLS_ERR_SSL_ALLOC_FAILED );
    }

    /* Initialize structures */
    mbedtls_ssl_session_init( ssl->session_negotiate );
    mbedtls_ssl_transform_init( ssl->transform_negotiate );
    ssl_handshake_params_init( ssl->handshake );

    return( 0 );
}

int __wrap_mbedtls_ssl_setup(mbedtls_ssl_context *ssl, const mbedtls_ssl_config *conf)
{
    ssl->conf = conf;
    CHECK_OK(ssl_handshake_init(ssl));

    ssl->out_buf = NULL;
    CHECK_OK(esp_mbedtls_setup_tx_buffer(ssl));

    ssl->in_buf = NULL;
    esp_mbedtls_setup_rx_buffer(ssl);

    return 0;
}

int __wrap_mbedtls_ssl_write(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
    int ret;

    CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, 0));

    ret = __real_mbedtls_ssl_write(ssl, buf, len);

    if (tx_done(ssl)) {
        CHECK_OK(esp_mbedtls_free_tx_buffer(ssl));
    }

    return ret;
}

int __wrap_mbedtls_ssl_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
    int ret;

    ESP_LOGD(TAG, "add mbedtls RX buffer");
    ret = esp_mbedtls_add_rx_buffer(ssl);
    if (ret == MBEDTLS_ERR_SSL_CONN_EOF) {
        ESP_LOGD(TAG, "fail, the connection indicated an EOF");
        return 0;
    } else if (ret < 0) {
        ESP_LOGD(TAG, "fail, error=%d", -ret);
        return ret;
    }
    ESP_LOGD(TAG, "end");

    ret = __real_mbedtls_ssl_read(ssl, buf, len);

    if (rx_done(ssl)) {
        CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
    }

    return ret;
}

void __wrap_mbedtls_ssl_free(mbedtls_ssl_context *ssl)
{
    if (ssl->out_buf) {
        esp_mbedtls_free_buf(ssl->out_buf);
        ssl->out_buf = NULL;
    }

    if (ssl->in_buf) {
        esp_mbedtls_free_buf(ssl->in_buf);
        ssl->in_buf = NULL;
    }

    __real_mbedtls_ssl_free(ssl);
}

int __wrap_mbedtls_ssl_session_reset(mbedtls_ssl_context *ssl)
{
    CHECK_OK(esp_mbedtls_reset_add_tx_buffer(ssl));

    CHECK_OK(esp_mbedtls_reset_add_rx_buffer(ssl));

    CHECK_OK(__real_mbedtls_ssl_session_reset(ssl));

    CHECK_OK(esp_mbedtls_reset_free_tx_buffer(ssl));

    esp_mbedtls_reset_free_rx_buffer(ssl);

    return 0;
}

int __wrap_mbedtls_ssl_send_alert_message(mbedtls_ssl_context *ssl, unsigned char level, unsigned char message)
{
    int ret;

    CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, 0));

    ret = __real_mbedtls_ssl_send_alert_message(ssl, level, message);

    if (tx_done(ssl)) {
        CHECK_OK(esp_mbedtls_free_tx_buffer(ssl));
    }

    return ret;
}

int __wrap_mbedtls_ssl_close_notify(mbedtls_ssl_context *ssl)
{
    int ret;

    CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, 0));

    ret = __real_mbedtls_ssl_close_notify(ssl);

    if (tx_done(ssl)) {
        CHECK_OK(esp_mbedtls_free_tx_buffer(ssl));
    }

    return ret;
}
