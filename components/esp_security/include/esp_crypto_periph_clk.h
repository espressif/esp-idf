/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
 * @param enable true: enable; false: disable
 */
void esp_crypto_key_mgr_enable_periph_clk(bool enable);

#ifdef __cplusplus
}
#endif
