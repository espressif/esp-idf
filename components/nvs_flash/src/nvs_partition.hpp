/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_PARTITION_HPP_
#define ESP_PARTITION_HPP_

#include "esp_partition.h"
#include "intrusive_list.h"
#include "partition.hpp"
#include "nvs_memory_management.hpp"

#define ESP_ENCRYPT_BLOCK_SIZE 16

#define PART_NAME_MAX_SIZE              16   /*!< maximum length of partition name (excluding null terminator) */

namespace nvs {

/**
 * Implementation of Partition for NVS.
 *
 * It is implemented as an intrusive_list_node to easily store instances of it. NVSStorage and NVSPage take pointer
 * references of this class to abstract their partition operations.
 */
class NVSPartition : public Partition, public intrusive_list_node<NVSPartition>, public ExceptionlessAllocatable {
public:
    /**
     * Copy partition_name to mPartitionName and initialize mESPPartition.
     *
     * @param partition_name the name of the partition as in the partition table, must be non-NULL!
     * @param partition an already initialized partition structure
     */
    NVSPartition(const esp_partition_t* partition);

    /**
     * No need to de-initialize mESPPartition here, if you used esp_partition_find_first.
     * Otherwise, the user is responsible for de-initializing it.
     */
    virtual ~NVSPartition() { }

    const char *get_partition_name() override;

    /**
     * Look into \c esp_partition_read_raw for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition API
     */
    esp_err_t read_raw(size_t src_offset, void* dst, size_t size) override;

    /**
     * Look into \c esp_partition_read for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_ARG if size isn't a multiple of ESP_ENCRYPT_BLOCK_SIZE
     *      - other error codes from the esp_partition API
     */
    esp_err_t read(size_t src_offset, void* dst, size_t size) override;

    /**
     * Look into \c esp_partition_write_raw for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - error codes from the esp_partition API
     */
    esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) override;

    /**
     * Look into \c esp_partition_write for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - ESP_ERR_INVALID_ARG if size isn't a multiple of ESP_ENCRYPT_BLOCK_SIZE
     *      - other error codes from the esp_partition API
     */
    esp_err_t write(size_t dst_offset, const void* src, size_t size) override;

    /**
     * Look into \c esp_partition_erase_range for more details.
     *
     * @return
     *      - ESP_OK on success
     *      - error codes from the esp_partition API
     */
    esp_err_t erase_range(size_t dst_offset, size_t size) override;

    /**
     * @return the base address of the partition.
     */
    uint32_t get_address() override;

    /**
     * @return the size of the partition in bytes.
     */
    uint32_t get_size() override;

    /**
     * @return true if the partition is read-only.
     */
    bool get_readonly() override;

protected:
    const esp_partition_t* mESPPartition;
};

} // nvs

#endif // ESP_PARTITION_HPP_
