/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include_next "mbedtls/bignum.h"
#include "sdkconfig.h"

/**
 * This is a wrapper for the main mbedtls/bignum.h. This wrapper
 * provides a few additional ESP32-only functions.
 *
 * This is because we don't set MBEDTLS_BIGNUM_ALT in the same way we
 * do for AES, SHA, etc. Because we still use most of the bignum.h
 * implementation and just replace a few hardware accelerated
 * functions (see MBEDTLS_MPI_EXP_MOD_ALT & MBEDTLS_MPI_MUL_MPI_ALT in
 * esp_config.h).
 *
 * @note Unlike the other hardware accelerator support functions in esp32/hwcrypto, there is no
 * generic "hwcrypto/bignum.h" header for using these functions without mbedTLS. The reason for this
 * is that all of the function implementations depend strongly upon the mbedTLS MPI implementation.
 */

/**
 * @brief Lock access to RSA Accelerator (MPI/bignum operations)
 *
 * RSA Accelerator hardware unit can only be used by one
 * consumer at a time.
 *
 * @note This function is non-recursive (do not call it twice from the
 * same task.)
 *
 * @note You do not need to call this if you are using the mbedTLS bignum.h
 * API or esp_mpi_xxx functions. This function is only needed if you
 * want to call ROM RSA functions or access the registers directly.
 *
 */
void esp_mpi_acquire_hardware(void);

/**
 * @brief Unlock access to RSA Accelerator (MPI/bignum operations)
 *
 * Has to be called once for each call to esp_mpi_acquire_hardware().
 *
 * @note You do not need to call this if you are using the mbedTLS bignum.h
 * API or esp_mpi_xxx functions. This function is only needed if you
 * want to call ROM RSA functions or access the registers directly.
 */
void esp_mpi_release_hardware(void);

#if CONFIG_MBEDTLS_HARDWARE_MPI

/* @brief MPI modular mupltiplication function
 *
 * Calculates Z = (X * Y) mod M using MPI hardware acceleration.
 *
 * This is not part of the standard mbedTLS bignum API.
 *
 * @note All of X, Y & Z should be less than 4096 bit long or an error is returned.
 *
 * @param Z Result bignum, should be pre-initialised with mbedtls_mpi_init().
 * @param X First multiplication argument.
 * @param Y Second multiplication argument.
 * @param M Modulus value for result.
 *
 * @return 0 on success, mbedTLS MPI error codes on failure.
 */
int esp_mpi_mul_mpi_mod(mbedtls_mpi *Z, const mbedtls_mpi *X, const mbedtls_mpi *Y, const mbedtls_mpi *M);

#if CONFIG_MBEDTLS_LARGE_KEY_SOFTWARE_MPI

/**
 * @brief          Perform a sliding-window exponentiation: X = A^E mod N
 *
 * @param X        The destination MPI. This must point to an initialized MPI.
 * @param A        The base of the exponentiation.
 *                 This must point to an initialized MPI.
 * @param E        The exponent MPI. This must point to an initialized MPI.
 * @param N        The base for the modular reduction. This must point to an
 *                 initialized MPI.
 * @param _RR      A helper MPI depending solely on \p N which can be used to
 *                 speed-up multiple modular exponentiations for the same value
 *                 of \p N. This may be \c NULL. If it is not \c NULL, it must
 *                 point to an initialized MPI.
 *
 * @return         \c 0 if successful.
 * @return         #MBEDTLS_ERR_MPI_ALLOC_FAILED if a memory allocation failed.
 * @return         #MBEDTLS_ERR_MPI_BAD_INPUT_DATA if \c N is negative or
 *                 even, or if \c E is negative.
 * @return         Another negative error code on different kinds of failures.
 *
 */
int mbedtls_mpi_exp_mod_soft(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RR);

#endif // CONFIG_MBEDTLS_LARGE_KEY_SOFTWARE_MPI

#endif // CONFIG_MBEDTLS_HARDWARE_MPI
