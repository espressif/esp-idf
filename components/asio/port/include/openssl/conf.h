/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_ASIO_OPENSSL_CONF_H
#define _ESP_ASIO_OPENSSL_CONF_H
#include "esp_asio_config.h"
#include "openssl/esp_asio_openssl_stubs.h"

#if defined(ASIO_USE_WOLFSSL)
// SSLv3 Methods not present in current wolfSSL library
#define OPENSSL_NO_SSL3
#include_next "openssl/conf.h"
#endif // ASIO_USE_WOLFSSL

#endif // _ESP_ASIO_OPENSSL_CONF_H
