/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_vfs.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Special case function for registering a VFS that uses a method other than
 * open() to open new file descriptors from the interval <min_fd; max_fd).
 *
 * This is a special-purpose function intended for registering LWIP sockets to VFS.
 *
 * @param vfs Pointer to esp_vfs_fs_ops_t. Meaning is the same as for esp_vfs_register_fs().
 * @param ctx Pointer to context structure. Meaning is the same as for esp_vfs_register_fs().
 * @param min_fd The smallest file descriptor this VFS will use.
 * @param max_fd Upper boundary for file descriptors this VFS will use (the biggest file descriptor plus one).
 *
 * @return  ESP_OK if successful,
 *          ESP_ERR_NO_MEM if too many VFSes are registered,
 *          ESP_ERR_INVALID_ARG if the file descriptor boundaries are incorrect.
 */
esp_err_t esp_vfs_register_fd_range(const esp_vfs_fs_ops_t *vfs, int flags, void *ctx, int min_fd, int max_fd);

#ifdef __cplusplus
}
#endif
