/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"                      // for CONFIG_NVS_BDL_STACK
#include <catch2/catch_test_macros.hpp>     // for Catch2 test macros
#include "test_fixtures.hpp"                // for test fixtures class definition
#include "nvs_partition.hpp"                // for NVSPartition class
#include "esp_partition.h"                  // for BDL and native esp_partition related functions
#include "esp_private/partition_linux.h"    // for partition_linux functions like statistics and emulated sector size
#include <random>                           // for std::random_device, std::mt19937, std::generate_n
#include <fcntl.h>                          // for open()
#include <unistd.h>                         // for close(), read(), write(), lseek()
#include "nvs_constants.h"                  // for NVS_CONST_PAGE_SIZE
#include "esp_log.h"                        // for ESP_LOGE
#include "nvs.h"                            // for ESP_ERR_NVS_NOT_FOUND

#define TAG "NVSPartitionTestHelper"

// Initialize the static variable(s) of NVSPartitionTestHelper
int NVSPartitionTestHelper::s_instance_count = 0;

#ifndef CONFIG_NVS_BDL_STACK
NVSPartitionTestHelper::NVSPartitionTestHelper(const char *part_name, const bool erase_partition) :
    // First call to the esp_partition_find_first maps the partition memory
    nvs::NVSPartition(esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name))
{
    // Increase the instance count
    s_instance_count++;

    CHECK(mESPPartition != nullptr);

    // Remember the erase size of the BDL
    mOriginalEraseSize = ((esp_partition_t*) mESPPartition)->erase_size;

    if(erase_partition)
    {
        // erase the partition before testing
        CHECK(erase_range(0, get_size()) == ESP_OK);
    }
}

NVSPartitionTestHelper::~NVSPartitionTestHelper()
{
    // Decrease the instance count
    s_instance_count--;

    // reset the erase size to the original value
    set_erase_size(mOriginalEraseSize);
}

esp_err_t NVSPartitionTestHelper::load_from_file(const char *file_name)
{
    return load_from_file(get_partition_name(), file_name);
}

uint32_t NVSPartitionTestHelper::get_sectors()
{
    return get_size() / NVS_CONST_PAGE_SIZE;
}

esp_err_t NVSPartitionTestHelper::set_erase_size(const size_t size)
{
    if(mESPPartition == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    ((esp_partition_t*) mESPPartition)->erase_size = size;
    return ESP_OK;
}

size_t NVSPartitionTestHelper::get_first_emulated_sector_index()
{
    if (mESPPartition == nullptr) {
        return 0;
    }
    return ((esp_partition_t*) mESPPartition)->address / ESP_PARTITION_EMULATED_SECTOR_SIZE;
}

size_t NVSPartitionTestHelper::get_sector_erase_count(size_t nvs_sector_index)
{
    // The esp_partition_get_sector_erase_count function is global across all partitions
    // it's sector index is relative to the emulated flash and the unit of the sector is defined by ESP_PARTITION_EMULATED_SECTOR_SIZE.
    // nvs_sector_index parameter is the index of the sector in the partition and the unit of the sector is defined by NVS_CONST_PAGE_SIZE.
    // Therefore, we need to convert the sector index to the emulated flash sector index.
    if (mESPPartition == nullptr || nvs_sector_index >= get_sectors()) {
        return 0; // Invalid sector index
    }
    // To get the correct sector index, we need to calculate the offset of the first sector
    size_t first_sector = get_first_emulated_sector_index();
    size_t emulated_sector_index = (nvs_sector_index * NVS_CONST_PAGE_SIZE) / ESP_PARTITION_EMULATED_SECTOR_SIZE;

    return esp_partition_get_sector_erase_count(emulated_sector_index + first_sector);
}

// Static methods to make testing easier

esp_err_t NVSPartitionTestHelper::randomize_partition(const char *part_name, const uint32_t seed)
{
    // Erase the whole partition before randomizing
    esp_err_t ret = erase_partition(part_name);
    if (ret != ESP_OK) {
        return ret;
    }

    // Find the partition by name
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name);

    if (part == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(seed);

    uint8_t buf[NVS_CONST_PAGE_SIZE];

    // loop over all sectors in the partition
    // and fill them with random data
    for (uint32_t i = 0; i < part->size / NVS_CONST_PAGE_SIZE; i++)
    {
        // fill the buffer with random data
        std::generate_n(buf, NVS_CONST_PAGE_SIZE, gen);

        ret = esp_partition_write(part, i * NVS_CONST_PAGE_SIZE, buf, NVS_CONST_PAGE_SIZE);

        if (ret != ESP_OK) {
            break;
        }
    }
    return ret;
}

// Erase the whole partition
esp_err_t NVSPartitionTestHelper::erase_partition(const char *part_name)
{
    // Find the partition by name
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name);

    if (part == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }
    // Erase the whole partition
    return esp_partition_erase_range(part, 0, part->size);
}

