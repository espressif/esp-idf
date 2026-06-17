/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * Runtime verification helpers for Key-Manager ECDH0 deployments.
 *
 * After an ECDH0 deploy the KM exports k2*G in `ecdh0_info`. The host side
 * knows k1, so it can reconstruct x(k1*k2*G) on P-256 (via the ECC
 * peripheral), derive the expected per-peripheral effective key, and
 * assert the hardware output (HMAC / ECDSA pubkey / XTS ciphertext)
 * matches bit-for-bit.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compute x(k1 * k2_G) on NIST P-256 using the ECC peripheral and return it
 * as 32 big-endian bytes.
 *
 *   k1_be:     big-endian scalar bytes (24 or 32).
 *   k1_len:    length of k1_be in bytes.
 *   k2_G_km:   64-byte KM-format public point exported in ecdh0_info: the
 *              peripheral's native LE-x || LE-y layout.
 *   x_be_out:  32 big-endian bytes of the result's x-coordinate.
 */
void km_verify_compute_x_be(const uint8_t *k1_be, size_t k1_len,
                            const uint8_t *k2_G_km, uint8_t x_be_out[32]);

/* Given an ECDSA scalar (big-endian) and an ECC curve length
 * (P192_LEN / P256_LEN / P384_LEN, from ecc_impl.h), compute scalar*G on
 * that curve via the ECC peripheral and write pub_x / pub_y in the
 * little-endian byte order the ECDSA peripheral exports.
 */
void km_verify_ecdsa_pubkey_from_scalar(unsigned curve_len,
                                        const uint8_t *scalar_be,
                                        size_t scalar_len,
                                        uint8_t *pub_x_le_out,
                                        uint8_t *pub_y_le_out);

/* ESP32 flash-encryption XTS-AES in software (block-reversal wrapper around
 * standard XTS, flash-address-based tweak). Encrypts `pt_len` <= 128 bytes
 * at a 128-byte-aligned `flash_address`.
 */
void km_verify_flash_xts_encrypt(const uint8_t *key, size_t key_bits,
                                 uint32_t flash_address,
                                 const uint8_t *pt, size_t pt_len,
                                 uint8_t *ct_out);

/* HMAC-SHA256 one-shot via PSA */
void km_verify_hmac_sha256(const uint8_t *key, size_t key_len,
                           const uint8_t *msg, size_t msg_len,
                           uint8_t mac_out[32]);

#ifdef __cplusplus
}
#endif
