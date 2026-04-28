/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "esp_vfs_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOCK_VFS_BUF_SIZE 256

/**
 * @brief In-memory fake VFS backend used to exercise the esp_stdio console mux.
 *
 * Every callback records that it was invoked so tests can assert exactly how
 * the mux dispatched the operation. Writes are appended to @c write_buf; reads
 * are served from @c read_buf.
 */
typedef struct {
    int open_count;                       /*!< number of open_p() calls */
    int close_count;                      /*!< number of close_p() calls */
    int write_count;                      /*!< number of write_p() calls */
    int read_count;                       /*!< number of read_p() calls */
    int fsync_count;                      /*!< number of fsync_p() calls */
    int next_fd;                          /*!< fd returned by next open_p() */
    int last_closed_fd;                   /*!< fd passed to the last close_p() */
    char write_buf[MOCK_VFS_BUF_SIZE];    /*!< accumulated written bytes */
    size_t write_len;                     /*!< bytes stored in write_buf */
    char read_buf[MOCK_VFS_BUF_SIZE];     /*!< bytes served by read_p() */
    size_t read_len;                      /*!< bytes available in read_buf */
    size_t read_pos;                      /*!< read cursor into read_buf */
} mock_vfs_state_t;

/* Two independent backend instances + ops tables, enough for primary + aux tests. */
extern mock_vfs_state_t mock_a;
extern mock_vfs_state_t mock_b;

extern const esp_vfs_fs_ops_t mock_vfs_ops_a;
extern const esp_vfs_fs_ops_t mock_vfs_ops_b;

/** @brief Reset both mock backends to a clean state. */
void mock_vfs_reset(void);

#ifdef __cplusplus
}
#endif
