/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @file esp_blockdev.h
 * @brief Block Device Layer interface definition.
 *
 * This header defines public parts of the Block Device Layer interface (BDL).
 * It can be extended but existing definitions are guaranteed to stay unchanged (Open-Closed Principle).
 * See the in-code comments and README.md for more details.
 */

/**
 * @brief Block device I/O control commands
 */
typedef enum {
    ESP_BLOCKDEV_CMD_ERASE,                         /*!< perform device specific ERASE operation */
    ESP_BLOCKDEV_CMD_TRIM,                          /*!< perform device specific TRIM operation */
    ESP_BLOCKDEV_CMD_DISCARD,                       /*!< perform device specific DISCARD operation */
    ESP_BLOCKDEV_CMD_SANITIZE,                      /*!< perform device specific SANITIZE operation */
    ESP_BLOCKDEV_CMD_SECTOR_SIZE,                   /*!< get device sector size (not necessarily equal to specific block size) */
    ESP_BLOCKDEV_CMD_DBG_INFO,                      /*!< get device debug information */
    ESP_BLOCKDEV_CMD_STATISTICS                     /*!< get device operation statistics */
} esp_blockdev_ioctl_cmd_t;

typedef struct esp_blockdev_cmd_arg_erase_t {
    uint64_t start_addr;                            /*!< IN  - starting address of the disk space to erase/trim/discard/sanitize (in bytes), must be a multiple of erase block size */
    size_t erase_len;                               /*!< IN  - size of the area to erase/trim/discard/sanitize (in bytes), must be a multiple of erase block size */
} esp_blockdev_cmd_arg_erase_t;

typedef struct esp_blockdev_cmd_arg_dbginfo_t esp_blockdev_cmd_arg_dbginfo_t;
typedef struct esp_blockdev_cmd_arg_dbginfo_t {
    uint32_t device_bdl_version;                    /*!< OUT - device BDL interface version */
    bool get_device_chain;                          /*!< IN  - require debug info from the underlying device (true/false) */
    esp_blockdev_cmd_arg_dbginfo_t* bottom_device;  /*!< OUT - optional parameter to retrieve the debug info of the underlying device.
                                                               Used when 'get_device_chain' flag is True. To be freed by the topmost caller */
} esp_blockdev_cmd_arg_dbginfo_t;

typedef struct esp_blockdev_cmd_arg_stat_t {
    uint64_t available_space;                       /*!< OUT - remaining disk space (in bytes) */
} esp_blockdev_cmd_arg_stat_t;

/**
 * @brief Block device property flags
 *
 *  Various properties and characteristics of given BDL device.
 *
 *  @note Convenience macros ESP_BLOCKDEV_FLAGS_<INST>_CONFIG_DEFAULT() provide the most common setup of usual ESP-IDF component equipped with BDL interface. They can be used as a starting point for own initializers.
 */
typedef union {
    struct {
        uint32_t read_only: 1;                      /*!< no erase/write operations allowed */
        uint32_t encrypted: 1;                      /*!< the device data is encrypted */
        uint32_t erase_before_write: 1;             /*!< erasing required before any write operation */
        uint32_t and_type_write: 1;                 /*!< 0-bits can't be changed to 1-bits - NAND/NOR flash behavior */
        uint32_t default_val_after_erase: 1;        /*!< default bit value after erasing (0 or 1) */
        uint32_t reserved: 27;
    };
    uint32_t val;
} esp_blockdev_flags_t;

#define ESP_BLOCKDEV_FLAGS_CONFIG_DEFAULT() {  \
    {  \
        .read_only   = 0,  \
        .encrypted   = 0,  \
        .erase_before_write = 1,  \
        .and_type_write = 1,  \
        .default_val_after_erase = 1,  \
        .reserved = 0                      \
    } \
}

#define ESP_BLOCKDEV_FLAGS_INST_CONFIG_DEFAULT(flags) {  \
        flags.read_only   = 0;  \
        flags.encrypted   = 0;  \
        flags.erase_before_write = 1;  \
        flags.and_type_write = 1;  \
        flags.default_val_after_erase = 1;  \
        flags.reserved = 0;  \
}

/**
 * @brief Block device geometry
 *
 * Various block size parameters needed for proper R/W/E processing on given device.
 */
