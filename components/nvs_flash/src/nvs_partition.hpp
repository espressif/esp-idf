/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"                  // For CONFIG_NVS_BDL_STACK
#include "partition.hpp"

#ifdef CONFIG_NVS_BDL_STACK
    #include "esp_blockdev.h"           // For esp_blockdev_handle_t
    #include "nvs.h"                    // For NVS_PART_NAME_MAX_SIZE
#else
    #include "esp_partition.h"
#endif // CONFIG_NVS_BDL_STACK

namespace nvs {

/**
 * Implementation of non-encrypted esp_partition or bdl storage wrapper for NVS.
 */
class NVSPartition : public Partition {
public:
#ifdef CONFIG_NVS_BDL_STACK
    /**
     * Constructor copies storage name to mLabel and initializes BDL reference.
     * See function is_bdl_nvs_compliant for the requirements on the BDL.
     *
     * @param label the label of the partition corresponding to the referenced BDL handle.
     * @param bdl an already initialized bdl handle
     * @param managed_bdl if true, the BDL handle will be released in the destructor of this class.
     */
    NVSPartition(const char* label, const esp_blockdev_handle_t bdl, const bool managed_bdl = true);

    /**
     * Function allowing to check if the block device is compliant with NVS requirements.
     * This static function shall be called prior to creating an instance of NVSPartition.
     * Following checks are performed:
     * 1. The block device handle must not be null
     * 2. The block device label must not be null
     * 3. The block device label must fit to the statically allocated buffer (NVS_PART_NAME_MAX_SIZE)
     * 4. The block device read block size must be 1
     * 5. The block device write block size must be 1
     * 6. The block device erase block size must be a multiple of NVS_CONST_PAGE_SIZE (NVS_CONST_PAGE_SIZE is 4kB)
     * 7. The size of the block device must be a multiple of the NVS_CONST_PAGE_SIZE (NVS_CONST_PAGE_SIZE is 4kB)
     * 8. BDL operations read / write / erase must be supported
     * 9. The block device write operation type must be at least NOR flash compliant. Regular write, or write setting to zero are supported.
     * 10. The block device must not be encrypted
     *
     * @param label the label of the storage corresponding to the referenced BDL handle.
     * @param bdl an already initialized bdl handle
     *
     * @return true if the block device is NVS compliant and can be used for NVS operations, false otherwise.
     */
    static bool is_bdl_nvs_compliant(const char* label, const esp_blockdev_handle_t bdl);

#else
     /**
     * Constructor initializes the mPartition.
     *
     * @param partition an already initialized esp_partition structure
     */
    NVSPartition(const esp_partition_t* partition);
#endif

    /**
     * Destructor cleans up the wrapper.
     * If BDL is used and constructor was called with managed_bdl = true,
     * the BDL handle will be released.
     */
    virtual ~NVSPartition();

    /**
     * Return the name (label) of the storage.
     * In esp_partition implementation it returns the partition label.
     * In BDL implementation it returns the BDL label handed over to the constructor.
     *
     * @return
     *      - the label of the storage
     */
    const char *get_partition_name() override;

    /**
     * Reads data from the storage.
     * Raw read is used for reading data aligned down to the size of 1 byte
     *
     * @param src_offset the offset in the storage to read from
     * @param dst an already allocated buffer to read data into
     * @param size the size of the data to read in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition or BDL API
     */
    esp_err_t read_raw(size_t src_offset, void* dst, size_t size) override;

    /**
     * Reads data from the storage
     * This function is intended for reading data aligned to the size of NVS entry.
     * This, not encrypted, implementation does not require the size to be aligned to the NVS entry size though.
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
     * Writes data from the src buffer to the storage.
     * Raw write is used for writing data aligned down to the size of 1 byte
     * Even repeated writes to the same address can be called.
     *
     * @param dst_offset the offset in the storage to write to
     * @param src pointer to the buffer to write data from
     * @param size the size of the data to write in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition or BDL API
     */
    esp_err_t write_raw(size_t dst_offset, const void* src, size_t size) override;

    /**
     * Writes data from the src buffer to the storage.
     * This function is intended for writing data aligned to the size of NVS entry.
     * This, not encrypted, implementation does not require the size to be aligned to the NVS entry size though.
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

    /**
     * Erases the address range in the storage.
     * Erased data is set to 0xFF.
     *
     * @param dst_offset the offset in the storage to erase
     * @param size the size of the data to erase in bytes
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition or BDL API
     */
    esp_err_t erase_range(size_t dst_offset, size_t size) override;

    /**
     * Returns the RAM address of the beginning of the memory mapped storage.
     * Not available if the block device layer is enabled.
     *
     * @return
     *      - ESP_OK on success
     *      - other error codes from the esp_partition or BDL API
     */
    uint32_t get_address() override;

    /**
     * Returns total size of the storage in bytes.
     *
     * @return size of the storage in bytes.
     */
    uint32_t get_size() override;

    /**
     * Returns the read-only status of the storage.
     *
     * @return true if the storage is read-only.
     */
    bool get_readonly() override;

protected:

#ifdef CONFIG_NVS_BDL_STACK
    esp_blockdev_handle_t mBDL;                         // Block Device Layer handle for the storage
    char mLabel[NVS_PART_NAME_MAX_SIZE + 1];            // Label of the storage, used for matching in NVS open operations
    bool mManagedBDLbyInstance;                         // True if the BDL is managed by this instance, false if it is managed externally
#else
    const esp_partition_t* mESPPartition;               // Pointer to the ESP partition structure for the storage
#endif // CONFIG_NVS_BDL_STACK
};

} // nvs
