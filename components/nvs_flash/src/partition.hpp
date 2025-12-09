/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "intrusive_list.h"
#include "nvs_memory_management.hpp"

namespace nvs {

/**
 * @brief Abstract interface for permanent storage (partition) related operations, currently in NVS.
 *
 * It resembles the main operations required by NVS to operate on permanent storage.
 * It is implemented as an intrusive_list_node to easily store instances of it.
 * It also allows to create instances of this class without throwing exceptions using custom heap allocator
 */
class Partition : public intrusive_list_node<Partition>, public ExceptionlessAllocatable {
public:
    virtual ~Partition() { }

    /**
     * Return the name (label) of the storage.
     *
     * @return
     *      - the name of the storage, e.g. "nvs".
     */
    virtual const char *get_partition_name() = 0;

    /**
     * Reads data from the storage.
     * Raw read is used for reading data aligned down to the size of 1 byte
     * No decryption is applied.
     *
     * @param src_offset the offset in the storage to read from
     * @param dst an already allocated buffer to read data into
     * @param size the size of the data to read in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the implementation of the storage
     */
    virtual esp_err_t read_raw(size_t src_offset, void* dst, size_t size) = 0;

    /**
     * Reads data from the storage
     * This function is intended for reading data aligned to the size of NVS entry.
     * Implementations of this function may leverage the minimal size to read efficiently.
     * For implementation with encryption, this function will decrypt the data.
     *
     * @param src_offset the offset in the storage to read from
     * @param dst an already allocated buffer to read data into
     * @param size the size of the data to read in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the implementation of the storage
     */
    virtual esp_err_t read(size_t src_offset, void* dst, size_t size) = 0;

    /**
     * Writes data from the src buffer to the storage.
     * Raw write is used for writing data aligned down to the size of 1 byte
     * Repeated writes to the same address can be called thus no encryption is applied.
     *
     * @param dst_offset the offset in the storage to write to
     * @param src pointer to the buffer to write data from
     * @param size the size of the data to write in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the implementation of the storage
     */
    virtual esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) = 0;

    /**
     * Writes data from the src buffer to the storage.
     * This function is intended for writing data aligned to the size of NVS entry.
     * This, not encrypted, implementation does not require the size to be aligned to the NVS entry size though.
     * Repeated writes to the same address are not expected to be called.
     * For implementation with encryption, this function will encrypt the data.
     *
     * @param dst_offset the offset in the storage to write to
     * @param src pointer to the buffer to write data from
     * @param size the size of the data to write in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the implementation of the storage
     */
    virtual esp_err_t write(size_t dst_offset, const void* src, size_t size) = 0;

    /**
     * Erases the address range in the storage.
     * Erased data is set to 0xFF.
     *
     * @param dst_offset the offset in the storage to erase
     * @param size the size of the data to erase in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the implementation of the storage
     */
    virtual esp_err_t erase_range(size_t dst_offset, size_t size) = 0;

    /**
     * Returns the RAM address of the beginning of the memory mapped storage.
     * Not available if the block device layer is enabled.
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the implementation of the storage
     */
    virtual uint32_t get_address() = 0;

    /**
     * Returns total size of the storage in bytes.
     *
     * @return size of the storage in bytes.
     */
    virtual uint32_t get_size() = 0;

    /**
     * Returns the read-only status of the storage.
     *
     * @return true if the storage is read-only.
     */
    virtual bool get_readonly() = 0;
};

} // nvs