// Get number of NVS sectors in the partition
uint32_t NVSPartitionTestHelper::get_sectors(const char *part_name)
{
    // Find the partition by name
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name);

    if (part == nullptr) {
        return 0;
    }

    return part->size / NVS_CONST_PAGE_SIZE;
}

// Load the partition from a file
esp_err_t NVSPartitionTestHelper::load_from_file(const char *part_name, const char *file_name)
{
    // Find the partition by name
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name);

    if (part == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }

    // Load the partition from file
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1) {
        return ESP_ERR_NOT_FOUND;
    }

    off_t size = lseek(file_fd, 0L, SEEK_END);
    if (size < 0) {
        close(file_fd);
        return ESP_ERR_INVALID_SIZE;
    }

    // Check if file fits into the partition
    if (size > part->size) {
        close(file_fd);
        return ESP_ERR_INVALID_SIZE;
    }

    void *p_buff = malloc((size_t)size);
    if (p_buff == nullptr) {
        close(file_fd);
        return ESP_ERR_NO_MEM;
    }

    lseek(file_fd, 0L, SEEK_SET);
    ssize_t read_size = ::read(file_fd, p_buff, size);
    close(file_fd);

    if (read_size != size) {
        free(p_buff);
        return ESP_ERR_INVALID_SIZE;
    }

    esp_err_t ret = esp_partition_erase_range(part, 0, part->size);
    if (ret != ESP_OK) {
        free(p_buff);
        return ret;
    }

    ret = esp_partition_write(part, 0, p_buff, size);
    free(p_buff);
    return ret;
}

esp_err_t NVSPartitionTestHelper::check_marker(const char *part_name, const uint8_t *marker, size_t marker_len)
{
    if (marker == nullptr || marker_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    // Find the partition by name
    const esp_partition_t *part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name);

    if (part == nullptr) {
        return ESP_ERR_NOT_FOUND;
    }

    // Allocate buffer for reading partition data
    uint8_t *read_buf = (uint8_t*)malloc(part->size);
    if (read_buf == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    // Read the entire partition
    esp_err_t ret = esp_partition_read(part, 0, read_buf, part->size);
    if (ret != ESP_OK) {
        free(read_buf);
        return ret;
    }

    // Search for the marker in the partition data
    esp_err_t result = ESP_ERR_NVS_NOT_FOUND;
    for (size_t i = 0; i <= part->size - marker_len; i++) {
        if (memcmp(read_buf + i, marker, marker_len) == 0) {
            result = ESP_OK;
            break;
        }
    }

    free(read_buf);
    return result;
}

void NVSPartitionTestHelper::clear_stats(void)
{
    return esp_partition_clear_stats();
}

size_t NVSPartitionTestHelper::get_read_ops(void)
{
    return esp_partition_get_read_ops();
}

size_t NVSPartitionTestHelper::get_write_ops(void)
{
    return esp_partition_get_write_ops();
}

size_t NVSPartitionTestHelper::get_erase_ops(void)
{
    return esp_partition_get_erase_ops();
}

size_t NVSPartitionTestHelper::get_read_bytes(void)
{
    return esp_partition_get_read_bytes();
}

size_t NVSPartitionTestHelper::get_write_bytes(void)
{
    return esp_partition_get_write_bytes();
}

size_t NVSPartitionTestHelper::get_total_time(void)
{
    return esp_partition_get_total_time();
}

void NVSPartitionTestHelper::fail_after(size_t count, uint8_t mode)
{
    esp_partition_fail_after(count, mode);
}

#else   // CONFIG_NVS_BDL_STACK
// BDL implementation of NVSPartitionTestHelper
NVSPartitionTestHelper::NVSPartitionTestHelper(const char *part_name, const bool erase_partition) :
    // Call parent constructor with non-zero though invalid pointer to BDL handle, we will retrieve the correct one later
    nvs::NVSPartition(part_name, (esp_blockdev_handle_t)0x1 , false)
{
    // Retrieve the BDL handle for the partition
    esp_err_t err = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name,
        &mBDL);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get BDL handle for NVS partition %s: %s", part_name, esp_err_to_name(err));
        std::abort();
    }

    CHECK(mBDL != nullptr);

    // Increase the instance count
    s_instance_count++;

    // Remember the erase size of the BDL
    mOriginalEraseSize = mBDL->geometry.erase_size;

    if(erase_partition)
    {
        // erase the partition before testing
        CHECK(erase_range(0, get_size()) == ESP_OK);
    }
}

