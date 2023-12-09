/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PARTITION_HPP_
#define PARTITION_HPP_

#include "esp_err.h"

namespace nvs {

/**
 * @brief Abstract interface for partition related operations, currently in NVS.
 *
 * It resembles the main operations according to esp_partition.h.
 */
class Partition {
public:
    virtual ~Partition() { }

    /**
     * Return the partition name as in the partition table.
     */
    virtual const char *get_partition_name() = 0;

    virtual esp_err_t read_raw(size_t src_offset, void* dst, size_t size) = 0;

    virtual esp_err_t read(size_t src_offset, void* dst, size_t size) = 0;

    virtual esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) = 0;

    virtual esp_err_t write(size_t dst_offset, const void* src, size_t size) = 0;

    virtual esp_err_t erase_range(size_t dst_offset, size_t size) = 0;

    /**
     * Return the address of the beginning of the partition.
     */
    virtual uint32_t get_address() = 0;

    /**
     * Return the partition size in bytes.
     */
    virtual uint32_t get_size() = 0;

    /**
     * Return true if the partition is read-only.
     */
    virtual bool get_readonly() = 0;
};

} // nvs

#endif // PARTITION_HPP_
