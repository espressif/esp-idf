/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Linux target VFS syscall wrappers
 *
 * This file provides POSIX syscall wrappers that redirect to ESP-IDF VFS functions
 * when building for Linux target.
 */

#include <stdlib.h>
#include <string.h>
#include "esp_vfs.h"

int open(const char * path, int flags, ...)
{
    struct _reent r;
    va_list list;
    va_start(list, flags);
    int mode = va_arg(list, int);
    va_end(list);
    return esp_vfs_open(&r, path, flags, mode);
}

int close(int fd)
{
    struct _reent r;
    return esp_vfs_close(&r, fd);
}

ssize_t read(int fd, void * dst, size_t size)
{
    struct _reent r;
    return esp_vfs_read(&r, fd, dst, size);
}

ssize_t write(int fd, const void * data, size_t size)
{
    struct _reent r;
    return esp_vfs_write(&r, fd, data, size);
}

off_t lseek(int fd, off_t size, int mode)
{
    struct _reent r;
    return esp_vfs_lseek(&r, fd, size, mode);
}

ssize_t pread(int fd, void *dst, size_t size, off_t offset)
{
    return esp_vfs_pread(fd,dst, size, offset);
}

ssize_t pwrite(int fd, const void *src, size_t size, off_t offset)
{
    return esp_vfs_pwrite(fd, src,size, offset);
}

int fstat(int fd, struct stat *st)
{
    struct _reent r;
    return esp_vfs_fstat(&r, fd, st);
}

int fcntl(int fd, int cmd, ...)
{
    struct _reent r;
    va_list list;
    va_start(list, cmd);
    int args = va_arg(list, int);
    va_end(list);
    return esp_vfs_fcntl_r(&r, fd, cmd, args);
}

int ioctl(int fd, unsigned long cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int ret = esp_vfs_ioctl(fd, cmd, args);
    va_end(args);
    return ret;
}

int fsync(int fd)
{
    return esp_vfs_fsync(fd);
}

#ifdef CONFIG_VFS_SUPPORT_SELECT
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    return esp_vfs_select(nfds, readfds, writefds, errorfds, timeout);
}
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_DIR
int stat(const char * path, struct stat * st)
{
    struct _reent r;
    return esp_vfs_stat(&r, path, st);
}

int link(const char* n1, const char* n2)
{
    struct _reent r;
    return esp_vfs_link(&r, n1, n2);
}

int unlink(const char *path)
{
    struct _reent r;
    return esp_vfs_unlink(&r, path);
}

int rename(const char *src, const char *dst)
{
    struct _reent r;
    return esp_vfs_rename(&r, src, dst);
}

int truncate(const char *path, off_t length)
{
    return esp_vfs_truncate(path, length);
}

int ftruncate(int fd, off_t length)
{
    return esp_vfs_ftruncate(fd, length);
}

int access(const char *path, int amode)
{
    return esp_vfs_access(path, amode);
}

int utime(const char *path, const struct utimbuf *times)
{
    return esp_vfs_utime(path, times);
}

int rmdir(const char* name)
{
    return esp_vfs_rmdir(name);
}

int mkdir(const char* name, mode_t mode)
{
    return esp_vfs_mkdir(name, mode);
}

DIR* opendir(const char* name)
{
    return esp_vfs_opendir(name);
}

int closedir(DIR* pdir)
{
    return esp_vfs_closedir(pdir);
}

int readdir_r(DIR* pdir, struct dirent* entry, struct dirent** out_dirent)
{
    return esp_vfs_readdir_r(pdir, entry, out_dirent);
}

struct dirent* readdir(DIR* pdir)
{
    return esp_vfs_readdir(pdir);
}

long telldir(DIR* pdir)
{
    return esp_vfs_telldir(pdir);
}

void seekdir(DIR* pdir, long loc)
{
    return esp_vfs_seekdir(pdir, loc);
}

void rewinddir(DIR* pdir)
{
    return esp_vfs_rewinddir(pdir);
}

#endif // CONFIG_VFS_SUPPORT_DIR
