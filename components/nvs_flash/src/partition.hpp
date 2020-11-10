// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
};

} // nvs

#endif // PARTITION_HPP_
