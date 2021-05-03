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
// limitations under the License.

#include <string.h>
#include "esp_mbedtls_dynamic_impl.h"

#define COUNTER_SIZE (8)
#define CACHE_IV_SIZE (16)
#define CACHE_BUFFER_SIZE (CACHE_IV_SIZE + COUNTER_SIZE)

#define TX_IDLE_BUFFER_SIZE (MBEDTLS_SSL_HEADER_LEN + CACHE_BUFFER_SIZE)

static const char *TAG = "Dynamic Impl";

static void esp_mbedtls_set_buf_state(unsigned char *buf, esp_mbedtls_ssl_buf_states state)
{
    struct esp_mbedtls_ssl_buf *temp = __containerof(buf, struct esp_mbedtls_ssl_buf, buf[0]);
    temp->state = state;
}

static esp_mbedtls_ssl_buf_states esp_mbedtls_get_buf_state(unsigned char *buf)
{
    struct esp_mbedtls_ssl_buf *temp = __containerof(buf, struct esp_mbedtls_ssl_buf, buf[0]);
    return temp->state;
}

void esp_mbedtls_free_buf(unsigned char *buf)
{
    struct esp_mbedtls_ssl_buf *temp = __containerof(buf, struct esp_mbedtls_ssl_buf, buf[0]);
    ESP_LOGV(TAG, "free buffer @ %p", temp);
    mbedtls_free(temp);
}

static void esp_mbedtls_init_ssl_buf(struct esp_mbedtls_ssl_buf *buf, unsigned int len)
{
    if (buf) {
        buf->state = ESP_MBEDTLS_SSL_BUF_CACHED;
        buf->len = len;
    }
}

static void esp_mbedtls_parse_record_header(mbedtls_ssl_context *ssl)
{
    ssl->in_msgtype =  ssl->in_hdr[0];
    ssl->in_msglen = (ssl->in_len[0] << 8) | ssl->in_len[1];
}

static int tx_buffer_len(mbedtls_ssl_context *ssl, int len)
{
    (void)ssl;

    if (!len) {
        return MBEDTLS_SSL_OUT_BUFFER_LEN;
    } else {
        return len + MBEDTLS_SSL_HEADER_LEN
                   + MBEDTLS_SSL_COMPRESSION_ADD
                   + MBEDTLS_MAX_IV_LENGTH
                   + MBEDTLS_SSL_MAC_ADD
                   + MBEDTLS_SSL_PADDING_ADD;
    }
}

static void init_tx_buffer(mbedtls_ssl_context *ssl, unsigned char *buf)
{
    /**
     * In mbedtls, ssl->out_msg = ssl->out_buf + offset;
     */
    if (!buf) {
        int out_msg_off = (int)ssl->out_msg - (int)ssl->out_buf;

        if (!out_msg_off) {
            out_msg_off = MBEDTLS_SSL_HEADER_LEN;
        }

        ssl->out_buf = NULL;
        ssl->out_ctr = NULL;
        ssl->out_hdr = NULL;
        ssl->out_len = NULL;
        ssl->out_iv  = NULL;
        ssl->out_msg = (unsigned char *)out_msg_off;
    } else {
        int out_msg_off = (int)ssl->out_msg;

        ssl->out_buf = buf;
        ssl->out_ctr = ssl->out_buf;
        ssl->out_hdr = ssl->out_buf +  8;
        ssl->out_len = ssl->out_buf + 11;
        ssl->out_iv  = ssl->out_buf + MBEDTLS_SSL_HEADER_LEN;
        ssl->out_msg = ssl->out_buf + out_msg_off;

        ESP_LOGV(TAG, "out msg offset is %d", out_msg_off);
    }

    ssl->out_msgtype = 0;
    ssl->out_msglen = 0;
    ssl->out_left = 0;
}

static void init_rx_buffer(mbedtls_ssl_context *ssl, unsigned char *buf)
{
    /**
     * In mbedtls, ssl->in_msg = ssl->in_buf + offset;
     */
    if (!buf) {
        int in_msg_off = (int)ssl->in_msg - (int)ssl->in_buf;

        if (!in_msg_off) {
            in_msg_off = MBEDTLS_SSL_HEADER_LEN;
        }

        ssl->in_buf = NULL;
        ssl->in_ctr = NULL;
        ssl->in_hdr = NULL;
        ssl->in_len = NULL;
        ssl->in_iv  = NULL;
        ssl->in_msg = (unsigned char *)in_msg_off;
    } else {
        int in_msg_off = (int)ssl->in_msg;

        ssl->in_buf = buf;
        ssl->in_ctr = ssl->in_buf;
        ssl->in_hdr = ssl->in_buf +  8;
        ssl->in_len = ssl->in_buf + 11;
        ssl->in_iv  = ssl->in_buf + MBEDTLS_SSL_HEADER_LEN;
        ssl->in_msg = ssl->in_buf + in_msg_off;

        ESP_LOGV(TAG, "in msg offset is %d", in_msg_off);
    }

    ssl->in_msgtype = 0;
    ssl->in_msglen = 0;
    ssl->in_left = 0;
}

