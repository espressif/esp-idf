// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and

#include <sys/param.h>
#include "esp_mbedtls_dynamic_impl.h"

int __real_mbedtls_ssl_handshake_server_step(mbedtls_ssl_context *ssl);

int __wrap_mbedtls_ssl_handshake_server_step(mbedtls_ssl_context *ssl);

static const char *TAG = "SSL Server";

static int manage_resource(mbedtls_ssl_context *ssl, bool add)
{
    int state = add ? ssl->state : ssl->state - 1;

    if (ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER || ssl->handshake == NULL) {
        return 0;
    }

    if (!add) {
        if (!ssl->out_left) {
            CHECK_OK(esp_mbedtls_free_tx_buffer(ssl));
        }
    }

    switch (state) {
        case MBEDTLS_SSL_HELLO_REQUEST:
            ssl->major_ver = MBEDTLS_SSL_MAJOR_VERSION_3;
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
                mbedtls_ssl_key_cert *key_cert = ssl->conf->key_cert;

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
                esp_mbedtls_free_keycert_cert(ssl);
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
                esp_mbedtls_free_keycert_key(ssl);
                esp_mbedtls_free_keycert(ssl);
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

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
                esp_mbedtls_free_cacert(ssl);
#endif
            }
            break;
        case MBEDTLS_SSL_CLIENT_KEY_EXCHANGE:
            if (add) {
                CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));
            } else {
                CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
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

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_PEER_CERT
                esp_mbedtls_free_peer_cert(ssl);
#endif
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
