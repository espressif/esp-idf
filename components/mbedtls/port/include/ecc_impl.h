/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define P256_LEN        (256/8)
#define P192_LEN        (192/8)

/* Note: x & y are stored in little endian order (same as CPU byte order, and order used internally by most libraries).

   This is the same order used in hardware

   Note this is opposite to most byte string formats used to represent keys, which are often big endian
*/
typedef struct {
    uint8_t x[P256_LEN]; /* Little endian order */
    uint8_t y[P256_LEN]; /* Little endian order */
    unsigned len;        /* P192_LEN or P256_LEN */
} ecc_point_t;

/**
 * @brief Perform ECC point multiplication (R = K * (Px, Py))
 *
 * @param point ECC point (multiplicand)
 * @param scalar Integer represented in byte array format (multiplier)
 * @param result Result of the multiplication
 * @param verify_first Verify that the point is on the curve before performing multiplication
 *
 * @return - 0 if the multiplication was successful
 *         - -1 otherwise
 *
 * @note 'scalar' is expected as a byte array in little endian order.
 *        Most byte string formats used to represent keys are in big endian order.
 */
int esp_ecc_point_multiply(const ecc_point_t *point, const uint8_t *scalar, ecc_point_t *result, bool verify_first);

/**
 * @brief Perform ECC point verification,
 *        i.e check whether the point (Px, Py) lies on the curve
 *
 * @param point ECC point that needs to be verified
 *
 * @return - 1, if point lies on the curve
 *         - 0, otherwise
 *
 */
int esp_ecc_point_verify(const ecc_point_t *point);

#ifdef __cplusplus
}
#endif
