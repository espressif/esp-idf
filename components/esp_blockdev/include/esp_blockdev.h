/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file esp_blockdev.h
 * @brief Block Device Layer interface definition.
 *
 * This header defines public parts of the Block Device Layer interface (BDL).
 * It can be extended, but existing definitions are guaranteed to remain unchanged (open-closed principle).
 * See the in-code comments and README.md for more details.
 */

/**
 * @defgroup esp_blockdev_ioctl_cmds Block device ioctl commands
 *
 * Commands fall into two ranges: system (internal device management, not for typical application use; values 0-127)
 * and user (application-defined; values 128-255).
 *
 * @{
 */

#define ESP_BLOCKDEV_CMD_SYSTEM_BASE                0x00                                /*!< System commands base value */
#define ESP_BLOCKDEV_CMD_USER_BASE                  0x80                                /*!< User commands base value */

/**
 * @brief Mark a range as deleted or invalid (TRIM / discard)
 *
 * Tells the device or FTL that the range is no longer needed. Data may remain readable until the
 * space is reclaimed; behavior is analogous to TRIM on SSDs or discard on eMMC. Use
 * @ref ESP_BLOCKDEV_CMD_ERASE_CONTENTS when the caller requires the range to read as the device's
 * post-erase default value.
 *
 * Argument: a pointer to @ref esp_blockdev_cmd_arg_erase_t.
 *
 * @code{c}
 * esp_blockdev_cmd_arg_erase_t arg = {
 *     .start_addr = aligned_start,
 *     .erase_len = aligned_len,
 * };
 * esp_err_t ret = bdl->ops->ioctl(bdl, ESP_BLOCKDEV_CMD_MARK_DELETED, &arg);
 * @endcode
 */
#define ESP_BLOCKDEV_CMD_MARK_DELETED               (ESP_BLOCKDEV_CMD_SYSTEM_BASE + 0)

/**
 * @brief Erase a range and set contents to the default erased value
 *
 * Performs an erase (or equivalent) so that reads in the range return the device's default value
 * after erase (see the @c default_val_after_erase field in @ref esp_blockdev_flags_t). This is stronger than
 * @ref ESP_BLOCKDEV_CMD_MARK_DELETED, which only invalidates data from the perspective of the translation layer.
 *
 * Argument: a pointer to @ref esp_blockdev_cmd_arg_erase_t.
 *
 * @code{c}
 * esp_blockdev_cmd_arg_erase_t arg = {
 *     .start_addr = aligned_start,
 *     .erase_len = aligned_len,
 * };
 * esp_err_t ret = bdl->ops->ioctl(bdl, ESP_BLOCKDEV_CMD_ERASE_CONTENTS, &arg);
 * @endcode
 */
#define ESP_BLOCKDEV_CMD_ERASE_CONTENTS             (ESP_BLOCKDEV_CMD_SYSTEM_BASE + 1)

/** @} */

/**
 * @brief Input arguments for erase-related block-device ioctl commands
 *
 * Pass a pointer to this structure as the @c args argument to @ref esp_blockdev_ops_t::ioctl
 * when using @ref ESP_BLOCKDEV_CMD_MARK_DELETED or @ref ESP_BLOCKDEV_CMD_ERASE_CONTENTS.
 * Both @c start_addr and @c erase_len must be aligned to @ref esp_blockdev_geometry_t::erase_size
 * for the target device.
 */
typedef struct {
    /**
     * @brief Byte offset of the range from the start of the device
     *
     * Must be a multiple of @ref esp_blockdev_geometry_t::erase_size.
     */
    uint64_t start_addr;

    /**
     * @brief Length of the range in bytes
     *
     * Must be a multiple of @ref esp_blockdev_geometry_t::erase_size.
     */
    size_t erase_len;
} esp_blockdev_cmd_arg_erase_t;

/**
 * @brief Block device property flags (@c esp_blockdev_flags_t)
 *
 * Various properties and characteristics of a BDL device.
 * Represented as a @c struct when @c __DOXYGEN__ is defined (for documentation only) and as a @c union in normal builds.
 *
 * @note The macros @c ESP_BLOCKDEV_FLAGS_CONFIG_DEFAULT and @c ESP_BLOCKDEV_FLAGS_INST_CONFIG_DEFAULT provide a typical configuration for ESP-IDF components that expose a BDL interface. Use them as a starting point for custom initializers.
 */
