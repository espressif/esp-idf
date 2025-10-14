/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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
#include "hal/ecdsa_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USE_ECDSA_KEY_FROM_KEY_MANAGER INT_MAX

/**
 * @brief ECDSA private key context initialization config structure
 * @note  Contains configuration information like the efuse key block that should be used as the private key,
 *        EC group ID of the private key and if the export public key operation is supported
 *        by the peripheral, a flag load_pubkey that is used specify if the public key has to be populated
 */
typedef struct {
    mbedtls_ecp_group_id grp_id;            /*!< MbedTLS ECP group identifier */
    union {
        uint8_t efuse_block;                /*!< EFuse block id for ECDSA private key */
        const char *tee_key_id;             /*!< TEE secure storage key id for ECDSA private key */
    };                                      /*!< Union to hold either EFuse block id or TEE secure storage key id for ECDSA private key */
#if SOC_ECDSA_SUPPORT_EXPORT_PUBKEY || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    bool load_pubkey;                       /*!< Export ECDSA public key from the hardware */

#endif
    bool use_km_key;                        /*!< Use key deployed in the key manager for ECDSA operation.
                                                 Note: The key must be already deployed by the application and it must be activated for the lifetime of this context */
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    bool use_tee_sec_stg_key;               /*!< Use key deployed in the TEE secure storage for ECDSA operation.
                                                 Note: The key must be already deployed by the application and it must be activated for the lifetime of this context */
#endif
} esp_ecdsa_pk_conf_t;  //TODO: IDF-9008 (Add a config to select the ecdsa key from the key manager peripheral)

#if SOC_ECDSA_SUPPORT_EXPORT_PUBKEY || __DOXYGEN__

/**
 * @brief Populate the public key buffer of the mbedtls_ecp_keypair context.
 *
 * @param keypair The mbedtls ECP key-pair structure
 * @param efuse_blk The efuse key block that should be used as the private key.
 *                  The efuse block where ECDSA key is stored.  If two blocks are used to store the key, then the macro HAL_ECDSA_COMBINE_KEY_BLOCKS() can be used to combine them. The macro is defined in hal/ecdsa_types.h.
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
 *                  The efuse block where ECDSA key is stored.  If two blocks are used to store the key, then the macro HAL_ECDSA_COMBINE_KEY_BLOCKS() can be used to combine them. The macro is defined in hal/ecdsa_types.h.
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
 *                  The efuse block where ECDSA key is stored.  If two blocks are used to store the key, then the macro HAL_ECDSA_COMBINE_KEY_BLOCKS() can be used to combine them. The macro is defined in hal/ecdsa_types.h.
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

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN || __DOXYGEN__

/**
 * @brief Populate the public key buffer of the mbedtls_ecp_keypair context from
 *        the TEE secure storage.
 *
 * @param keypair The mbedtls ECP key-pair structure
 * @param tee_key_id The TEE secure storage key id of the private key
 *
 * @return - 0 if successful else MBEDTLS_ERR_ECP_BAD_INPUT_DATA
 */
int esp_ecdsa_tee_load_pubkey(mbedtls_ecp_keypair *keypair, const char *tee_key_id);

/**
 * @brief Initialize PK context and fully populate the mbedtls_ecp_keypair context.
 *        This function modifies the MPI struct used to represent the private key `d`
 *        in the ECP keypair to differentiate between TEE secure storage keys and software keys.
 *        It also populates the ECP group field in the mbedtls_ecp_keypair context.
 *        Additionally, if the load_pubkey flag is set in the esp_ecdsa_pk_conf_t config argument,
 *        the public key buffer of the mbedtls_ecp_keypair context will be populated.
 *
 * @param key_ctx The context in which this function stores the TEE secure storage context.
 *                This must be uninitialized.
 * @param conf ESP-ECDSA private key context initialization config structure.
 *
 * @return - 0 if successful
 *         - -1 otherwise
 */
int esp_ecdsa_tee_set_pk_context(mbedtls_pk_context *key_ctx, esp_ecdsa_pk_conf_t *conf);

#endif // CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN || __DOXYGEN__

#ifdef __cplusplus
}
#endif
