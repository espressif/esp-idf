/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#include "stdint.h"
#include "hal/ecc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the work mode of the operation
 *
 * @param mode Mode of operation
 */
void ecc_hal_set_mode(ecc_mode_t mode);

/**
 * @brief Set the ECC curve of operation
 *
 * @param curve Curve to use for operation
 */
void ecc_hal_set_curve(ecc_curve_t curve);

/**
 * @brief Start calculation
 *
 */
void ecc_hal_start_calc(void);

/**
 * @brief Check whether the calculation has finished
 *
 * @return - 1 if the hardware has finished calculating
 *         - 0 otherwise
 */
int ecc_hal_is_calc_finished(void);

/**
 * @brief Write parameters for point multiplication (K * (Px, Py))
 *
 * @param k Scalar value
 * @param px X coordinate of the ECC point
 * @param py Y coordinate of the ECC point
 * @param len Length (in bytes) of the ECC point
 *            - 32 bytes for SECP256R1
 *            - 24 bytes for SECP192R1
 */
void ecc_hal_write_mul_param(const uint8_t *k, const uint8_t *px, const uint8_t *py, uint16_t len);

/**
 * @brief Write parameters for point verification,
 *        i.e to check if the point lies on the curve
 *
 * @param px X coordinate of the ECC point
 * @param py Y coordinate of the ECC point
 * @param len Length (in bytes) of the ECC point
 *            - 32 for SECP256R1
 *            - 24 for SECP192R1
 */
void ecc_hal_write_verify_param(const uint8_t *px, const uint8_t *py, uint16_t len);

/**
 * @brief Read point multiplication result
 *
 * @param rx X coordinate of the multiplication result
 * @param ry Y coordinate of the multiplication result
 * @param len Length (in bytes) of the ECC point
 *            - 32 for SECP256R1
 *            - 24 for SECP192R1
 *
 * @return - 0 if the operation was successful
 *         - -1 if the operation was not successful
 *
 * In case the operation is not successful, rx and ry will contain
 * all zeros
 */
int ecc_hal_read_mul_result(uint8_t *rx, uint8_t *ry, uint16_t len);

/**
 * @brief Read point verification result
 *
 * @return - 1 if point lies on curve
 *         - 0 otherwise
 */
int ecc_hal_read_verify_result(void);

#ifdef __cplusplus
}
#endif
