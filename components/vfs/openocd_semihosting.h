/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string.h>
#include <sys/errno.h>

#ifdef __XTENSA__
#include "xtensa/semihosting.h"
#elif __riscv
#include "riscv/semihosting.h"
#else
#error Unsupported architecture
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Semihosting call numbers and functions for OpenOCD.
 * In OpenOCD, ARM semihosting call numbers and parameters are used for
 * RISC-V and Xtensa targets.
 *
 * These conventions are not compatible with Xtensa ISS and QEMU for Xtensa,
 * which the actual Xtensa-specific semihosting call numbers and formats;
 * these are not supported in ESP-IDF yet.
 */

#define SEMIHOSTING_SYS_OPEN            0x01
#define SEMIHOSTING_SYS_CLOSE           0x02
#define SEMIHOSTING_SYS_WRITEC          0x03
#define SEMIHOSTING_SYS_WRITE0          0x04
#define SEMIHOSTING_SYS_WRITE           0x05
#define SEMIHOSTING_SYS_READ            0x06
#define SEMIHOSTING_SYS_READC           0x07
#define SEMIHOSTING_SYS_ISERROR         0x08
#define SEMIHOSTING_SYS_ISTTY           0x09
#define SEMIHOSTING_SYS_SEEK            0x0A
#define SEMIHOSTING_SYS_FLEN            0x0C
#define SEMIHOSTING_SYS_REMOVE          0x0E
#define SEMIHOSTING_SYS_RENAME          0x0F
#define SEMIHOSTING_SYS_CLOCK           0x10
#define SEMIHOSTING_SYS_TIME            0x11
#define SEMIHOSTING_SYS_SYSTEM          0x12
#define SEMIHOSTING_SYS_ERRNO           0x13
#define SEMIHOSTING_SYS_GET_CMDLINE     0x15
#define SEMIHOSTING_SYS_HEAPINFO        0x16
#define SEMIHOSTING_SYS_EXIT            0x18
#define SEMIHOSTING_SYS_EXIT_EXTENDED   0x20

/* This call is an Espressif OpenOCD extension to send the version
 * information to the host. This lets the host support different IDF versions,
 * allowing semihosting interface to be modified over time.
 *
 * The parameters of this call are:
 * - pointer to the version info structure,
 * - size of the version info structure.
 *
 * At present, the structure should contain a single word, indicating
 * the semihosting interface version used by the target.
 *
 * If the syscall is recognized, the return value is zero.
 */
#define ESP_SEMIHOSTING_SYS_DRV_INFO                0x100

/* 0x101...0x104 used by RiscV for custom semihosting calls */

/* Other Espressif extension sys calls */
#define ESP_SEMIHOSTING_SYS_SEEK                    0x105	/* custom lseek with whence */
/* not implemented yet */
#define ESP_SEMIHOSTING_SYS_MKDIR                   0x106
#define ESP_SEMIHOSTING_SYS_OPENDIR                 0x107
#define ESP_SEMIHOSTING_SYS_READDIR                 0x108
#define ESP_SEMIHOSTING_SYS_READDIR_R               0x109
#define ESP_SEMIHOSTING_SYS_SEEKDIR                 0x10A
#define ESP_SEMIHOSTING_SYS_TELLDIR                 0x10B
#define ESP_SEMIHOSTING_SYS_CLOSEDIR                0x10C
#define ESP_SEMIHOSTING_SYS_RMDIR                   0x10D
#define ESP_SEMIHOSTING_SYS_ACCESS                  0x10E
#define ESP_SEMIHOSTING_SYS_TRUNCATE                0x10F
#define ESP_SEMIHOSTING_SYS_UTIME                   0x110
#define ESP_SEMIHOSTING_SYS_FSTAT                   0x111
#define ESP_SEMIHOSTING_SYS_STAT                    0x112
#define ESP_SEMIHOSTING_SYS_FSYNC                   0x113
#define ESP_SEMIHOSTING_SYS_LINK                    0x114
#define ESP_SEMIHOSTING_SYS_UNLINK                  0x115

/*  Semihosting version bumped to 2. Changelog;
    1 - Memory based approach with 2 registers implemented as defined in the ARM standard.
    2 - User defined syscall numbers located between 0x100-0x1FF
    3 - The break instruction operands updated to (1, 14)
    4 - Absolute path support is dropped
*/
#define SEMIHOSTING_DRV_VERSION 2

/**
 * @brief Perform semihosting call and retrieve errno
 *
 * @param id    semihosting call number
 * @param data  data block to pass to the host; number of items and their
 *              meaning depends on the semihosting call. See the spec for
 *              details.
 * @param[out] out_errno  output, errno value from the host. Only set if
 *                        the return value is negative.
 * @return   return value from the host
 */
static inline long semihosting_call(long id, long *data, int *out_errno)
{
    long ret = semihosting_call_noerrno(id, data);
    if (ret < 0) {
        const int semihosting_sys_errno = SEMIHOSTING_SYS_ERRNO;
        *out_errno = (int)semihosting_call_noerrno(semihosting_sys_errno, NULL);
    }
    return ret;
}

