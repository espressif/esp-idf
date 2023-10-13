/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdlib>
#include "nvs_partition.hpp"

namespace nvs {

NVSPartition::NVSPartition(const esp_partition_t* partition)
    : mESPPartition(partition)
{
    // ensure the class is in a valid state
    if (partition == nullptr) {
        std::abort();
    }
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
    if (size % ESP_ENCRYPT_BLOCK_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }

    return esp_partition_read(mESPPartition, src_offset, dst, size);
}

esp_err_t NVSPartition::write_raw(size_t dst_offset, const void* src, size_t size)
{
    return esp_partition_write_raw(mESPPartition, dst_offset, src, size);
}

esp_err_t NVSPartition::write(size_t dst_offset, const void* src, size_t size)
{
    if (size % ESP_ENCRYPT_BLOCK_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }

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

} // nvs
