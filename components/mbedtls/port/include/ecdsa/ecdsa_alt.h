/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"
#include "mbedtls/pk.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN

/**
 * @brief Initialize MPI to notify mbedtls_ecdsa_sign to use the private key in efuse
 *        We break the MPI struct of the private key in order to
 *        differentiate between hardware key and software key
 *
 * @note Currently, `efuse_blk` is not used internally.
 *       Hardware will choose the efuse block that has purpose set to ECDSA_KEY.
 *       In case of multiple ECDSA_KEY burnt in efuse, hardware will choose the
 *       greater efuse block number as the private key.
 *
 * @param key The MPI in which this functions stores the hardware context.
 *            This must be uninitialized
 * @param efuse_blk The efuse key block that should be used as the private key.
 *                  The key purpose of this block must be ECDSA_KEY
 *
 * @return - 0 if successful
 *         - -1 otherwise
 *
 */
int esp_ecdsa_privkey_load_mpi(mbedtls_mpi *key, int efuse_blk);

/**
 * @brief Initialize PK context to notify mbedtls_ecdsa_sign to use the private key in efuse
 *        We break the MPI struct used to represent the private key `d` in ECP keypair
 *        in order to differentiate between hardware key and software key
 *
 * @note Currently, `efuse_blk` is not used internally.
 *       Hardware will choose the efuse block that has purpose set to ECDSA_KEY.
 *       In case of multiple ECDSA_KEY burnt in efuse, hardware will choose the
 *       greater efuse block number as the private key.
 *
 * @param key_ctx The context in which this functions stores the hardware context.
 *                This must be uninitialized
 * @param efuse_blk The efuse key block that should be used as the private key.
 *                  The key purpose of this block must be ECDSA_KEY
 *
 * @return - 0 if successful
 *         - -1 otherwise
 */
int esp_ecdsa_privkey_load_pk_context(mbedtls_pk_context *key_ctx, int efuse_blk);
#endif

#ifdef __cplusplus
}
#endif
