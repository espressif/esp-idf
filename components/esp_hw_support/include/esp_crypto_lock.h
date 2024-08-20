/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SOC_HMAC_SUPPORTED
/**
 * @brief Acquire lock for HMAC cryptography peripheral
 *
 * Internally also locks the SHA peripheral, as the HMAC depends on the SHA peripheral
 */
void esp_crypto_hmac_lock_acquire(void);

/**
 * @brief Release lock for HMAC cryptography peripheral
 *
 * Internally also releases the SHA peripheral, as the HMAC depends on the SHA peripheral
 */
void esp_crypto_hmac_lock_release(void);
#endif /* SOC_HMAC_SUPPORTED */

#ifdef SOC_DIG_SIGN_SUPPORTED
/**
 * @brief Acquire lock for DS cryptography peripheral
 *
 * Internally also locks the HMAC (which locks SHA), AES and MPI  peripheral, as the DS depends on these peripherals
 */
void esp_crypto_ds_lock_acquire(void);

/**
 * @brief Release lock for DS cryptography peripheral
 *
 * Internally also releases the HMAC (which locks SHA), AES and MPI  peripheral, as the DS depends on these peripherals
 */
void esp_crypto_ds_lock_release(void);
#endif /* SOC_DIG_SIGN_SUPPORTED */

#if defined(SOC_SHA_SUPPORTED) && defined(SOC_AES_SUPPORTED)
/**
 * @brief Acquire lock for the SHA and AES cryptography peripheral.
 *
 */
void esp_crypto_sha_aes_lock_acquire(void);

/**
 * @brief Release lock for the SHA and AES cryptography peripheral.
 *
 */
void esp_crypto_sha_aes_lock_release(void);
#endif /* defined(SOC_SHA_SUPPORTED) && defined(SOC_AES_SUPPORTED) */

#if defined(SOC_SHA_CRYPTO_DMA) && defined(SOC_AES_CRYPTO_DMA)
/**
 * This API should be used by all components which use the SHA, AES, HMAC and DS crypto hardware on the ESP32S2.
 * They can not be used in parallel because they use the same DMA or are calling each other.
 * E.g., HMAC uses SHA or DS uses HMAC and AES. See the ESP32S2 Technical Reference Manual for more details.
 *
 * Other unrelated components must not use it.
 */

/**
 * Acquire lock for the AES and SHA cryptography peripherals, which both use the crypto DMA.
 */
void esp_crypto_dma_lock_acquire(void);

/**
 * Release lock for the AES and SHA cryptography peripherals, which both use the crypto DMA.
 */
void esp_crypto_dma_lock_release(void);
#endif /* defined(SOC_SHA_CRYPTO_DMA) && defined(SOC_AES_CRYPTO_DMA) */

#ifdef SOC_MPI_SUPPORTED
/**
 * @brief Acquire lock for the mpi cryptography peripheral.
 *
 */
void esp_crypto_mpi_lock_acquire(void);

/**
 * @brief Release lock for the mpi/rsa cryptography peripheral.
 *
 */
void esp_crypto_mpi_lock_release(void);
#endif /* SOC_MPI_SUPPORTED */

#ifdef SOC_ECC_SUPPORTED
/**
 * @brief Acquire lock for the ECC cryptography peripheral.
 *
 */
void esp_crypto_ecc_lock_acquire(void);

/**
 * @brief Release lock for the ECC cryptography peripheral.
 *
 */
void esp_crypto_ecc_lock_release(void);
#endif /* SOC_ECC_SUPPORTED */

#ifdef SOC_ECDSA_SUPPORTED
/**
 * @brief Acquire lock for ECDSA cryptography peripheral
 *
 * Internally also locks the ECC and MPI peripheral, as the ECDSA depends on these peripherals
 */
void esp_crypto_ecdsa_lock_acquire(void);

/**
 * @brief Release lock for ECDSA cryptography peripheral
 *
 * Internally also releases the ECC and MPI peripheral, as the ECDSA depends on these peripherals
 */
void esp_crypto_ecdsa_lock_release(void);
#endif /* SOC_ECDSA_SUPPORTED */

#ifdef SOC_KEY_MANAGER_SUPPORTED
/**
 * @brief Acquire lock for Key Manager peripheral
 *
 */
void esp_crypto_key_manager_lock_acquire(void);

/**
 * @brief Release lock for Key Manager peripheral
 *
 */
void esp_crypto_key_manager_lock_release(void);
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#ifdef __cplusplus
}
#endif
