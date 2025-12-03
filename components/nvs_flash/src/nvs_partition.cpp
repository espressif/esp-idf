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
    return esp_partition_erase_range(mESPPartition, dst_offset, size);
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
