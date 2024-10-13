/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include <stdbool.h>
#include "esp_mbedtls_dynamic_impl.h"

int __real_mbedtls_ssl_handshake_client_step(mbedtls_ssl_context *ssl);
int __real_mbedtls_ssl_write_client_hello(mbedtls_ssl_context *ssl);

int __wrap_mbedtls_ssl_handshake_client_step(mbedtls_ssl_context *ssl);
int __wrap_mbedtls_ssl_write_client_hello(mbedtls_ssl_context *ssl);

static const char *TAG = "SSL client";

static int manage_resource(mbedtls_ssl_context *ssl, bool add)
{
    int state = add ? ssl->MBEDTLS_PRIVATE(state) : ssl->MBEDTLS_PRIVATE(state) - 1;

    if (mbedtls_ssl_is_handshake_over(ssl) || ssl->MBEDTLS_PRIVATE(handshake) == NULL) {
        return 0;
    }

    if (!add) {
        if (!ssl->MBEDTLS_PRIVATE(out_left)) {
            CHECK_OK(esp_mbedtls_free_tx_buffer(ssl));
        }
    }

    /* Change state now, so that it is right in mbedtls_ssl_read_record(), used
     * by DTLS for dropping out-of-sequence ChangeCipherSpec records */
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    if( ssl->state == MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC &&
        ssl->handshake->new_session_ticket != 0 )
    {
        ssl->state = MBEDTLS_SSL_NEW_SESSION_TICKET;
    }
#endif

    switch (state) {
        case MBEDTLS_SSL_HELLO_REQUEST:
            break;
        case MBEDTLS_SSL_CLIENT_HELLO:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;


        case MBEDTLS_SSL_SERVER_HELLO:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;
        case MBEDTLS_SSL_SERVER_CERTIFICATE:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
                esp_mbedtls_free_cacert(ssl);
#endif
            }
            break;
        case MBEDTLS_SSL_SERVER_KEY_EXCHANGE:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                if (!ssl->MBEDTLS_PRIVATE(keep_current_message)) {
                    CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
                }
            }
            break;
        case MBEDTLS_SSL_CERTIFICATE_REQUEST:
            if (add) {
                if (!ssl->MBEDTLS_PRIVATE(keep_current_message)) {
                    CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
                }
            } else {
                if (!ssl->MBEDTLS_PRIVATE(keep_current_message)) {
                    CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
                }
            }
            break;
        case MBEDTLS_SSL_SERVER_HELLO_DONE:
            if (add) {
                if (!ssl->MBEDTLS_PRIVATE(keep_current_message)) {
                    CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
                }
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;


        case MBEDTLS_SSL_CLIENT_CERTIFICATE:
            if (add) {
                size_t buffer_len = 3;

                const mbedtls_ssl_config *conf = mbedtls_ssl_context_get_config(ssl);
                mbedtls_ssl_key_cert *key_cert = conf->MBEDTLS_PRIVATE(key_cert);

                while (key_cert && key_cert->cert) {
                    size_t num;

                    buffer_len += esp_mbedtls_get_crt_size(key_cert->cert, &num);
                    buffer_len += num * 3;

                    key_cert = key_cert->next;
                }

                buffer_len = MAX(buffer_len, MBEDTLS_SSL_OUT_BUFFER_LEN);

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_CLIENT_KEY_EXCHANGE:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_CERTIFICATE_VERIFY:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            } else {
#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
                esp_mbedtls_free_dhm(ssl);
                esp_mbedtls_free_keycert_key(ssl);
                esp_mbedtls_free_keycert(ssl);
#endif
            }
            break;
        case MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_CLIENT_FINISHED:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;


#if defined(MBEDTLS_SSL_SESSION_TICKETS)
        case MBEDTLS_SSL_NEW_SESSION_TICKET:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;
#endif


        case MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;
        case MBEDTLS_SSL_SERVER_FINISHED:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;
        case MBEDTLS_SSL_FLUSH_BUFFERS:
            break;
        case MBEDTLS_SSL_HANDSHAKE_WRAPUP:
#if defined(MBEDTLS_SSL_RENEGOTIATION)
            if (add && ssl->MBEDTLS_PRIVATE(renego_status)) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            }
#endif
            break;
        default:
            break;
    }

    return 0;
}

int __wrap_mbedtls_ssl_handshake_client_step(mbedtls_ssl_context *ssl)
{
    CHECK_OK(manage_resource(ssl, true));

    CHECK_OK(__real_mbedtls_ssl_handshake_client_step(ssl));

    CHECK_OK(manage_resource(ssl, false));

    return 0;
}

int __wrap_mbedtls_ssl_write_client_hello(mbedtls_ssl_context *ssl)
{
    CHECK_OK(manage_resource(ssl, true));

    CHECK_OK(__real_mbedtls_ssl_write_client_hello(ssl));

    CHECK_OK(manage_resource(ssl, false));

    return 0;
}