typedef struct esp_blockdev_geometry_t {

    /** Size of the device disk space (in bytes).
     *  Mandatory parameter.
     *  */
    uint64_t disk_size;

    /** Minimum block size (in bytes) for disk-read operations on given device.
     *  Mandatory parameter.
     *  */
    size_t read_size;

    /** Minimum block size (in bytes) for disk-write operations on given device.
     *  Mandatory parameter for all R/W devices, 0 for R/O.
     *  */
    size_t write_size;

    /** Minimum block size (in bytes) for erase operations on given device.
     *  Mandatory parameter for all R/W devices, 0 for R/O.
     *  */
    size_t erase_size;

    /** Default write block size (in bytes) of given device. Recommended for optimal performance.
     *  0 means not used.
     *  */
    size_t recommended_write_size;

    /** Default read block size (in bytes) of given device. Recommended for optimal performance.
     *  0 means not used.
     *  */
    size_t recommended_read_size;

    /** Default erase block size (in bytes) of given device. Recommended for optimal performance.
     *  0 means not used.
     *  */
    size_t recommended_erase_size;

} esp_blockdev_geometry_t;

/** Standard BDL access handle, the only public BDL instance identifier.
 *  Allocated and initialized by the device's class factory, optionally closed by the device release handler.
 */
typedef struct esp_blockdev_t* esp_blockdev_handle_t;

/**
 * @brief IDF generic Block device interface structure
 *  This structure defines an interface for a generic block-device capable of common disk operations and providing properties important
 *  for the device's deployment into the target component stack.
 */
typedef struct esp_blockdev_t {

    /* Device flags */
    esp_blockdev_flags_t device_flags;

    /* Block device geometry parameters */
    esp_blockdev_geometry_t geometry;

    /** READ operation:
     *   Read required number of bytes from the device at given offset, store the data into the output buffer.
     *
     * @param dev_handle Target device handle
     * @param dst_buf Output buffer to receive the data read
     * @param dst_buf_size Size of the destination buffer (in bytes)
     * @param src_addr Data read source address (offset in given device address space)
     * @param data_read_len Data read length (in bytes)
     * */
    esp_err_t (*read)(esp_blockdev_handle_t dev_handle, uint8_t* dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len);

    /** WRITE operation:
     *   Write required number of bytes taken from the input memory buffer to the device at given offset.
     *
     * @param dev_handle Target device handle
     * @param src_buf Input buffer providing the data to write
     * @param dst_addr Data destination address (offset in given device address space)
     * @param data_write_len Length of the data to be written (in bytes)
     * */
    esp_err_t (*write)(esp_blockdev_handle_t dev_handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len);

    /** ERASE operation:
     *   Erase given address range <start_addr, start_addr + erase_len>.
     *   Both erase_len and start_addr must be aligned to multiples of the erase block size (in bytes) given by 'erase_size' member.
     *   The erasing process can be secured by one or more operations, depending on given BDL device nature.
     *   The only expected result is safely wiped data within the required range.
     *
     * @param dev_handle Target device handle
     * @param start_addr Start address for the erase operation
     * @param erase_len Length of the address space chunk to be erased (in bytes)
     * */
    esp_err_t (*erase)(esp_blockdev_handle_t dev_handle, uint64_t start_addr, size_t erase_len);

    /** SYNC operation:
     *  Commit all the pending write operations and block until all are finished.
     *  Does nothing if the device doesn't support write operations caching
     *
     * @param dev_handle Target device handle
     * */
    esp_err_t (*sync)(esp_blockdev_handle_t dev_handle);

    /** IOCTL operation:
     *   I/O control commands. Each command has corresponding in/out parameters which are to be verified
     *   within given ioctl handler.
     *
     * @param dev_handle Target device handle
     * @param cmd Command ID as given by esp_block_dev_ioctl_cmd enumeration
     * @param args Command specific arguments, see esp_block_dev_ioctl_cmd details
     * */
    esp_err_t (*ioctl)(esp_blockdev_handle_t dev_handle, const esp_blockdev_ioctl_cmd_t cmd, void* args);

} esp_blockdev_t;

#ifdef __cplusplus
} // extern "C"
#endif
