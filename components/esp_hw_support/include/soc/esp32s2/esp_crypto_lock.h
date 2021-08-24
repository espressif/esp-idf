/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * Acquire lock for the MPI/RSA cryptography peripheral
 */
void esp_crypto_mpi_lock_acquire(void);

/**
 * Release lock for the MPI/RSA cryptography peripheral
 */
void esp_crypto_mpi_lock_release(void);

#ifdef __cplusplus
}
#endif