#if defined(__DOXYGEN__)
typedef struct {
    bool read_only;                                 /*!< no erase/write operations allowed */
    bool encrypted;                                 /*!< the device data is encrypted */
    bool erase_before_write;                        /*!< erasing required before any write operation */
    bool and_type_write;                            /*!< 0-bits cannot be changed to 1-bits (NAND/NOR flash behavior) */
    bool default_val_after_erase;                   /*!< default bit value after erasing (0 or 1) */
    bool reserved[27];                              /*!< Reserved for future blockdev flags */
    uint32_t val;                                   /*!< Raw bitfield view for bulk initialization */
} esp_blockdev_flags_t;
#else
typedef union esp_blockdev_flags {
    struct {
        uint32_t read_only: 1;                      /*!< no erase/write operations allowed */
        uint32_t encrypted: 1;                      /*!< the device data is encrypted */
        uint32_t erase_before_write: 1;             /*!< erasing required before any write operation */
        uint32_t and_type_write: 1;                 /*!< 0-bits cannot be changed to 1-bits (NAND/NOR flash behavior) */
        uint32_t default_val_after_erase: 1;        /*!< default bit value after erasing (0 or 1) */
        uint32_t reserved: 27;                      /*!< Reserved for future blockdev flags */
    };
    uint32_t val;                                   /*!< Raw bitfield view for bulk initialization */
} esp_blockdev_flags_t;
#endif

/**
 * @brief Brace-enclosed initializer for @c esp_blockdev_flags_t with typical ESP-IDF defaults
 *
 * @see ESP_BLOCKDEV_FLAGS_INST_CONFIG_DEFAULT
 */
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

/**
 * @brief Assign typical ESP-IDF default values to an existing @c esp_blockdev_flags_t
 *
 * @param flags Flags instance to initialize (statement form, not an expression)
 *
 * @see ESP_BLOCKDEV_FLAGS_CONFIG_DEFAULT
 */
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
 * Granularity and capacity parameters for read, write, and erase operations on the device.
 */
typedef struct {
    /**
     * @brief Total addressable size of the device in bytes
     *
     * Mandatory for every block device.
     */
    uint64_t disk_size;

    /**
     * @brief Minimum read transaction size in bytes
     *
     * Mandatory. All read lengths and offsets must respect this granularity where required by the driver.
     */
    size_t read_size;

    /**
     * @brief Minimum write transaction size in bytes
     *
     * Mandatory for read/write devices; set to @c 0 for read-only devices.
     */
    size_t write_size;

    /**
     * @brief Minimum erase granularity in bytes
     *
     * Mandatory for read/write devices; set to @c 0 for read-only devices.
     *
     * @note Often corresponds to the logical sector size used by file systems.
     */
    size_t erase_size;

    /**
     * @brief Suggested write chunk size in bytes for best performance
     *
     * Set to @c 0 if not used. Optional hint for upper layers.
     */
    size_t recommended_write_size;

    /**
     * @brief Suggested read chunk size in bytes for best performance
     *
     * Set to @c 0 if not used. Optional hint for upper layers.
     */
    size_t recommended_read_size;

    /**
     * @brief Suggested erase chunk size in bytes for best performance
     *
     * Set to @c 0 if not used. Optional hint for upper layers.
     *
     * @note Often matches the physical erase block of the underlying flash or similar media.
     */
    size_t recommended_erase_size;

} esp_blockdev_geometry_t;

/** Forward declaration; see @ref esp_blockdev */
typedef struct esp_blockdev esp_blockdev_t;

/**
 * @brief Opaque handle to a block device instance
 *
 * Pointer to @ref esp_blockdev. This is the only public identifier for a BDL instance: it is returned
 * from the device factory and must be released through @ref esp_blockdev_ops_t::release when no longer needed.
 */
typedef esp_blockdev_t* esp_blockdev_handle_t;

/**
 * @brief Sentinel for an invalid block device handle
 *
 * Compare handles to @c NULL; this macro expands to @c NULL for readability.
 */
#define ESP_BLOCKDEV_HANDLE_INVALID NULL

/**
 * @struct esp_blockdev_ops_t
 * @brief Block device operations
 *
 * Function table for read, write, erase, sync, ioctl, and release on a block device instance.
 */
