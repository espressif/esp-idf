/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_secure_boot.h"
#include "rom/secure_boot.h"

esp_err_t verify_ecdsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block);

esp_err_t verify_rsa_signature_block(const ets_secure_boot_signature_t *sig_block, const uint8_t *image_digest, const ets_secure_boot_sig_block_t *trusted_block);