NVSPartitionTestHelper::~NVSPartitionTestHelper()
{
    // reset the erase size to the original value
    set_erase_size(mOriginalEraseSize);

    // Decrease the instance count
    s_instance_count--;
}

esp_err_t NVSPartitionTestHelper::load_from_file(const char *file_name)
{
    return load_from_file(get_partition_name(), file_name);
}

uint32_t NVSPartitionTestHelper::get_sectors()
{
    return get_size() / NVS_CONST_PAGE_SIZE;
}

esp_err_t NVSPartitionTestHelper::set_erase_size(const size_t size)
{
    // Set the new erase size in the BDL geometry
    mBDL->geometry.erase_size = size;

    // We have to set also the erase size in the esp_partition_t structure
    const esp_partition_t* part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        get_partition_name());

    if(part != nullptr) {
        ((esp_partition_t*)part)->erase_size = size;
        return ESP_OK;
    }

    return ESP_ERR_NOT_FOUND;
}

size_t NVSPartitionTestHelper::get_first_emulated_sector_index()
{
    // TODO: Change this implementation to reflect the concept of sectors counting for statistics once it is
    // implemented in BDL.

    // Current implementation assumes the esp_partition API and it's linux implementation evaluates statistics
    // based on the emulated flash sectors, which are defined by ESP_PARTITION_EMULATED_SECTOR_SIZE.
    // In order to evaluate the statistics correctly, we need to return index of the first sector number this
    // partition resides in the emulated flash.

    const esp_partition_t* part = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        get_partition_name());

    CHECK(part != nullptr);

    return part->address / ESP_PARTITION_EMULATED_SECTOR_SIZE;
}

size_t NVSPartitionTestHelper::get_sector_erase_count(size_t nvs_sector_index)
{
    // TODO: Once BDL implementation supports sector erase count tracking, we will need to re-implement

    // The esp_partition_get_sector_erase_count function is global across all partitions
    // it's sector index is relative to the emulated flash and the unit of the sector is defined by ESP_PARTITION_EMULATED_SECTOR_SIZE.
    // nvs_sector_index parameter is the index of the sector in the partition and the unit of the sector is defined by NVS_CONST_PAGE_SIZE.
    // Therefore, we need to convert the sector index to the emulated flash sector index.

    CHECK(nvs_sector_index < get_sectors());

    // The statistics are based on the emulated flash sectors, which are defined by ESP_PARTITION_EMULATED_SECTOR_SIZE.
    // Therefore, we need to convert the sector index to the emulated flash sector index.

    // Get the first sector index of the partition in the emulated flash
    size_t first_sector = get_first_emulated_sector_index();

    // Calculate the emulated sector index based on the NVS sector index (the unis are defined by NVS_CONST_PAGE_SIZE)
    size_t emulated_sector_index = (nvs_sector_index * NVS_CONST_PAGE_SIZE) / ESP_PARTITION_EMULATED_SECTOR_SIZE;

    // Call the esp_partition_get_sector_erase_count function with the emulated sector index
    return esp_partition_get_sector_erase_count(emulated_sector_index + first_sector);
}

// Static methods to make testing easier

esp_err_t NVSPartitionTestHelper::randomize_partition(const char *part_name, const uint32_t seed)
{
    // Erase the whole partition before randomizing
    esp_err_t ret = erase_partition(part_name);
    if (ret != ESP_OK) {
        return ret;
    }

    esp_blockdev_handle_t bdl_handle;

    // get the BDL handle for the partition
    ret = esp_partition_get_blockdev(
            ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_DATA_NVS,
            part_name,
            &bdl_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(seed);

    uint8_t buf[NVS_CONST_PAGE_SIZE];

    // loop over all sectors in the partition
    // and fill them with random data
    for (uint32_t i = 0; i < bdl_handle->geometry.disk_size / NVS_CONST_PAGE_SIZE; i++)
    {
        // fill the buffer with random data
        std::generate_n(buf, NVS_CONST_PAGE_SIZE, gen);

        // Write the random data to the partition using the BDL handle
        ret = bdl_handle->ops->write(bdl_handle, buf, i * NVS_CONST_PAGE_SIZE, NVS_CONST_PAGE_SIZE);

        if (ret != ESP_OK) {
            break;
        }
    }
    return ret;
}

// Erase the whole partition
esp_err_t NVSPartitionTestHelper::erase_partition(const char *part_name)
{
    // get the BDL handle for the partition
    esp_blockdev_handle_t bdl_handle;

    esp_err_t ret = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name,
        &bdl_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    // Erase the whole partition
    return bdl_handle->ops->erase(bdl_handle, 0, bdl_handle->geometry.disk_size);
}

// Get number of NVS sectors in the partition
uint32_t NVSPartitionTestHelper::get_sectors(const char *part_name)
{
    // get the BDL handle for the partition
    esp_blockdev_handle_t bdl_handle;

    esp_err_t ret = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name,
        &bdl_handle);

    if (ret != ESP_OK) {
        return 0;
    }

    return bdl_handle->geometry.disk_size / NVS_CONST_PAGE_SIZE;
}

