/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "mbedtls/aes.h"        // For mbedtls_aes_xts_context
#include "nvs_flash.h"          // For nvs_sec_cfg_t
#include "nvs_partition.hpp"

namespace nvs {

class NVSEncryptedPartition : public NVSPartition {
public:
    NVSEncryptedPartition(const esp_partition_t *partition);

    virtual ~NVSEncryptedPartition() { }

    esp_err_t init(nvs_sec_cfg_t* cfg);

    esp_err_t read(size_t src_offset, void* dst, size_t size) override;

    esp_err_t write(size_t dst_offset, const void* src, size_t size) override;

protected:
    mbedtls_aes_xts_context mEctxt;
    mbedtls_aes_xts_context mDctxt;
};

} // nvs
