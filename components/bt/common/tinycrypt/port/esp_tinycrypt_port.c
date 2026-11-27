/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_tinycrypt_port.h"

#include <string.h>

#include <tinycrypt/ecc.h>

#if SOC_ECC_SUPPORTED
#include "ecc_impl.h"
#endif /* SOC_ECC_SUPPORTED */

#define ECC_MAX_PARAM_BYTES 48

#if SOC_ECC_SUPPORTED
static void uecc_vli_native_to_le(uint8_t *le, const uECC_word_t *native, uint16_t len)
{
    uint8_t be[ECC_MAX_PARAM_BYTES];

    uECC_vli_nativeToBytes(be, len, native);
    for (uint16_t i = 0; i < len; i++) {
        le[i] = be[len - 1 - i];
    }
}

static void uecc_vli_le_to_native(uECC_word_t *native, const uint8_t *le, uint16_t len)
{
    uint8_t be[ECC_MAX_PARAM_BYTES];

    for (uint16_t i = 0; i < len; i++) {
        be[i] = le[len - 1 - i];
    }
    uECC_vli_bytesToNative(native, be, len);
}

static void esp_tinycrypt_zeroize(void *buf, size_t len)
{
    volatile uint8_t *p = (volatile uint8_t *)buf;
    while (len-- > 0) {
        *p++ = 0;
    }
}

/*
 * Map a coordinate length to a curve supported by the common ECC API.
 * TinyCrypt offloads secp256r1 always, and secp192r1 when BR/EDR SMP is built.
 */
static bool esp_tinycrypt_curve_supported(uint8_t len)
{
    if (len == NUM_ECC_BYTES) {
        return true;
    }
#if uECC_SUPPORTS_secp192r1
    if (len == NUM_ECC_BYTES_SECP192R1) {
        return true;
    }
#endif /* uECC_SUPPORTS_secp192r1 */
    return false;
}
#endif /* SOC_ECC_SUPPORTED */

int esp_tinycrypt_verify_ecc_point(const uint8_t *pk_x, const uint8_t *pk_y, uint8_t length)
{
#if SOC_ECC_SUPPORTED
    ecc_point_t point = {0};

    /* Check the length before the conversion below writes ECC_MAX_PARAM_BYTES buffers. */
    if (!esp_tinycrypt_curve_supported(length)) {
        return -1;
    }

    point.len = length;
    uecc_vli_native_to_le(point.x, (const uECC_word_t *)pk_x, length);
    uecc_vli_native_to_le(point.y, (const uECC_word_t *)pk_y, length);

    int result = esp_ecc_point_verify(&point);

    esp_tinycrypt_zeroize(&point, sizeof(point));
    return result == 1 ? 0 : -1;
#else
    (void)pk_x;
    (void)pk_y;
    (void)length;
    return -1;
#endif /* SOC_ECC_SUPPORTED */
}

int esp_tinycrypt_calc_ecc_mult(const uECC_word_t *point_x, const uECC_word_t *point_y,
                                const uECC_word_t *scalar, uECC_word_t *result_x,
                                uECC_word_t *result_y, uint8_t num_bytes, bool verify_first)
{
#if SOC_ECC_SUPPORTED
    ecc_point_t point = {0};
    ecc_point_t result = {0};
    uint8_t scalar_le[MAX_SIZE] = {0};

    /* Check the length before the conversions below write ECC_MAX_PARAM_BYTES buffers. */
    if (!esp_tinycrypt_curve_supported(num_bytes)) {
        return -1;
    }

    point.len = num_bytes;
    uecc_vli_native_to_le(scalar_le, scalar, num_bytes);
    uecc_vli_native_to_le(point.x, point_x, num_bytes);
    uecc_vli_native_to_le(point.y, point_y, num_bytes);

    int ret = esp_ecc_point_multiply(&point, scalar_le, &result, verify_first);

    if (ret == 0) {
        uecc_vli_le_to_native(result_x, result.x, num_bytes);
        uecc_vli_le_to_native(result_y, result.y, num_bytes);
    }

    esp_tinycrypt_zeroize(&point, sizeof(point));
    esp_tinycrypt_zeroize(&result, sizeof(result));
    esp_tinycrypt_zeroize(scalar_le, sizeof(scalar_le));
    return ret == 0 ? 0 : -1;
#else
    (void)point_x;
    (void)point_y;
    (void)scalar;
    (void)result_x;
    (void)result_y;
    (void)num_bytes;
    (void)verify_first;
    return -1;
#endif /* SOC_ECC_SUPPORTED */
}
