/*
 * SPDX-FileCopyrightText: 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BT_CRYPTO_H
#define __BT_CRYPTO_H

#include <stddef.h>
#include <stdint.h>

#include <zephyr/bluetooth/bluetooth.h>

/**
 * @brief Cypher based Message Authentication Code (CMAC) with AES 128 bit
 *
 * Defined in Core Vol. 3, part H 2.2.5.
 *
 * @param[in] key 128-bit key
 * @param[in] in message to be authenticated
 * @param[in] len length of the message in octets
 * @param[out] out message authentication code
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_aes_cmac(const uint8_t *key, const uint8_t *in, size_t len, uint8_t *out);

/**
 * @brief Cryptographic Toolbox f4
 *
 * Defined in Core Vol. 3, part H 2.2.6.
 *
 * @param[in] u 256-bit
 * @param[in] v 256-bit
 * @param[in] x 128-bit key
 * @param[in] z 8-bit
 * @param[out] res
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_f4(const uint8_t *u, const uint8_t *v, const uint8_t *x, uint8_t z, uint8_t res[16]);

/**
 * @brief Cryptographic Toolbox f5
 *
 * Defined in Core Vol. 3, part H 2.2.7.
 *
 * @param[in] w 256-bit
 * @param[in] n1 128-bit
 * @param[in] n2 128-bit
 * @param[in] a1 56-bit
 * @param[in] a2 56-bit
 * @param[out] mackey most significant 128-bit of the result
 * @param[out] ltk least significant 128-bit of the result
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_f5(const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const bt_addr_le_t *a1,
                 const bt_addr_le_t *a2, uint8_t *mackey, uint8_t *ltk);

/**
 * @brief Cryptographic Toolbox f6
 *
 * Defined in Core Vol. 3, part H 2.2.8.
 *
 * @param[in] w 128-bit
 * @param[in] n1 128-bit
 * @param[in] n2 128-bit
 * @param[in] r 128-bit
 * @param[in] iocap 24-bit
 * @param[in] a1 56-bit
 * @param[in] a2 56-bit
 * @param[out] check
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_f6(const uint8_t *w, const uint8_t *n1, const uint8_t *n2, const uint8_t *r,
                 const uint8_t *iocap, const bt_addr_le_t *a1, const bt_addr_le_t *a2,
                 uint8_t *check);

/**
 * @brief Cryptographic Toolbox g2

 * Defined in Core Vol. 3, part H 2.2.9.
 *
 * @param[in] u 256-bit
 * @param[in] v 256-bit
 * @param[in] x 128-bit
 * @param[in] y 128-bit
 * @param[out] passkey
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_g2(const uint8_t u[32], const uint8_t v[32], const uint8_t x[16], const uint8_t y[16],
                 uint32_t *passkey);

/**
 * @brief Cryptographic Toolbox h6
 *
 * Link key conversion defined in Core Vol. 3, part H 2.2.10.
 *
 * @param[in] w 128-bit key
 * @param[in] key_id 32-bit
 * @param[out] res 128-bit
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_h6(const uint8_t w[16], const uint8_t key_id[4], uint8_t res[16]);

/**
 * @brief Cryptographic Toolbox h7
 *
 * Link key conversion defined in Core Vol. 3, part H 2.2.11.
 *
 * @param[in] salt 128-bit key
 * @param[in] w 128-bit input of the AES-CMAC function
 * @param[out] res 128-bit
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_h7(const uint8_t salt[16], const uint8_t w[16], uint8_t res[16]);

/**
 * @brief Cryptographic Toolbox function h8
 *
 * Defined in Core Vol. 6, part E 1.1.1.
 *
 * @note This function is purely a shorthand for the calculation. The parameters
 * are therefore intentionally not assigned meaning.
 *
 * Pseudocode: `aes_cmac(key=aes_cmac(key=s, plaintext=k), plaintext=key_id)`
 *
 * @param[in] k (128-bit number in big endian)
 * @param[in] s (128-bit number in big endian)
 * @param[in] key_id (32-bit number in big endian)
 * @param[out] res (128-bit number in big endian)
 *
 * @retval 0 Computation was successful. @p res contains the result.
 * @retval -EIO Computation failed.
 */
int bt_crypto_h8(const uint8_t k[16], const uint8_t s[16], const uint8_t key_id[4],
                 uint8_t res[16]);

#endif /* __BT_CRYPTO_H */
