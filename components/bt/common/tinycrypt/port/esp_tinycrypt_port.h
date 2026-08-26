/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

#if SOC_ECC_SUPPORTED
#include <tinycrypt/ecc.h>

int esp_tinycrypt_verify_ecc_point(const uint8_t *pk_x, const uint8_t *pk_y, uint8_t length);

int esp_tinycrypt_calc_ecc_mult(const uECC_word_t *point_x, const uECC_word_t *point_y,
                                const uECC_word_t *scalar, uECC_word_t *result_x,
                                uECC_word_t *result_y, uint8_t num_bytes, bool verify_first);
#endif /* SOC_ECC_SUPPORTED */
