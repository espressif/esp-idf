/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include "mbedtls/error.h"
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
    if (!ssl->MBEDTLS_PRIVATE(out_left))
        return 1;

    return 0;
}

static int rx_done(mbedtls_ssl_context *ssl)
{
    if (!ssl->MBEDTLS_PRIVATE(in_msglen)) {
        return 1;
    }

    ESP_LOGD(TAG, "RX left %zu bytes", ssl->MBEDTLS_PRIVATE(in_msglen));
    return 0;
}

static int ssl_update_checksum_start( mbedtls_ssl_context *ssl,
                                       const unsigned char *buf, size_t len )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
#if defined(MBEDTLS_SHA256_C)
    ret = mbedtls_md_update( &ssl->handshake->fin_sha256, buf, len );
#endif
#if defined(MBEDTLS_SHA512_C)
    ret = mbedtls_md_update( &ssl->handshake->fin_sha384, buf, len );
#endif
    return ret;
}

static int ssl_handshake_params_init( mbedtls_ssl_handshake_params *handshake )
{
    memset( handshake, 0, sizeof( mbedtls_ssl_handshake_params ) );

#if defined(MBEDTLS_SHA256_C)
    mbedtls_md_init( &handshake->fin_sha256 );
    int ret = mbedtls_md_setup( &handshake->fin_sha256,
                    mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
                    0 );
    if (ret != 0) {
        return ret;
    }
    ret = mbedtls_md_starts( &handshake->fin_sha256 );
    if (ret != 0) {
        return ret;
    }
#endif
#if defined(MBEDTLS_SHA512_C)
    mbedtls_md_init( &handshake->fin_sha384 );
    ret = mbedtls_md_setup( &handshake->fin_sha384,
                    mbedtls_md_info_from_type(MBEDTLS_MD_SHA384),
                    0 );
    if (ret != 0) {
        return ret;
    }
    ret = mbedtls_md_starts( &handshake->fin_sha384 );
    if (ret != 0) {
        return ret;
    }
#endif

    handshake->update_checksum = ssl_update_checksum_start;

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

    return 0;
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
#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    /* If the buffers are too small - reallocate */

    handle_buffer_resizing( ssl, 0, MBEDTLS_SSL_IN_BUFFER_LEN,
                                    MBEDTLS_SSL_OUT_BUFFER_LEN );
#endif

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
    int ret = ssl_handshake_params_init( ssl->handshake );
    if (ret != 0) {
        return ret;
    }

/*
 * curve_list is translated to IANA TLS group identifiers here because
 * mbedtls_ssl_conf_curves returns void and so can't return
 * any error codes.
 */
#if defined(MBEDTLS_ECP_C)
#if !defined(MBEDTLS_DEPRECATED_REMOVED)
    /* Heap allocate and translate curve_list from internal to IANA group ids */
    if ( ssl->conf->curve_list != NULL )
    {
        size_t length;
        const mbedtls_ecp_group_id *curve_list = ssl->conf->curve_list;

        for( length = 0;  ( curve_list[length] != MBEDTLS_ECP_DP_NONE ) &&
                          ( length < MBEDTLS_ECP_DP_MAX ); length++ ) {}

        /* Leave room for zero termination */
        uint16_t *group_list = mbedtls_calloc( length + 1, sizeof(uint16_t) );
        if ( group_list == NULL )
            return( MBEDTLS_ERR_SSL_ALLOC_FAILED );

        for( size_t i = 0; i < length; i++ )
        {
            const mbedtls_ecp_curve_info *info =
                        mbedtls_ecp_curve_info_from_grp_id( curve_list[i] );
            if ( info == NULL )
            {
                mbedtls_free( group_list );
                return( MBEDTLS_ERR_SSL_BAD_CONFIG );
            }
            group_list[i] = info->tls_id;
        }

        group_list[length] = 0;

        ssl->handshake->group_list = group_list;
        ssl->handshake->group_list_heap_allocated = 1;
    }
    else
    {
        ssl->handshake->group_list = ssl->conf->group_list;
        ssl->handshake->group_list_heap_allocated = 0;
    }
#endif /* MBEDTLS_DEPRECATED_REMOVED */
#endif /* MBEDTLS_ECP_C */

#if defined(MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED)
#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    /* Heap allocate and translate sig_hashes from internal hash identifiers to
       signature algorithms IANA identifiers.  */
    if ( mbedtls_ssl_conf_is_tls12_only( ssl->conf ) &&
         ssl->conf->sig_hashes != NULL )
    {
        const int *md;
        const int *sig_hashes = ssl->conf->sig_hashes;
        size_t sig_algs_len = 0;
        uint16_t *p;

#if defined(static_assert)
        static_assert( MBEDTLS_SSL_MAX_SIG_ALG_LIST_LEN
                       <= ( SIZE_MAX - ( 2 * sizeof(uint16_t) ) ),
                       "MBEDTLS_SSL_MAX_SIG_ALG_LIST_LEN too big" );
#endif

        for( md = sig_hashes; *md != MBEDTLS_MD_NONE; md++ )
        {
            if( mbedtls_ssl_hash_from_md_alg( *md ) == MBEDTLS_SSL_HASH_NONE )
                continue;
#if defined(MBEDTLS_ECDSA_C)
            sig_algs_len += sizeof( uint16_t );
#endif

#if defined(MBEDTLS_RSA_C)
            sig_algs_len += sizeof( uint16_t );
#endif
            if( sig_algs_len > MBEDTLS_SSL_MAX_SIG_ALG_LIST_LEN )
                return( MBEDTLS_ERR_SSL_BAD_CONFIG );
        }

        if( sig_algs_len < MBEDTLS_SSL_MIN_SIG_ALG_LIST_LEN )
            return( MBEDTLS_ERR_SSL_BAD_CONFIG );

        ssl->handshake->sig_algs = mbedtls_calloc( 1, sig_algs_len +
                                                      sizeof( uint16_t ));
        if( ssl->handshake->sig_algs == NULL )
            return( MBEDTLS_ERR_SSL_ALLOC_FAILED );

        p = (uint16_t *)ssl->handshake->sig_algs;
        for( md = sig_hashes; *md != MBEDTLS_MD_NONE; md++ )
        {
            unsigned char hash = mbedtls_ssl_hash_from_md_alg( *md );
            if( hash == MBEDTLS_SSL_HASH_NONE )
                continue;
#if defined(MBEDTLS_ECDSA_C)
            *p = (( hash << 8 ) | MBEDTLS_SSL_SIG_ECDSA);
            p++;
#endif
#if defined(MBEDTLS_RSA_C)
            *p = (( hash << 8 ) | MBEDTLS_SSL_SIG_RSA);
            p++;
#endif
        }
        *p = MBEDTLS_TLS_SIG_NONE;
        ssl->handshake->sig_algs_heap_allocated = 1;
    }
    else
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
    {
        ssl->handshake->sig_algs_heap_allocated = 0;
    }
#endif /* !MBEDTLS_DEPRECATED_REMOVED */
#endif /* MBEDTLS_KEY_EXCHANGE_WITH_CERT_ENABLED */

    return( 0 );
}

int __wrap_mbedtls_ssl_setup(mbedtls_ssl_context *ssl, const mbedtls_ssl_config *conf)
{
    ssl->conf = conf;
    ssl->tls_version = ssl->conf->max_tls_version;

    CHECK_OK(ssl_handshake_init(ssl));

    mbedtls_free(ssl->MBEDTLS_PRIVATE(out_buf));
    ssl->MBEDTLS_PRIVATE(out_buf) = NULL;
    CHECK_OK(esp_mbedtls_setup_tx_buffer(ssl));

    mbedtls_free(ssl->MBEDTLS_PRIVATE(in_buf));
    ssl->MBEDTLS_PRIVATE(in_buf) = NULL;
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
    if (ssl->MBEDTLS_PRIVATE(out_buf)) {
        esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(out_buf));
        ssl->MBEDTLS_PRIVATE(out_buf) = NULL;
    }

    if (ssl->MBEDTLS_PRIVATE(in_buf)) {
        esp_mbedtls_free_buf(ssl->MBEDTLS_PRIVATE(in_buf));
        ssl->MBEDTLS_PRIVATE(in_buf) = NULL;
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
