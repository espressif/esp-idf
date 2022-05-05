/*
 * md5_alt.h: MD5 block cipher
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
#ifndef MD5_ALT_H
#define MD5_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_MD5_ALT)
#include "md/esp_md.h"

#define mbedtls_md5_init                        esp_md5_init
#define mbedtls_md5_update                      esp_md5_update
#define mbedtls_md5_finish                      esp_md5_finish
#define mbedtls_md5_starts                      esp_md5_starts

#define mbedtls_md5_free                        esp_md5_free
#define mbedtls_md5_clone                       esp_md5_clone
#define mbedtls_internal_md5_process            esp_md5_process

#endif /* MBEDTLS_MD5_ALT */

#ifdef __cplusplus
}
#endif

#endif