static inline int semihosting_open(const char *path, int open_mode, int mode)
{
    int host_errno = 0;
    long args[] = {(long) path, open_mode, strlen(path), 0};
    (void) mode;  // unused in OpenOCD
    int result = (int)semihosting_call(SEMIHOSTING_SYS_OPEN, args, &host_errno);
    if (result < 0) {
        errno = host_errno;
    }
    return result;
}

static inline ssize_t semihosting_write(int fd, const void *data, size_t size)
{
    int host_errno = 0;
    long args[] = {fd, (long) data, size, 0};
    ssize_t ret = (ssize_t)semihosting_call(SEMIHOSTING_SYS_WRITE, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
        return ret;
    }
    /* On success, write syscall returns the number of bytes NOT written,
     * adjust the return value to match POSIX.
     */
    return size - (ssize_t)ret;
}

static inline ssize_t semihosting_read(int fd, void *data, size_t size)
{
    int host_errno = 0;
    long args[] = {fd, (long) data, size, 0};
    ssize_t ret = (ssize_t)semihosting_call(SEMIHOSTING_SYS_READ, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
        return ret;
    }
    /* On success, read syscall returns the number of bytes NOT read,
     * adjust the return value to match POSIX.
     */
    return size - (ssize_t)ret;
}

static inline int semihosting_close(int fd)
{
    int host_errno = 0;
    long args[] = {fd, 0, 0, 0};
    int ret = (int)semihosting_call(SEMIHOSTING_SYS_CLOSE, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline off_t semihosting_seek(int fd, off_t offset, int mode)
{
    int host_errno = 0;
    long args[] = {fd, offset, mode, 0};
    off_t ret = (off_t)semihosting_call(ESP_SEMIHOSTING_SYS_SEEK, args, &host_errno);
    if (ret == -1) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_ver_info(void)
{
    int host_errno = 0;
    struct {
        int version;
    } ver_info = { SEMIHOSTING_DRV_VERSION };
    long args[] = {(long) &ver_info, sizeof(ver_info), 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_DRV_INFO, args, &host_errno);
    (void) host_errno;  /* errno not set by this call */
    return ret;
}

static inline int semihosting_fstat(int fd, struct stat *restrict statbuf)
{
    int host_errno = 0;
    long args[] = {fd, (int)statbuf, 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_FSTAT, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_fsync(int fd)
{
    int host_errno = 0;
    long args[] = {fd, 0, 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_FSYNC, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

#ifdef CONFIG_VFS_SUPPORT_DIR
static inline int semihosting_mkdir(const char *host_path, mode_t mode)
{
    int host_errno = 0;
    long args[] = {(long)host_path, mode, strlen(host_path), 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_MKDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_rmdir(const char *host_path)
{
    int host_errno = 0;
    long args[] = {(long)host_path, strlen(host_path), 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_RMDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_access(const char *host_path, int mode)
{
    int host_errno = 0;
    long args[] = {(long)host_path, strlen(host_path), mode, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_ACCESS, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_truncate(const char *host_path, off_t truncate_length)
{
    int host_errno = 0;
    long args[] = {(long)host_path, strlen(host_path), truncate_length, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_TRUNCATE, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_utime(const char *host_path, const struct utimbuf *times)
{
    int host_errno = 0;
    long args[] = {(long)host_path, strlen(host_path), times->actime, times->modtime};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_UTIME, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_stat(const char *host_path, struct stat *restrict statbuf)
{
    int host_errno = 0;
    long args[] = {(long)host_path, strlen(host_path), (long)statbuf, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_STAT, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_rename(const char *old_path, const char *new_path)
{
    int host_errno = 0;
    long args[] = {(long)old_path, strlen(old_path), (long)new_path, strlen(new_path)};
    int ret = (int)semihosting_call(SEMIHOSTING_SYS_RENAME, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_link(const char *path1, const char *path2)
{
    int host_errno = 0;
    long args[] = {(long)path1, strlen(path1), (long)path2, strlen(path2)};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_LINK, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_unlink(const char *path)
{
    int host_errno = 0;
    long args[] = {(long)path, strlen(path), 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_UNLINK, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_opendir(const char *path, long offset)
{
    int host_errno = 0;
    long args[] = {(long)path, strlen(path), offset, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_OPENDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_readdir(long struct_dirent_ptr, long offset)
{
    int host_errno = 0;
    long args[] = {struct_dirent_ptr, offset, 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_READDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline int semihosting_closedir(long id)
{
    int host_errno = 0;
    long args[] = {id, 0, 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_CLOSEDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline long semihosting_telldir(long id)
{
    int host_errno = 0;
    long args[] = {id, 0, 0, 0};
    long ret = semihosting_call(ESP_SEMIHOSTING_SYS_TELLDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}
static inline int semihosting_seekdir(long id, long offset)
{
    int host_errno = 0;
    long args[] = {id, offset, 0, 0};
    int ret = (int)semihosting_call(ESP_SEMIHOSTING_SYS_SEEKDIR, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

#endif
#ifdef __cplusplus
}
#endif
