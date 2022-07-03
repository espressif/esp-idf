/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Place-holder lock APIs as hardware AES is not supported in ESP32-C2
static inline void esp_crypto_sha_aes_lock_acquire(void) {}
static inline void esp_crypto_sha_aes_lock_release(void) {}

#ifdef __cplusplus
}
#endif