typedef struct {

    /**
     * @brief Read data from the device
     *
     * Reads the requested number of bytes from the device at the given offset into the output buffer.
     *
     * @par Parameters
     * - @a dev_handle Target device handle
     * - @a dst_buf Output buffer to receive the data read
     * - @a dst_buf_size Size of the destination buffer (in bytes)
     * - @a src_addr Source address for the read (offset in the device's address space)
     * - @a data_read_len Data read length (in bytes)
     *
     * @par Return value
     * - ESP_OK on success
     * - Another @c esp_err_t error code on failure
     */
    esp_err_t (*read)(esp_blockdev_handle_t dev_handle, uint8_t* dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len);

    /**
     * @brief Write data to the device
     *
     * Writes the requested number of bytes from the input buffer to the device at the given offset.
     *
     * @par Parameters
     * - @a dev_handle Target device handle
     * - @a src_buf Input buffer providing the data to write
     * - @a dst_addr Destination address for the write (offset in the device's address space)
     * - @a data_write_len Length of the data to be written (in bytes)
     *
     * @par Return value
     * - ESP_OK on success
     * - Another @c esp_err_t error code on failure
     */
    esp_err_t (*write)(esp_blockdev_handle_t dev_handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len);

    /**
     * @brief Erase an address range on the device
     *
     * Erases the address range @c [start_addr, start_addr + erase_len).
     * Both @c erase_len and @c start_addr must be aligned to multiples of the erase block size in bytes (@c erase_size in @ref esp_blockdev_geometry_t).
     * The implementation may use one or more internal operations, depending on the device.
     * The result must be that data in the range is no longer readable as previously written content.
     *
     * @par Parameters
     * - @a dev_handle Target device handle
     * - @a start_addr Start address for the erase operation
     * - @a erase_len Length of the address range to erase (in bytes)
     *
     * @par Return value
     * - ESP_OK on success
     * - Another @c esp_err_t error code on failure
     */
    esp_err_t (*erase)(esp_blockdev_handle_t dev_handle, uint64_t start_addr, size_t erase_len);

    /**
     * @brief Commit pending writes to the device
     *
     * Commits all pending writes and blocks until they complete.
     * Does nothing if the device does not cache writes.
     *
     * @par Parameters
     * - @a dev_handle Target device handle
     *
     * @par Return value
     * - ESP_OK on success
     * - Another @c esp_err_t error code on failure
     */
    esp_err_t (*sync)(esp_blockdev_handle_t dev_handle);

    /**
     * @brief Device-specific I/O control
     *
     * Each command has parameters that the handler must validate.
     * Implementations may extend commands within these ranges:
     * - 0x00-0x7F: IDF device commands (system)
     * - 0x80-0xFF: user-defined commands
     *
     * Refer to the @c ESP_BLOCKDEV_CMD_ macros in this header.
     *
     * @par Parameters
     * - @a dev_handle Target device handle
     * - @a cmd Command ID
     * - @a args Command-specific arguments
     *
     * @par Return value
     * - ESP_OK on success
     * - Another @c esp_err_t error code on failure
     */
    esp_err_t (*ioctl)(esp_blockdev_handle_t dev_handle, const uint8_t cmd, void* args);

    /**
     * @brief Release resources for this device handle
     *
     * @par Parameters
     * - @a dev_handle Target device handle
     *
     * @par Return value
     * - ESP_OK on success
     * - Another @c esp_err_t error code on failure
     */
    esp_err_t (*release)(esp_blockdev_handle_t dev_handle);

} esp_blockdev_ops_t;

/**
 * @struct esp_blockdev
 * @brief Generic IDF block device interface
 *
 * Describes a block device that supports common disk operations and exposes parameters needed to integrate it into a component stack.
 *
 * @note The context pointer @c ctx holds driver-private data; the BDL layer does not interpret it.
 * @note The @c ops table may be shared by multiple instances of the same driver type.
 */
struct esp_blockdev {

    void* ctx;                                      /*!< Owner-provided context; not interpreted by the BDL layer */

    esp_blockdev_flags_t device_flags;              /*!< Capabilities and requirements */
    esp_blockdev_geometry_t geometry;               /*!< Operation granularity and capacity */
    const esp_blockdev_ops_t* ops;                  /*!< Function table implementing the device */

};

#ifdef __cplusplus
} // extern "C"
#endif
