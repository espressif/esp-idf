/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

#if SOC_ECC_SUPPORTED
int esp_tinycrypt_verify_ecc_point(const uint8_t *pk_x, const uint8_t *pk_y, uint8_t length);

int esp_tinycrypt_calc_ecc_mult(const uint8_t *p_x, const uint8_t *p_y, const uint8_t *scalar,
                                uint8_t *r_x, uint8_t *r_y, uint8_t num_bytes, bool verify_first);
#endif /* SOC_ECC_SUPPORTED */
