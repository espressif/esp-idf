/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_ASIO_OPENSSL_X509V3_STUB_H
#define _ESP_ASIO_OPENSSL_X509V3_STUB_H
// Dummy header needed for ASIO compilation with esp-openssl

#if defined(ASIO_USE_WOLFSSL)
#include_next "openssl/x509v3.h"
#endif // ASIO_USE_WOLFSSL

#endif // _ESP_ASIO_OPENSSL_X509V3_STUB_H
