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
#include <stdbool.h>
#include "soc/soc_caps.h"
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

#if SOC_ECC_EXTENDED_MODES_SUPPORTED
/**
 * @brief Set the mod base value used in MOD operation
 *
 * @param base Identifier of the base to use
 */
void ecc_hal_set_mod_base(ecc_mod_base_t base);

/**
 * @brief Write parameters for Jacobian verification
 *        i.e Check whether (Qx, Qy, Qz) is a point on selected curve
 *
 * @param qx X coordinate of the ECC point in jacobian form
 * @param qy Y coordinate of the ECC point in jacobian form
 * @param qz Z coordinate of the ECC point in jacobian form
 * @param len Length (in bytes) of the ECC point
 *            - 32 bytes for SECP256R1
 *            - 24 bytes for SECP192R1
 */
void ecc_hal_write_jacob_verify_param(const uint8_t *qx, const uint8_t *qy, const uint8_t *qz, uint16_t len);

/**
 * @brief Read ECC point multiplication result in jacobian form
 *
 * @param rx X coordinate of the multiplication result
 * @param ry Y coordinate of the multiplication result
 * @param rz Z coordinate of the multiplication result
 * @param len Length (in bytes) of the ECC point
 *            - 32 for SECP256R1
 *            - 24 for SECP192R1
 *
 * @return - 0 if the operation was successful
 *         - -1 if the operation was not successful
 *
 * In case the operation is not successful, rx, ry, and rz will contain
 * all zeros
 */
int ecc_hal_read_jacob_mul_result(uint8_t *rx, uint8_t *ry, uint8_t *rz, uint16_t len);

/**
 * @brief Write parameters for ECC point addition ((Px, Py, 1) + (Qx, Qy, Qz))
 *
 * @param px X coordinate of the 1st addend ECC point
 * @param py Y coordinate of the 1st addend ECC point
 * @param qx X coordinate of the 2nd addend ECC point in jacobian form
 * @param qy Y coordinate of the 2nd addend ECC point in jacobian form
 * @param qz Z coordinate of the 2nd addend ECC point in jacobian form
 * @param len Length (in bytes) of the ECC point
 *            - 32 bytes for SECP256R1
 *            - 24 bytes for SECP192R1
 */
void ecc_hal_write_point_add_param(const uint8_t *px, const uint8_t *py, const uint8_t *qx, const uint8_t *qy, const uint8_t *qz, uint16_t len);

/**
 * @brief Read ECC point addition result
 *
 * @param rx X coordinate of the addition result
 * @param ry Y coordinate of the addition result
 * @param rz Z coordinate of the addition result
 * @param len Length (in bytes) of the ECC point
 *            - 32 for SECP256R1
 *            - 24 for SECP192R1
 * @param read_jacob Read the result in Jacobian form
 *
 * @return - 0 if the operation was successful
 *         - -1 otherwise
 */
int ecc_hal_read_point_add_result(uint8_t *rx, uint8_t *ry, uint8_t *rz, uint16_t len, bool read_jacob);

/**
 * @brief Write parameters for mod operations
 *        i.e mod add, mod sub, mod mul, mod inverse mul (or mod division)
 *
 * @param a Value of operand 1
 * @param b Value of operand 2
 * @param len Length (in bytes) of the ECC point
 *            - 32 bytes for SECP256R1
 *            - 24 bytes for SECP192R1
 */
void ecc_hal_write_mod_op_param(const uint8_t *a, const uint8_t *b, uint16_t len);

/**
 * @brief Read result of mod operations
 *        i.e mod add, mod sub, mod mul, mod inverse mul (or mod division)
 *
 * @param r Result of the mod operation
 * @param len Length (in bytes) of the ECC point
 *            - 32 bytes for SECP256R1
 *            - 24 bytes for SECP192R1
 *
 * @return - 0 if operation successful
 *         - -1 otherwise
 */
int ecc_hal_read_mod_op_result(uint8_t *r, uint16_t len);

#endif /* SOC_ECC_EXTENDED_MODES_SUPPORTED */

#ifdef __cplusplus
}
#endif
