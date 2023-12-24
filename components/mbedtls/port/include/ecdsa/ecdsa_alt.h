/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "mbedtls/ecp.h"
#include "mbedtls/pk.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ECDSA private key context initialization config structure
 * @note  Contains configuration information like the efuse key block that should be used as the private key,
 *        EC group ID of the private key and if the export public key operation is supported
 *        by the peripheral, a flag load_pubkey that is used specify if the public key has to be populated
 */
typedef struct {
    mbedtls_ecp_group_id grp_id;            /*!< MbedTLS ECP group identifier */
    uint8_t efuse_block;                    /*!< EFuse block id for ECDSA private key */
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    bool load_pubkey;                       /*!< Export ECDSA public key from the hardware */
#endif
} esp_ecdsa_pk_conf_t;  //TODO: IDF-7925 (Add a config to select the ecdsa key from the key manager peripheral)

#if SOC_ECDSA_SUPPORT_EXPORT_PUBKEY || __DOXYGEN__

/**
 * @brief Populate the public key buffer of the mbedtls_ecp_keypair context.
 *
 * @param keypair The mbedtls ECP key-pair structure
 * @param efuse_blk The efuse key block that should be used as the private key.
 *                  The key purpose of this block must be ECDSA_KEY
 * @return - 0 if successful
 *         - MBEDTLS_ERR_ECP_BAD_INPUT_DATA if invalid ecp group id specified
 *         - MBEDTLS_ERR_ECP_INVALID_KEY if efuse block with purpose ECDSA_KEY is not found
 *         - -1 if invalid efuse block is specified
 */
int esp_ecdsa_load_pubkey(mbedtls_ecp_keypair *keypair, int efuse_blk);

#endif // SOC_ECDSA_SUPPORT_EXPORT_PUBKEY || __DOXYGEN__

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || __DOXYGEN__

/**
 * @brief Initialize MPI to notify mbedtls_ecdsa_sign to use the private key in efuse
 *        We break the MPI struct of the private key in order to
 *        differentiate between hardware key and software key
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
 * @param key_ctx The context in which this functions stores the hardware context.
 *                This must be uninitialized
 * @param efuse_blk The efuse key block that should be used as the private key.
 *                  The key purpose of this block must be ECDSA_KEY
 *
 * @return - 0 if successful
 *         - -1 otherwise
 */
int esp_ecdsa_privkey_load_pk_context(mbedtls_pk_context *key_ctx, int efuse_blk);

/**
 * @brief Initialize PK context and completely populate mbedtls_ecp_keypair context.
 *        We break the MPI struct used to represent the private key `d` in ECP keypair
 *        in order to differentiate between hardware key and software key.
 *        We also populate the ECP group field present in the mbedtls_ecp_keypair context.
 *        If the ECDSA peripheral of the chip supports exporting the public key,
 *        we can also populate the public key buffer of the mbedtls_ecp_keypair context
 *        if the load_pubkey flag is set in the esp_ecdsa_pk_conf_t config argument.
 *
 * @param key_ctx The context in which this functions stores the hardware context.
 *                This must be uninitialized
 * @param conf ESP-ECDSA private key context initialization config structure
 *
 * @return - 0 if successful
 *         - -1 otherwise
 */
int esp_ecdsa_set_pk_context(mbedtls_pk_context *key_ctx, esp_ecdsa_pk_conf_t *conf);

#endif // CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || __DOXYGEN__

#ifdef __cplusplus
}
#endif
