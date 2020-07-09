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

    ESP_LOGD(TAG, "RX left %d bytes", ssl->in_msglen);

    return 0;
}

int __wrap_mbedtls_ssl_setup(mbedtls_ssl_context *ssl, const mbedtls_ssl_config *conf)
{
    CHECK_OK(__real_mbedtls_ssl_setup(ssl, conf));

    mbedtls_free(ssl->out_buf);
    ssl->out_buf = NULL;
    CHECK_OK(esp_mbedtls_setup_tx_buffer(ssl));

    mbedtls_free(ssl->in_buf);
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

    CHECK_OK(esp_mbedtls_add_rx_buffer(ssl));

    ret = __real_mbedtls_ssl_read(ssl, buf, len);

    if (rx_done(ssl)) {
        CHECK_OK(esp_mbedtls_free_rx_buffer(ssl));
    }

    return ret;
}

void __wrap_mbedtls_ssl_free(mbedtls_ssl_context *ssl)
{
    if (ssl->out_buf) {
        mbedtls_free(ssl->out_buf);
        ssl->out_buf = NULL;
    }

    if (ssl->in_buf) {
        mbedtls_free(ssl->in_buf);
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

