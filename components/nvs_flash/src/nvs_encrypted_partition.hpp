/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"          // For CONFIG_NVS_BDL_STACK

#include "nvs_flash.h"
#include "nvs_partition.hpp"
#include "nvs_xts_aes.h"
namespace nvs {

/**
 * Implementation of encrypted esp_partition or bdl storage wrapper for NVS.
 */
class NVSEncryptedPartition : public NVSPartition {
public:
#ifdef CONFIG_NVS_BDL_STACK
    NVSEncryptedPartition(const char* label, const esp_blockdev_handle_t bdl, const bool managed_bdl = true);
#else
    NVSEncryptedPartition(const esp_partition_t *partition);
#endif

    virtual ~NVSEncryptedPartition() { }

    /**
     * Initializes the AES encryption components with the provided configuration.
     *
     * @param cfg the configuration for the AES encryption
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the AES component initialization
     *      - ESP_ERR_NVS_XTS_CFG_FAILED if the AES key configuration is invalid
     */
    esp_err_t init(nvs_sec_cfg_t* cfg);

    /**
     * Reads and decrypts the data from the storage
     * This function is intended for reading data aligned to the size of NVS entry.
     *
     * @param src_offset the offset in the storage to read from
     * @param dst an already allocated buffer to read data into
     * @param size the size of the data to read in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition or BDL API
     */
    esp_err_t read(size_t src_offset, void* dst, size_t size) override;

    /**
     * Encrypts the data from the src buffer and writes it to the storage.
     * This function is intended for writing data aligned to the size of NVS entry.
     * Repeated writes to the same address are not expected to be called.
     *
     * @param dst_offset the offset in the storage to write to
     * @param src pointer to the buffer to write data from
     * @param size the size of the data to write in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition or BDL API
     */
    esp_err_t write(size_t dst_offset, const void* src, size_t size) override;

protected:
    XTS_CONTEXT mEctxt;     // AES context for encryption
    XTS_CONTEXT mDctxt;     // AES context for decryption
};

} // nvs
