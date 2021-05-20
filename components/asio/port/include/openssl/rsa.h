/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_ASIO_OPENSSL_RSA_STUB_H
#define _ESP_ASIO_OPENSSL_RSA_STUB_H
// Dummy header needed for ASIO compilation with esp-openssl

#if defined(ASIO_USE_WOLFSSL)
#include_next "openssl/rsa.h"
#endif // ASIO_USE_WOLFSSL

#endif // _ESP_ASIO_OPENSSL_RSA_STUB_H
