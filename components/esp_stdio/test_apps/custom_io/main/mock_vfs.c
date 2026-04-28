/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "mock_vfs.h"

mock_vfs_state_t mock_a;
mock_vfs_state_t mock_b;

static int mock_open(void *ctx, const char *path, int flags, int mode)
{
    (void)path;
    (void)flags;
    (void)mode;
    mock_vfs_state_t *s = (mock_vfs_state_t *)ctx;
    s->open_count++;
    return s->next_fd;
}

static int mock_close(void *ctx, int fd)
{
    mock_vfs_state_t *s = (mock_vfs_state_t *)ctx;
    s->close_count++;
    s->last_closed_fd = fd;
    return 0;
}

static ssize_t mock_write(void *ctx, int fd, const void *data, size_t size)
{
    (void)fd;
    mock_vfs_state_t *s = (mock_vfs_state_t *)ctx;
    s->write_count++;
    size_t room = MOCK_VFS_BUF_SIZE - s->write_len;
    size_t n = size < room ? size : room;
    memcpy(s->write_buf + s->write_len, data, n);
    s->write_len += n;
    return (ssize_t)size;
}

static ssize_t mock_read(void *ctx, int fd, void *dst, size_t size)
{
    (void)fd;
    mock_vfs_state_t *s = (mock_vfs_state_t *)ctx;
    s->read_count++;
    size_t avail = s->read_len - s->read_pos;
    size_t n = size < avail ? size : avail;
    memcpy(dst, s->read_buf + s->read_pos, n);
    s->read_pos += n;
    return (ssize_t)n;
}

static int mock_fstat(void *ctx, int fd, struct stat *st)
{
    (void)ctx;
    (void)fd;
    if (st) {
        memset(st, 0, sizeof(*st));
    }
    return 0;
}

static int mock_fsync(void *ctx, int fd)
{
    (void)fd;
    mock_vfs_state_t *s = (mock_vfs_state_t *)ctx;
    s->fsync_count++;
    return 0;
}

const esp_vfs_fs_ops_t mock_vfs_ops_a = {
    .write_p = &mock_write,
    .open_p = &mock_open,
    .close_p = &mock_close,
    .read_p = &mock_read,
    .fstat_p = &mock_fstat,
    .fsync_p = &mock_fsync,
};

const esp_vfs_fs_ops_t mock_vfs_ops_b = {
    .write_p = &mock_write,
    .open_p = &mock_open,
    .close_p = &mock_close,
    .read_p = &mock_read,
    .fstat_p = &mock_fstat,
    .fsync_p = &mock_fsync,
};

void mock_vfs_reset(void)
{
    memset(&mock_a, 0, sizeof(mock_a));
    memset(&mock_b, 0, sizeof(mock_b));
    mock_a.next_fd = 10;
    mock_b.next_fd = 20;
    mock_a.last_closed_fd = -1;
    mock_b.last_closed_fd = -1;
}
