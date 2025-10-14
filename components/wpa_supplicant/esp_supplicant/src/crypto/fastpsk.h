/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate PSK
 *
 * @param password       Password
 * @param password_len   Length of password, it must be <= 63
 * @param ssid           SSID
 * @param ssid_len       Length of SSID, it must be <= 32
 * @param iterations     Iterations of the PBKDF2-SHA1, this is a dummy param and it must be 4096
 * @param output         Buffer for calculated PSK, it must be at least 32 bytes
 * @param output_len     Length of output to return, this is a dummy param and it must be 32
 * @return 0 on success, non-zero on failure
 */
int esp_fast_psk(const char *password, size_t password_len, const uint8_t *ssid, size_t ssid_len, size_t iterations, uint8_t *output, size_t output_len);

#ifdef __cplusplus
}
#endif
