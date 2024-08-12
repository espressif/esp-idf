/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include <stdio.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <fcntl.h>
#include "esp_vfs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_private/startup_internal.h"

#include "esp_vfs_null.h"

#define UNUSED(x) (void)(x)
typedef enum {
    VFS_NULL_CLOSED = 0,
    VFS_NULL_READ = 1 << 0,
    VFS_NULL_WRITE = 1 << 1,
} vfs_null_flags_t;

typedef uint32_t vfs_null_ctx_t;
#define VFS_NULL_MAX_FDS (sizeof(vfs_null_ctx_t) * 4)

#define GET_FLAGS(ctx, fd) ((ctx >> (fd * 2)) & 0x3)
#define SET_FLAGS(ctx, fd, flags) (ctx |= (flags << (fd * 2)))

#define WRITABLE(ctx, fd) (GET_FLAGS(ctx, fd) & VFS_NULL_WRITE)
#define READABLE(ctx, fd) (GET_FLAGS(ctx, fd) & VFS_NULL_READ)

#define SET_WRITABLE(ctx, fd) SET_FLAGS(ctx, fd, VFS_NULL_WRITE)
#define SET_READABLE(ctx, fd) SET_FLAGS(ctx, fd, VFS_NULL_READ)

#define FD_IN_RANGE(fd) (fd >= 0 && fd < VFS_NULL_MAX_FDS)
#define IS_FD_VALID(fd) (FD_IN_RANGE(fd) && GET_FLAGS(g_fds, fd) != VFS_NULL_CLOSED)

#ifdef CONFIG_ESP_CONSOLE_NONE // Prevents recursive logging
static const char* TAG = "nullfs";
#define NULLFS_LOGD(TAG, fmt, ...) ESP_LOGD(TAG, fmt, ##__VA_ARGS__)
#else
#define NULLFS_LOGD(TAG, fmt, ...)
#endif

static vfs_null_ctx_t g_fds = 0;

static ssize_t vfs_null_write(int fd, const void *data, size_t size);
static off_t vfs_null_lseek(int fd, off_t offset, int whence);
static ssize_t vfs_null_read(int fd, void *data, size_t size);
static ssize_t vfs_null_pread(int fd, void *data, size_t size, off_t offset);
static ssize_t vfs_null_pwrite(int fd, const void *data, size_t size, off_t offset);
static int vfs_null_open(const char* path, int flags, int mode);
static int vfs_null_close(int fd);
static int vfs_null_fstat(int fd, struct stat *st);
#if CONFIG_VFS_SUPPORT_DIR
static int vfs_null_stat(const char* path, struct stat *st);
#endif // CONFIG_VFS_SUPPORT_DIR
static int vfs_null_fcntl(int fd, int cmd, int arg);
static int vfs_null_ioctl(int fd, int cmd, va_list args);
static int vfs_null_fsync(int fd);

static const esp_vfs_t s_vfs_null = {
    .flags = ESP_VFS_FLAG_DEFAULT,
    .write = &vfs_null_write,
    .lseek = &vfs_null_lseek,
    .read = &vfs_null_read,
    .pread = &vfs_null_pread,
    .pwrite = &vfs_null_pwrite,
    .open = &vfs_null_open,
    .close = &vfs_null_close,
    .fstat = &vfs_null_fstat,
#if CONFIG_VFS_SUPPORT_DIR
    .stat = &vfs_null_stat,
#endif // CONFIG_VFS_SUPPORT_DIR
    .fcntl = &vfs_null_fcntl,
    .ioctl = &vfs_null_ioctl,
    .fsync = &vfs_null_fsync,
};

const esp_vfs_t *esp_vfs_null_get_vfs(void)
{
    return &s_vfs_null;
}

esp_err_t esp_vfs_null_register(void)
{
    return esp_vfs_register("/dev/null", &s_vfs_null, NULL);
}

static ssize_t vfs_null_write(int fd, const void *data, size_t size)
{
    UNUSED(data);

    if (FD_IN_RANGE(fd) && WRITABLE(g_fds, fd)) {
        return size;
    }

    errno = EBADF;
    return -1;
}

static off_t vfs_null_lseek(int fd, off_t offset, int whence)
{
    UNUSED(offset);

    if (!IS_FD_VALID(fd)) {
        errno = EBADF;
        return -1;
    }

    switch (whence) {
    case SEEK_SET:
    case SEEK_CUR:
    case SEEK_END:
        return 0;
    default:
        errno = EINVAL;
        return -1;
    }
}

static ssize_t vfs_null_read(int fd, void *data, size_t size)
{
    UNUSED(data);

    NULLFS_LOGD(TAG, "read %u bytes", size);

    if (FD_IN_RANGE(fd) && READABLE(g_fds, fd)) {
        return 0; // EOF
    }

    errno = EBADF;
    return -1;
}