// Load the partition from a file
esp_err_t NVSPartitionTestHelper::load_from_file(const char *part_name, const char *file_name)
{
    // get the BDL handle for the partition
    esp_blockdev_handle_t bdl_handle;

    esp_err_t ret = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name,
        &bdl_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    // Load the partition from file
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd == -1) {
        return ESP_ERR_NOT_FOUND;
    }

    off_t size = lseek(file_fd, 0L, SEEK_END);
    if (size < 0) {
        close(file_fd);
        return ESP_ERR_INVALID_SIZE;
    }

    // Check if file fits into the partition
    if (size > bdl_handle->geometry.disk_size) {
        close(file_fd);
        return ESP_ERR_INVALID_SIZE;
    }

    void *p_buff = malloc((size_t)size);
    if (p_buff == nullptr) {
        close(file_fd);
        return ESP_ERR_NO_MEM;
    }

    lseek(file_fd, 0L, SEEK_SET);
    ssize_t read_size = ::read(file_fd, p_buff, size);
    close(file_fd);

    if (read_size != size) {
        free(p_buff);
        return ESP_ERR_INVALID_SIZE;
    }

    ret = erase_partition(part_name);
    if (ret != ESP_OK) {
        free(p_buff);
        return ret;
    }

    ret = bdl_handle->ops->write(bdl_handle, (const uint8_t*) p_buff, 0, size);
    free(p_buff);
    return ret;
}

esp_err_t NVSPartitionTestHelper::check_marker(const char *part_name, const uint8_t *marker, size_t marker_len)
{
    if (marker == nullptr || marker_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    // Get the BDL handle for the partition
    esp_blockdev_handle_t bdl_handle;

    esp_err_t ret = esp_partition_get_blockdev(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_NVS,
        part_name,
        &bdl_handle);

    if (ret != ESP_OK) {
        return ret;
    }

    // Allocate buffer for reading partition data
    size_t read_size = bdl_handle->geometry.disk_size;
    uint8_t *read_buf = (uint8_t*)malloc(read_size);
    if (read_buf == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    do {
        // Read the entire partition
        ret = bdl_handle->ops->read(bdl_handle, read_buf, read_size, 0, read_size);
        if (ret != ESP_OK) {
            break;
        }

        // Search for the marker in the partition data
        ret = ESP_ERR_NVS_NOT_FOUND;
        for (size_t i = 0; i <= bdl_handle->geometry.disk_size - marker_len; i++) {
            if (memcmp(read_buf + i, marker, marker_len) == 0) {
                ret = ESP_OK;
                break;
            }
        }

    } while (false);

    free(read_buf);
    return ret;
}

void NVSPartitionTestHelper::clear_stats(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function

    return esp_partition_clear_stats();
}

size_t NVSPartitionTestHelper::get_read_ops(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function

    return esp_partition_get_read_ops();
}

size_t NVSPartitionTestHelper::get_write_ops(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function

    return esp_partition_get_write_ops();
}

size_t NVSPartitionTestHelper::get_erase_ops(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function

    return esp_partition_get_erase_ops();
}

size_t NVSPartitionTestHelper::get_read_bytes(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function
    return esp_partition_get_read_bytes();
}

size_t NVSPartitionTestHelper::get_write_bytes(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function

    return esp_partition_get_write_bytes();
}

size_t NVSPartitionTestHelper::get_total_time(void)
{
    // TODO: Once BDL implementation supports statistics, we will need to re-implement this function

    return esp_partition_get_total_time();
}

void NVSPartitionTestHelper::fail_after(size_t count, uint8_t mode)
{
    // TODO: Once BDL implementation supports programmed failures, we will need to re-implement this function

    esp_partition_fail_after(count, mode);
}
#endif