static int esp_mbedtls_alloc_tx_buf(mbedtls_ssl_context *ssl, int len)
{
    struct esp_mbedtls_ssl_buf *esp_buf;

    if (ssl->out_buf) {
        esp_mbedtls_free_buf(ssl->out_buf);
        ssl->out_buf = NULL;
    }

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + len);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    ESP_LOGV(TAG, "add out buffer %d bytes @ %p", len, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, len);
    /**
     * Mark the out_msg offset from ssl->out_buf.
     * 
     * In mbedtls, ssl->out_msg = ssl->out_buf + offset;
     */
    ssl->out_msg = (unsigned char *)MBEDTLS_SSL_HEADER_LEN;

    init_tx_buffer(ssl, esp_buf->buf);

    return 0;
}

int esp_mbedtls_setup_tx_buffer(mbedtls_ssl_context *ssl)
{
    CHECK_OK(esp_mbedtls_alloc_tx_buf(ssl, TX_IDLE_BUFFER_SIZE));

    /* mark the out buffer has no data cached */
    esp_mbedtls_set_buf_state(ssl->out_buf, ESP_MBEDTLS_SSL_BUF_NO_CACHED);

    return 0;
}

void esp_mbedtls_setup_rx_buffer(mbedtls_ssl_context *ssl)
{
    ssl->in_msg = ssl->in_buf = NULL;
    init_rx_buffer(ssl, NULL);
}

int esp_mbedtls_reset_add_tx_buffer(mbedtls_ssl_context *ssl)
{
    return esp_mbedtls_alloc_tx_buf(ssl, MBEDTLS_SSL_OUT_BUFFER_LEN);
}

int esp_mbedtls_reset_free_tx_buffer(mbedtls_ssl_context *ssl)
{
    esp_mbedtls_free_buf(ssl->out_buf);
    init_tx_buffer(ssl, NULL);

    CHECK_OK(esp_mbedtls_setup_tx_buffer(ssl));

    return 0;
}

int esp_mbedtls_reset_add_rx_buffer(mbedtls_ssl_context *ssl)
{
    struct esp_mbedtls_ssl_buf *esp_buf;

    if (ssl->in_buf) {
        esp_mbedtls_free_buf(ssl->in_buf);
        ssl->in_buf = NULL;
    }

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + MBEDTLS_SSL_IN_BUFFER_LEN);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + MBEDTLS_SSL_IN_BUFFER_LEN);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    ESP_LOGV(TAG, "add in buffer %d bytes @ %p", MBEDTLS_SSL_IN_BUFFER_LEN, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, MBEDTLS_SSL_IN_BUFFER_LEN);
    /**
     * Mark the in_msg offset from ssl->in_buf.
     * 
     * In mbedtls, ssl->in_msg = ssl->in_buf + offset;
     */
    ssl->in_msg = (unsigned char *)MBEDTLS_SSL_HEADER_LEN;

    init_rx_buffer(ssl, esp_buf->buf);

    return 0;  
}

void esp_mbedtls_reset_free_rx_buffer(mbedtls_ssl_context *ssl)
{
    esp_mbedtls_free_buf(ssl->in_buf);
    init_rx_buffer(ssl, NULL);
}

int esp_mbedtls_add_tx_buffer(mbedtls_ssl_context *ssl, size_t buffer_len)
{
    int ret = 0;
    int cached = 0;
    struct esp_mbedtls_ssl_buf *esp_buf;
    unsigned char cache_buf[CACHE_BUFFER_SIZE];

    ESP_LOGV(TAG, "--> add out");

    if (ssl->out_buf) {
        if (esp_mbedtls_get_buf_state(ssl->out_buf) == ESP_MBEDTLS_SSL_BUF_CACHED) {
            ESP_LOGV(TAG, "out buffer is not empty");
            ret = 0;
            goto exit;
        } else {
            memcpy(cache_buf, ssl->out_buf, CACHE_BUFFER_SIZE);
            esp_mbedtls_free_buf(ssl->out_buf);
            init_tx_buffer(ssl, NULL);
            cached = 1;
        }
    }

    buffer_len = tx_buffer_len(ssl, buffer_len);

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    ESP_LOGV(TAG, "add out buffer %d bytes @ %p", buffer_len, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, buffer_len);
    init_tx_buffer(ssl, esp_buf->buf);

    if (cached) {
        memcpy(ssl->out_ctr, cache_buf, COUNTER_SIZE);
        memcpy(ssl->out_iv, cache_buf + COUNTER_SIZE, CACHE_IV_SIZE);
    }

    ESP_LOGV(TAG, "ssl->out_buf=%p ssl->out_msg=%p", ssl->out_buf, ssl->out_msg);

exit:
    ESP_LOGV(TAG, "<-- add out");

    return ret;
}


