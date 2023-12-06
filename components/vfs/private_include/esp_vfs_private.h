/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_vfs.h"
#include "esp_vfs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_VFS_SELECT_IN_RAM
#define VFS_MALLOC_FLAGS (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define VFS_MALLOC_FLAGS (MALLOC_CAP_DEFAULT)
#endif

typedef struct vfs_entry_ {
    esp_vfs_t vfs;          // contains pointers to VFS functions
    char path_prefix[ESP_VFS_PATH_MAX]; // path prefix mapped to this VFS
    size_t path_prefix_len; // micro-optimization to avoid doing extra strlen
    void* ctx;              // optional pointer which can be passed to VFS
    int offset;             // index of this structure in s_vfs array
} vfs_entry_t;


/**
 * @brief get pointer of uart vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of uart.
 *
 * @return pointer to structure esp_vfs_t
 */
const esp_vfs_t *esp_vfs_uart_get_vfs(void);

/**
 * @brief get pointer of cdcacm vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of cdcacm.
 *
 * @return pointer to structure esp_vfs_t
 */
const esp_vfs_t *esp_vfs_cdcacm_get_vfs(void);

/**
 * @brief get pointer of usb_serial_jtag vfs.
 *
 * This function is called in vfs_console in order to get the vfs implementation
 * of usb_serial_jtag.
 *
 * @return pointer to structure esp_vfs_nonblocking_console_t
 */
const esp_vfs_t *esp_vfs_usb_serial_jtag_get_vfs(void);

/**
 * Register a virtual filesystem.
 *
 * @param base_path  file path prefix associated with the filesystem.
 *                   Must be a zero-terminated C string, may be empty.
 *                   If not empty, must be up to ESP_VFS_PATH_MAX
 *                   characters long, and at least 2 characters long.
 *                   Name must start with a "/" and must not end with "/".
 *                   For example, "/data" or "/dev/spi" are valid.
 *                   These VFSes would then be called to handle file paths such as
 *                   "/data/myfile.txt" or "/dev/spi/0".
 *                   In the special case of an empty base_path, a "fallback"
 *                   VFS is registered. Such VFS will handle paths which are not
 *                   matched by any other registered VFS.
 * @param len  Length of the base_path.
 * @param vfs  Pointer to esp_vfs_t, a structure which maps syscalls to
 *             the filesystem driver functions. VFS component doesn't
 *             assume ownership of this pointer.
 * @param ctx  If vfs->flags has ESP_VFS_FLAG_CONTEXT_PTR set, a pointer
 *             which should be passed to VFS functions. Otherwise, NULL.
 * @param vfs_index Index for getting the vfs content.
 *
 * @return  ESP_OK if successful.
 *          ESP_ERR_NO_MEM if too many VFSes are registered.
 *          ESP_ERR_INVALID_ARG if given an invalid parameter.
 */
esp_err_t esp_vfs_register_common(const char *base_path, size_t len, const esp_vfs_t* vfs, void* ctx, int *vfs_index);

/**
 * Get vfs fd with given path.
 *
 * @param path file path prefix associated with the filesystem.
 *
 * @return Pointer to the `vfs_entry_t` corresponding to the given path, which cannot be NULL.
 */
const vfs_entry_t *get_vfs_for_path(const char *path);

/**
 * Get vfs fd with given vfs index.
 *
 * @param index VFS index.
 *
 * @return Pointer to the `vfs_entry_t` corresponding to the given path, which cannot be NULL.
 */
const vfs_entry_t *get_vfs_for_index(int index);

#ifdef __cplusplus
}
#endif
