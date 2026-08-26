/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "nvs_partition.hpp"                // for NVSPartition class
#include "esp_err.h"                        // for esp_err_t
#include "esp_private/partition_linux.h"    // for constants of the fail_after() ESP_PARTITION_FAIL_AFTER_MODE_BOTH

class NVSPartitionTestHelper : public nvs::NVSPartition {
public:
    // Constructor uses esp_partition API to initialize the partition
    // Parameters:
    // - part_name: name of the partition to be used for testing
    // - erase_partition: if true, the partition will be erased before testing
    NVSPartitionTestHelper(const char *part_name, const bool erase_partition = true);

    // Destructor decreases the instance count
    // and unmaps the partition memory if this is the last instance
    virtual ~NVSPartitionTestHelper();

    // Load the partition from a file
    esp_err_t load_from_file(const char *file_name);

    // Return the number of NVS sectors in the partition
    uint32_t get_sectors();

    // Overrides the erase size to the specified size
    esp_err_t set_erase_size(const size_t size);

    // Function returns the starting sector index of the partition in the emulated flash
    // The size of emulated sector is defined by ESP_PARTITION_EMULATED_SECTOR_SIZE
    // First emulated sector of the partition is any number, it depends on the partition location
    // in the emulated flash
    size_t get_first_emulated_sector_index();

    // Get the erase count of a specific NVS sector within the partition
    // The size of NVS sector is defined by NVS_CONST_PAGE_SIZE
    // First sector index is 0
    size_t get_sector_erase_count(size_t nvs_sector_index);

    //--------------- Static helper methods to make testing easier --------------------------

    // Randomize the whole partition with a given seed
    static esp_err_t randomize_partition(const char *part_name, const uint32_t seed);

    // Erase the whole partition
    static esp_err_t erase_partition(const char *part_name);

    // Get number of NVS sectors in the partition
    static uint32_t get_sectors(const char *part_name);

    // Load the partition from a file
    static esp_err_t load_from_file(const char *part_name, const char *file_name);

    // Check if a specific marker (byte pattern) exists in the partition
    // Returns ESP_OK if marker is found, ESP_ERR_NOT_FOUND if marker not found,
    // or other error codes for technical failures
    static esp_err_t check_marker(const char *part_name, const uint8_t *marker, size_t marker_len);

    // Set of functions to access partition statistics
    // At the moment these functions are global, it means that they do not distinguish between
    // different partitions
    // Be aware that host tests mixing multiple partitions
    // will not work correctly, as the statistics will be summed up for all partitions
    static void clear_stats(void);
    static size_t get_read_ops(void);
    static size_t get_write_ops(void);
    static size_t get_erase_ops(void);
    static size_t get_read_bytes(void);
    static size_t get_write_bytes(void);
    static size_t get_total_time(void);
    static void fail_after(size_t count, uint8_t mode);
private:
    static int s_instance_count;    // Static variable to track instance count
    size_t mOriginalEraseSize;      // Erase size of the Partition at the moment of the object creation
};