int esp_mbedtls_free_tx_buffer(mbedtls_ssl_context *ssl)
{
    int ret = 0;
    unsigned char buf[CACHE_BUFFER_SIZE];
    struct esp_mbedtls_ssl_buf *esp_buf;

    ESP_LOGV(TAG, "--> free out");

    if (!ssl->out_buf || (ssl->out_buf && (esp_mbedtls_get_buf_state(ssl->out_buf) == ESP_MBEDTLS_SSL_BUF_NO_CACHED))) {
        ret = 0;
        goto exit;
    }

    memcpy(buf, ssl->out_ctr, COUNTER_SIZE);
    memcpy(buf + COUNTER_SIZE, ssl->out_iv, CACHE_IV_SIZE);

    esp_mbedtls_free_buf(ssl->out_buf);
    init_tx_buffer(ssl, NULL);

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + TX_IDLE_BUFFER_SIZE);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + TX_IDLE_BUFFER_SIZE);
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    esp_mbedtls_init_ssl_buf(esp_buf, TX_IDLE_BUFFER_SIZE);
    memcpy(esp_buf->buf, buf, CACHE_BUFFER_SIZE);
    init_tx_buffer(ssl, esp_buf->buf);
    esp_mbedtls_set_buf_state(ssl->out_buf, ESP_MBEDTLS_SSL_BUF_NO_CACHED);
exit:
    ESP_LOGV(TAG, "<-- free out");

    return ret;
}

int esp_mbedtls_add_rx_buffer(mbedtls_ssl_context *ssl)
{
    int cached = 0;
    int ret = 0;
    int buffer_len;
    struct esp_mbedtls_ssl_buf *esp_buf;
    unsigned char cache_buf[16];
    unsigned char msg_head[5];
    size_t in_msglen, in_left;

    ESP_LOGV(TAG, "--> add rx");

    if (ssl->in_buf) {
        if (esp_mbedtls_get_buf_state(ssl->in_buf) == ESP_MBEDTLS_SSL_BUF_CACHED) {
            ESP_LOGV(TAG, "in buffer is not empty");
            ret = 0;
            goto exit;
        } else {
            cached = 1;
        }
    }

    ssl->in_hdr = msg_head;
    ssl->in_len = msg_head + 3;

    if ((ret = mbedtls_ssl_fetch_input(ssl, mbedtls_ssl_hdr_len(ssl))) != 0) {
        if (ret == MBEDTLS_ERR_SSL_TIMEOUT) {
            ESP_LOGD(TAG, "mbedtls_ssl_fetch_input reads data times out");
        } else if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
            ESP_LOGD(TAG, "mbedtls_ssl_fetch_input wants to read more data");
        } else {
            ESP_LOGE(TAG, "mbedtls_ssl_fetch_input error=-0x%x", -ret);
        }
        
        goto exit;
    }

    esp_mbedtls_parse_record_header(ssl);

    in_left = ssl->in_left;
    in_msglen = ssl->in_msglen;
    buffer_len = tx_buffer_len(ssl, in_msglen);

    ESP_LOGV(TAG, "message length is %d RX buffer length should be %d left is %d",
                (int)in_msglen, (int)buffer_len, (int)ssl->in_left);

    if (cached) {
        memcpy(cache_buf, ssl->in_buf, 16);
        esp_mbedtls_free_buf(ssl->in_buf);
        init_rx_buffer(ssl, NULL);
    }

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + buffer_len);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    ESP_LOGV(TAG, "add in buffer %d bytes @ %p", buffer_len, esp_buf->buf);

    esp_mbedtls_init_ssl_buf(esp_buf, buffer_len);
    init_rx_buffer(ssl, esp_buf->buf);

    if (cached) {
        memcpy(ssl->in_ctr, cache_buf, 8);
        memcpy(ssl->in_iv, cache_buf + 8, 8);
    }

    memcpy(ssl->in_hdr, msg_head, in_left);
    ssl->in_left = in_left;
    ssl->in_msglen = 0;

exit:
    ESP_LOGV(TAG, "<-- add rx");

    return ret; 
}

