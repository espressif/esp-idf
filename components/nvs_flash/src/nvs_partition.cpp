/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_partition.hpp"
#include <cstdlib>
#include "esp_log.h"
#include "nvs_constants.h"      // For NVS_CONST_PAGE_SIZE

#ifdef CONFIG_NVS_BDL_STACK
    #include "esp_partition.h"  // For esp_blockdev_handle_t
#endif // CONFIG_NVS_BDL_STACK

#define TAG "NVSPartition"

namespace nvs {

#ifndef CONFIG_NVS_BDL_STACK
NVSPartition::NVSPartition(const esp_partition_t* partition)
    : mESPPartition(partition)
{
    // ensure the class is in a valid state
    if (partition == nullptr) {
        std::abort();
    }
}

NVSPartition::~NVSPartition()
{
    // No need to de-initialize mESPPartition here, if you used esp_partition_find_first.
}

const char *NVSPartition::get_partition_name()
{
    return mESPPartition->label;
}

esp_err_t NVSPartition::read_raw(size_t src_offset, void* dst, size_t size)
{
    return esp_partition_read_raw(mESPPartition, src_offset, dst, size);
}

esp_err_t NVSPartition::read(size_t src_offset, void* dst, size_t size)
{
    return esp_partition_read(mESPPartition, src_offset, dst, size);
}

esp_err_t NVSPartition::write_raw(size_t dst_offset, const void* src, size_t size)
{
    return esp_partition_write_raw(mESPPartition, dst_offset, src, size);
}

esp_err_t NVSPartition::write(size_t dst_offset, const void* src, size_t size)
{
    return esp_partition_write(mESPPartition, dst_offset, src, size);
}

esp_err_t NVSPartition::erase_range(size_t dst_offset, size_t size)
{
#ifndef CONFIG_NVS_FLASH_VERIFY_ERASE
    return esp_partition_erase_range(mESPPartition, dst_offset, size);
#else
    esp_err_t err = ESP_FAIL;

    // Loop up to the CONFIG_NVS_FLASH_ERASE_ATTEMPTS times to attempt the erase operation.
    for (int attempt = 0; attempt < CONFIG_NVS_FLASH_ERASE_ATTEMPTS; ++attempt)
    {
        // In the loop, do the flash erase followed by a read(s) to verify the success of the operation.

        // Perform the erase operation.
        esp_err_t err = esp_partition_erase_range(mESPPartition, dst_offset, size);
        if (err != ESP_OK)
        {
            continue; // Erase failed, continue to the next attempt.
        }

        // Validate the erase operation by reading back the erased area.
        // Buffer of some reasonable size to read back and check.
        const size_t buf_size = 32;
        uint8_t buf[buf_size];
        size_t remaining = size;
        size_t read_offset = dst_offset;

        while (remaining > 0)
        {
            size_t to_read = (remaining < buf_size) ? remaining : buf_size;
            err = esp_partition_read_raw(mESPPartition, read_offset, buf, to_read);
            if (err != ESP_OK) {
                break; // Read failed, break to attempt erase again.
            }
            for (size_t i = 0; i < to_read; ++i)
            {
                if (buf[i] != 0xFF)
                {
                    err = ESP_ERR_NOT_FINISHED; // Verification failed.
                    break;
                }
            }
            remaining -= to_read;
            read_offset += to_read;
        }
        if(err == ESP_OK) return ESP_OK; // Erase and verification succeeded do not need to attempt again.
    }
    return err; // All attempts exhausted, return the last error.
#endif // CONFIG_NVS_FLASH_VERIFY_ERASE
}

uint32_t NVSPartition::get_address()
{
    return mESPPartition->address;
}

uint32_t NVSPartition::get_size()
{
    return mESPPartition->size;
}

bool NVSPartition::get_readonly()
{
    return mESPPartition->readonly;
}

#else // CONFIG_NVS_BDL_STACK
// BDL implementation of NVSPartition
NVSPartition::NVSPartition(const char* label, const esp_blockdev_handle_t bdl, const bool managed_bdl)
    : mBDL(bdl), mManagedBDLbyInstance(managed_bdl)
{
    if (bdl == nullptr || label == nullptr) {
        std::abort();
    }
    strlcpy(mLabel, label, NVS_PART_NAME_MAX_SIZE);
    mLabel[NVS_PART_NAME_MAX_SIZE] = '\0';
}

NVSPartition::~NVSPartition()
{
    if(mManagedBDLbyInstance) {
        // If the BDL was managed by this instance, we need to de-initialize it.
        if (mBDL != nullptr) {
            mBDL->ops->release(mBDL);
        }
    }
}

const char* NVSPartition::get_partition_name()
{
    return mLabel;
}

esp_err_t NVSPartition::read_raw(size_t src_offset, void* dst, size_t size)
{
    return mBDL->ops->read(mBDL, (uint8_t*) dst, size, src_offset, size);
}

esp_err_t NVSPartition::read(size_t src_offset, void* dst, size_t size)
{
    return mBDL->ops->read(mBDL, (uint8_t*) dst, size, src_offset, size);
}

esp_err_t NVSPartition::write_raw(size_t dst_offset, const void* src, size_t size)
{
    return mBDL->ops->write(mBDL, (const uint8_t*) src, dst_offset, size);
}

esp_err_t NVSPartition::write(size_t dst_offset, const void* src, size_t size)
{
    return mBDL->ops->write(mBDL, (const uint8_t*) src, dst_offset, size);
}

esp_err_t NVSPartition::erase_range(size_t dst_offset, size_t size)
{
    return mBDL->ops->erase(mBDL, dst_offset, size);
}

uint32_t NVSPartition::get_address()
{
    // BDL cannot be used directly to get the address, as it is not a partition in the traditional sense.
    ESP_LOGE(TAG, "get_address() is not supported");
    std::abort();
    return 0;
}

uint32_t NVSPartition::get_size()
{
    return mBDL->geometry.disk_size;
}

bool NVSPartition::get_readonly()
{
    return mBDL->device_flags.read_only;
}

bool NVSPartition::is_bdl_nvs_compliant(const char* label, const esp_blockdev_handle_t bdl)
{
    if (bdl == nullptr) {
        ESP_LOGV(TAG, "Block device layer handle is null for label %s", label);
        return false;
    }
    bool is_compliant = true;

    if(label == nullptr) {
        ESP_LOGV(TAG, "Label is null, cannot check compliance");
        return false;
    }

    if(strlen(label) >= NVS_PART_NAME_MAX_SIZE) {
        ESP_LOGV(TAG, "Label %s is too long, it must be less than %d characters", label, NVS_PART_NAME_MAX_SIZE);
        is_compliant = false;
    }

    if(bdl->geometry.read_size != 1){
        ESP_LOGV(TAG, "Block device read size is not 1, it is %zu", bdl->geometry.read_size);
        is_compliant = false;
    }

    if(bdl->geometry.write_size != 1) {
        ESP_LOGV(TAG, "Block device write size is not 1, it is %zu", bdl->geometry.write_size);
        is_compliant = false;
    }

    if(NVS_CONST_PAGE_SIZE % bdl->geometry.erase_size != 0) {
        ESP_LOGV(TAG, "Block device erase size %zu does not allow erasing pages of size %d.", bdl->geometry.erase_size, NVS_CONST_PAGE_SIZE);
        is_compliant = false;
    }

    if(bdl->geometry.disk_size % NVS_CONST_PAGE_SIZE != 0) {
        ESP_LOGV(TAG, "Block device size is not a multiple of %d, it is %zu", NVS_CONST_PAGE_SIZE, bdl->geometry.disk_size);
        is_compliant = false;
    }

    if(bdl->ops->read == nullptr) {
        ESP_LOGV(TAG, "Block device read operation is not present");
        is_compliant = false;
    }

    if(bdl->ops->write == nullptr) {
        ESP_LOGV(TAG, "Block device write operation is not present");
        is_compliant = false;
    }

    if(bdl->ops->erase == nullptr) {
        ESP_LOGV(TAG, "Block device erase operation is not present");
        is_compliant = false;
    }

    if(bdl->device_flags.default_val_after_erase != 1) {
        ESP_LOGV(TAG, "Block device default value after erase has to be 1, it is %d", bdl->device_flags.default_val_after_erase);
        is_compliant = false;
    }

    if(bdl->device_flags.encrypted) {
        ESP_LOGV(TAG, "Block device is encrypted, NVS does not support encrypted BDLs");
        is_compliant = false;
    }

    return is_compliant;
}

#endif // CONFIG_NVS_BDL_STACK
} // nvs
