/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "soc/soc_caps.h"
#include "hal/ecdsa_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Shared ECDSA test helpers. The definitions live in ecdsa/test_ecdsa.c and are
 * also called from key_manager/test_key_manager.c. Both translation units include
 * this header so that a signature change is caught at compile time in every caller. */

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
void test_ecdsa_export_pubkey(ecdsa_curve_t curve, uint8_t *ecdsa_pub_x, uint8_t *ecdsa_pub_y, bool use_km_key, const uint8_t *sw_key);
void test_ecdsa_export_pubkey_inner(ecdsa_curve_t curve, uint8_t *exported_pub_x, uint8_t *exported_pub_y, bool use_km_key, const uint8_t *sw_key, uint16_t *len);
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

void test_ecdsa_sign(ecdsa_curve_t curve, uint8_t *sha, uint8_t *r_le, uint8_t *s_le, bool use_km_key, ecdsa_sign_type_t k_type, const uint8_t *sw_key);
int test_ecdsa_verify(ecdsa_curve_t curve, uint8_t *sha, uint8_t *r_le, uint8_t *s_le, uint8_t *pub_x, uint8_t *pub_y);
void test_ecdsa_sign_and_verify(ecdsa_curve_t curve, uint8_t *sha, uint8_t *pub_x, uint8_t *pub_y, bool use_km_key, ecdsa_sign_type_t k_type, const uint8_t *sw_key);

#ifdef __cplusplus
}
#endif