int esp_mbedtls_free_rx_buffer(mbedtls_ssl_context *ssl)
{
    int ret = 0;
    unsigned char buf[16];
    struct esp_mbedtls_ssl_buf *esp_buf;

    ESP_LOGV(TAG, "--> free rx");

    /**
     * When have read multi messages once, can't free the input buffer directly.
     */
    if (!ssl->in_buf || (ssl->in_hslen && (ssl->in_hslen < ssl->in_msglen)) ||
        (ssl->in_buf && (esp_mbedtls_get_buf_state(ssl->in_buf) == ESP_MBEDTLS_SSL_BUF_NO_CACHED))) {
        ret = 0;
        goto exit;
    }

    /**
     * The previous processing is just skipped, so "ssl->in_msglen = 0"
     */
    if (!ssl->in_msgtype) {
        goto exit;
    }

    memcpy(buf, ssl->in_ctr, 8);
    memcpy(buf + 8, ssl->in_iv, 8);

    esp_mbedtls_free_buf(ssl->in_buf);
    init_rx_buffer(ssl, NULL);

    esp_buf = mbedtls_calloc(1, SSL_BUF_HEAD_OFFSET_SIZE + 16);
    if (!esp_buf) {
        ESP_LOGE(TAG, "alloc(%d bytes) failed", SSL_BUF_HEAD_OFFSET_SIZE + 16);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    esp_mbedtls_init_ssl_buf(esp_buf, 16);
    memcpy(esp_buf->buf, buf, 16);
    init_rx_buffer(ssl, esp_buf->buf);
    esp_mbedtls_set_buf_state(ssl->in_buf, ESP_MBEDTLS_SSL_BUF_NO_CACHED);
exit:
    ESP_LOGV(TAG, "<-- free rx");

    return ret;
}

size_t esp_mbedtls_get_crt_size(mbedtls_x509_crt *cert, size_t *num)
{
    size_t n = 0;
    size_t bytes = 0;

    while (cert) {
        bytes += cert->raw.len;
        n++;
    
        cert = cert->next;
    }

    *num = n;

    return bytes;
}

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA
void esp_mbedtls_free_dhm(mbedtls_ssl_context *ssl)
{
    mbedtls_mpi_free((mbedtls_mpi *)&ssl->conf->dhm_P);
    mbedtls_mpi_free((mbedtls_mpi *)&ssl->conf->dhm_G);
}

void esp_mbedtls_free_keycert(mbedtls_ssl_context *ssl)
{
    mbedtls_ssl_config *conf = (mbedtls_ssl_config *)ssl->conf;
    mbedtls_ssl_key_cert *keycert = conf->key_cert, *next;

    while (keycert) {
        next = keycert->next;

        if (keycert) {
            mbedtls_free(keycert);
        }

        keycert = next;
    }

    conf->key_cert = NULL;
}

void esp_mbedtls_free_keycert_key(mbedtls_ssl_context *ssl)
{
    mbedtls_ssl_key_cert *keycert = ssl->conf->key_cert;

    while (keycert) {
        if (keycert->key) {
            mbedtls_pk_free(keycert->key);
            keycert->key = NULL;
        }
        keycert = keycert->next;
    }
}

void esp_mbedtls_free_keycert_cert(mbedtls_ssl_context *ssl)
{
    mbedtls_ssl_key_cert *keycert = ssl->conf->key_cert;

    while (keycert) {
        if (keycert->cert) {
            mbedtls_x509_crt_free(keycert->cert);
            keycert->cert = NULL;
        }
        keycert = keycert->next;
    }
}
#endif /* CONFIG_MBEDTLS_DYNAMIC_FREE_CONFIG_DATA */

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT
void esp_mbedtls_free_cacert(mbedtls_ssl_context *ssl)
{
    if (ssl->conf->ca_chain) {
        mbedtls_ssl_config *conf = (mbedtls_ssl_config *)ssl->conf;

        mbedtls_x509_crt_free(conf->ca_chain);
        conf->ca_chain = NULL;
    }
}
#endif /* CONFIG_MBEDTLS_DYNAMIC_FREE_CA_CERT */

#ifdef CONFIG_MBEDTLS_DYNAMIC_FREE_PEER_CERT
void esp_mbedtls_free_peer_cert(mbedtls_ssl_context *ssl)
{
    if (ssl->session_negotiate->peer_cert) {
        mbedtls_x509_crt_free( ssl->session_negotiate->peer_cert );
        mbedtls_free( ssl->session_negotiate->peer_cert );
        ssl->session_negotiate->peer_cert = NULL;
    }
}

bool esp_mbedtls_ssl_is_rsa(mbedtls_ssl_context *ssl)
{
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info =
        ssl->transform_negotiate->ciphersuite_info;

    if (ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA ||
        ciphersuite_info->key_exchange == MBEDTLS_KEY_EXCHANGE_RSA_PSK) {
        return true;
    } else {
        return false;
    }
}
#endif