static int vfs_null_pread(int fd, void *data, size_t size, off_t offset)
{
    UNUSED(data);
    UNUSED(size);
    UNUSED(offset);

    NULLFS_LOGD(TAG, "pread %u bytes at offset %ld", size, offset);

    if (!FD_IN_RANGE(fd) || !READABLE(g_fds, fd)) {
        errno = EBADF;
        return -1;
    }

    return 0; // EOF

}

static int vfs_null_pwrite(int fd, const void *data, size_t size, off_t offset)
{
    UNUSED(data);
    UNUSED(offset);

    NULLFS_LOGD(TAG, "pwrite %u bytes at offset %ld", size, offset);

    if (!FD_IN_RANGE(fd) || !WRITABLE(g_fds, fd)) {
        errno = EBADF;
        return -1;
    }

    return size;
}

static int vfs_null_get_empty_fd(void)
{
    for (int i = 0; i < VFS_NULL_MAX_FDS; i++) {
        if (GET_FLAGS(g_fds, i) == VFS_NULL_CLOSED) {
            return i;
        }
    }

    return -1;
}

static int vfs_null_open(const char* path, int flags, int mode)
{
    UNUSED(mode);

    NULLFS_LOGD(TAG, "open %s, flags %d", path, flags);

    // Possibly check if the flags are valid

    if (strcmp(path, "/") != 0) {
        errno = ENOENT;
        return -1;
    }

    int fd = vfs_null_get_empty_fd();
    if (fd == -1) {
        errno = EMFILE;
        return -1;
    }

    int acc_mode = flags & O_ACCMODE;
    if (acc_mode == O_RDWR) {
        SET_READABLE(g_fds, fd);
        SET_WRITABLE(g_fds, fd);
    } else if (acc_mode == O_WRONLY) {
        SET_WRITABLE(g_fds, fd);
    } else if (acc_mode == O_RDONLY) {
        SET_READABLE(g_fds, fd);
    } else {
        errno = EINVAL;
        return -1;
    }

    return fd;
}

static int vfs_null_close(int fd)
{
    if (!FD_IN_RANGE(fd)) {
        errno = EBADF;
        return -1;
    }

    SET_FLAGS(g_fds, fd, VFS_NULL_CLOSED);
    return 0;
}

static int vfs_null_fstat(int fd, struct stat *st)
{
    if (!FD_IN_RANGE(fd)) {
        errno = EBADF;
        return -1;
    }

    memset(st, 0, sizeof(struct stat));
    st->st_mode = S_IFCHR | 0666; // Special character device with read/write permissions for everyone
    st->st_nlink = 1;
    st->st_uid = 0;
    st->st_gid = 0;
    st->st_size = 0;

    return 0;
}

#if CONFIG_VFS_SUPPORT_DIR

static int vfs_null_stat(const char* path, struct stat *st)
{
    if (strcmp(path, "/") != 0) {
        errno = ENOENT;
        return -1;
    }

    memset(st, 0, sizeof(struct stat));
    st->st_mode = S_IFCHR | 0666; // Special character device with read/write permissions for everyone
    st->st_nlink = 1;
    st->st_uid = 0;
    st->st_gid = 0;
    st->st_size = 0;

    return 0;
}

#endif // CONFIG_VFS_SUPPORT_DIR

static int vfs_null_fcntl(int fd, int cmd, int arg)
{
    UNUSED(arg);

    if (!FD_IN_RANGE(fd)) {
        errno = EBADF;
        return -1;
    }

    switch (cmd) {
    default:
        errno = ENOSYS;
        return -1;
    }
}

static int vfs_null_ioctl(int fd, int cmd, va_list args)
{
    UNUSED(args);

    if (!FD_IN_RANGE(fd)) {
        errno = EBADF;
        return -1;
    }

    switch (cmd) {
    default:
        errno = ENOSYS;
        return -1;
    }
}

static int vfs_null_fsync(int fd)
{
    if (!FD_IN_RANGE(fd)) {
        errno = EBADF;
        return -1;
    }

    return 0;
}

#if defined(CONFIG_VFS_INITIALIZE_DEV_NULL) || defined(CONFIG_ESP_CONSOLE_NONE)
ESP_SYSTEM_INIT_FN(init_vfs_nullfs, CORE, BIT(0), 113)
{
    return esp_vfs_null_register();
}
#endif // CONFIG_VFS_INITIALIZE_DEV_NULL

void esp_vfs_include_nullfs_register(void)
{
    // Linker hook function, exists to make the linker examine this file
}
