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

#ifndef _DYNAMIC_IMPL_H_
#define _DYNAMIC_IMPL_H_

#include <stddef.h>
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/platform.h"
#include "esp_log.h"

#define TRACE_CHECK(_fn, _state) \
({ \
    ESP_LOGV(TAG, "%d " _state " to do \"%s\"", __LINE__, # _fn); \
})

#define CHECK_OK(_fn) \
({ \
    int _ret; \
 \
    TRACE_CHECK(_fn, "state"); \
 \
    if ((_ret = _fn) != 0) { \
        ESP_LOGV(TAG, "\"%s\" result is -0x%x", # _fn, -_ret); \
        TRACE_CHECK(_fn, "fail"); \
        return _ret; \
    } \
 \
    TRACE_CHECK(_fn, "end"); \
 \
})

typedef enum {
    ESP_MBEDTLS_SSL_BUF_CACHED,
    ESP_MBEDTLS_SSL_BUF_NO_CACHED,
} esp_mbedtls_ssl_buf_states;

struct esp_mbedtls_ssl_buf {
    esp_mbedtls_ssl_buf_states state;
    unsigned int len;
    unsigned char buf[];
};

#define SSL_BUF_HEAD_OFFSET_SIZE offsetof(struct esp_mbedtls_ssl_buf, buf)

void esp_mbedtls_free_buf(unsigned char *buf);

int esp_mbedtls_setup_tx_buffer(mbedtls_ssl_context *ssl);

void esp_mbedtls_setup_rx_buffer(mbedtls_ssl_context *ssl);

int esp_mbedtls_reset_add_tx_buffer(mbedtls_ssl_context *ssl);

int esp_mbedtls_reset_add_rx_buffer(mbedtls_ssl_context *ssl);

int esp_mbedtls_reset_free_tx_buffer(mbedtls_ssl_context *ssl);

void esp_mbedtls_reset_free_rx_buffer(mbedtls_ssl_context *ssl);

int esp_mbedtls_add_tx_buffer(mbedtls_ssl_context *ssl, size_t buffer_len);

int esp_mbedtls_add_rx_buffer(mbedtls_ssl_context *ssl);

int esp_mbedtls_free_tx_buffer(mbedtls_ssl_context *ssl);

int esp_mbedtls_free_rx_buffer(mbedtls_ssl_context *ssl);

size_t esp_mbedtls_get_crt_size(mbedtls_x509_crt *cert, size_t *num);

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
void esp_mbedtls_free_dhm(mbedtls_ssl_context *ssl);

void esp_mbedtls_free_keycert(mbedtls_ssl_context *ssl);

void esp_mbedtls_free_keycert_cert(mbedtls_ssl_context *ssl);

void esp_mbedtls_free_keycert_key(mbedtls_ssl_context *ssl);
#endif

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
void esp_mbedtls_free_cacert(mbedtls_ssl_context *ssl);
#endif

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_PEER_CERT
void esp_mbedtls_free_peer_cert(mbedtls_ssl_context *ssl);

bool esp_mbedtls_ssl_is_rsa(mbedtls_ssl_context *ssl);
#endif

#endif /* _DYNAMIC_IMPL_H_ */
