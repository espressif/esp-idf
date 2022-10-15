/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include "esp_mbedtls_dynamic_impl.h"

int __real_mbedtls_ssl_handshake_server_step(mbedtls_ssl_context *ssl);

int __wrap_mbedtls_ssl_handshake_server_step(mbedtls_ssl_context *ssl);

static const char *TAG = "SSL Server";

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
/**
 * Check if ciphersuite uses rsa key exchange methods.
 */
static bool ssl_ciphersuite_uses_rsa_key_ex(mbedtls_ssl_context *ssl)
{
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info =
        ssl->MBEDTLS_PRIVATE(handshake)->ciphersuite_info;

    if (ciphersuite_info->MBEDTLS_PRIVATE(key_exchange) == MBEDTLS_KEY_EXCHANGE_RSA ||
        ciphersuite_info->MBEDTLS_PRIVATE(key_exchange) == MBEDTLS_KEY_EXCHANGE_RSA_PSK) {
        return true;
    } else {
        return false;
    }
}
#endif

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

    switch (state) {
        case MBEDTLS_SSL_HELLO_REQUEST:
            break;
        case MBEDTLS_SSL_CLIENT_HELLO:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;


        case MBEDTLS_SSL_SERVER_HELLO:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_SERVER_CERTIFICATE:
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
            } else {
#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
                /**
                 * Not free keycert->cert until MBEDTLS_SSL_CLIENT_KEY_EXCHANGE for rsa key exchange methods.
                 * For ssl server will use keycert->cert to parse client key exchange.
                 */
                if (!ssl_ciphersuite_uses_rsa_key_ex(ssl)) {
                    esp_mbedtls_free_keycert_cert(ssl);
                }
#endif
            }
            break;
        case MBEDTLS_SSL_SERVER_KEY_EXCHANGE:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            } else {
#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
                esp_mbedtls_free_dhm(ssl);
                /**
                 * Not free keycert->key and keycert until MBEDTLS_SSL_CLIENT_KEY_EXCHANGE for rsa key exchange methods.
                 * For ssl server will use keycert->key to parse client key exchange.
                 */
                if (!ssl_ciphersuite_uses_rsa_key_ex(ssl)) {
                    esp_mbedtls_free_keycert_key(ssl);
                    esp_mbedtls_free_keycert(ssl);
                }
#endif
            }
            break;
        case MBEDTLS_SSL_CERTIFICATE_REQUEST:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_SERVER_HELLO_DONE:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;


        case MBEDTLS_SSL_CLIENT_CERTIFICATE:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
                esp_mbedtls_free_cacert(ssl);
#endif
            }
            break;
        case MBEDTLS_SSL_CLIENT_KEY_EXCHANGE:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
                /**
                 * Free keycert after MBEDTLS_SSL_CLIENT_KEY_EXCHANGE for rsa key exchange methods.
                 * For ssl server will use keycert->cert and keycert->key to parse client key exchange.
                 */
                if (ssl_ciphersuite_uses_rsa_key_ex(ssl)) {
                    esp_mbedtls_free_keycert_cert(ssl);
                    esp_mbedtls_free_keycert_key(ssl);
                    esp_mbedtls_free_keycert(ssl);
                }
#endif
            }
            break;
        case MBEDTLS_SSL_CERTIFICATE_VERIFY:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;
        case MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;
        case MBEDTLS_SSL_CLIENT_FINISHED:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
            }
            break;


        case MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_SERVER_FINISHED:
            if (add) {
                size_t buffer_len = MBEDTLS_SSL_OUT_BUFFER_LEN;

                CHECK_OK(esp_mbedtls_add_tx_buffer(ssl, buffer_len));
            }
            break;
        case MBEDTLS_SSL_FLUSH_BUFFERS:
            break;
        case MBEDTLS_SSL_HANDSHAKE_WRAPUP:
            break;
        default:
            break;
    }

    return 0;
}

int __wrap_mbedtls_ssl_handshake_server_step(mbedtls_ssl_context *ssl)
{
    CHECK_OK(manage_resource(ssl, true));

    CHECK_OK(__real_mbedtls_ssl_handshake_server_step(ssl));

    CHECK_OK(manage_resource(ssl, false));

    return 0;
}
