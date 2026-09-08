/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable or disable the AES peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_aes_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the SHA peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_sha_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the MPI peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_mpi_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the ECC peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_ecc_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the HMAC peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_hmac_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the DS peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_ds_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the ECDSA peripheral clock
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_ecdsa_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the Key Manager peripheral clock
 *
 * When enable is true this also pulses the Key Manager reset. The caller must
 * hold esp_crypto_key_manager_lock across the matching true/false pair, because
 * that reset also covers the XTS-AES flash encryption key-usage selector.
 *
 * Prefer esp_crypto_key_mgr_enable_periph_clk_no_reset() when the caller only
 * needs the key-usage selector writable (ECDSA/HMAC/DS).
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_key_mgr_enable_periph_clk(bool enable);

/**
 * @brief Enable or disable the Key Manager clocks without resetting the peripheral
 *
 * Use this when a crypto accelerator only needs to write its own key-usage
 * selector. Resetting would drop the XTS-AES flash encryption selector that
 * MSPI may be using, and flash DMA does not take the Key Manager lock.
 * The caller must still hold esp_crypto_key_manager_lock across the matching
 * true/false pair to serialize selector writes.
 *
 * @param enable true: enable; false: disable
 */
void esp_crypto_key_mgr_enable_periph_clk_no_reset(bool enable);

#ifdef __cplusplus
}
#endif
