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

#define SEMIHOSTING_SYS_CLOCK 0x10
#define SEMIHOSTING_SYS_CLOSE 0x02
#define SEMIHOSTING_SYS_ERRNO 0x13
#define SEMIHOSTING_SYS_EXIT 0x18
#define SEMIHOSTING_SYS_EXIT_EXTENDED 0x20
#define SEMIHOSTING_SYS_FLEN 0x0C
#define SEMIHOSTING_SYS_GET_CMDLINE 0x15
#define SEMIHOSTING_SYS_HEAPINFO 0x16
#define SEMIHOSTING_SYS_ISERROR 0x08
#define SEMIHOSTING_SYS_ISTTY 0x09
#define SEMIHOSTING_SYS_OPEN 0x01
#define SEMIHOSTING_SYS_READ 0x06
#define SEMIHOSTING_SYS_READC 0x07
#define SEMIHOSTING_SYS_REMOVE 0x0E
#define SEMIHOSTING_SYS_RENAME 0x0F
#define SEMIHOSTING_SYS_SEEK 0x0A
#define SEMIHOSTING_SYS_SYSTEM 0x12
#define SEMIHOSTING_SYS_TIME 0x11
#define SEMIHOSTING_SYS_WRITE 0x05
#define SEMIHOSTING_SYS_WRITEC 0x03
#define SEMIHOSTING_SYS_WRITE0 0x04

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
#define SEMIHOSTING_SYS_DRVINFO 0xE0


static inline int semihosting_open(const char *path, int open_mode, int mode)
{
    int host_errno = 0;
    long args[] = {(long) path, open_mode, strlen(path), 0};
    (void) mode;  // unused in OpenOCD
    int result = (int) semihosting_call(SEMIHOSTING_SYS_OPEN, args, &host_errno);
    if (result < 0) {
        errno = host_errno;
    }
    return result;
}

static inline ssize_t semihosting_write(int fd, const void *data, size_t size)
{
    int host_errno = 0;
    long args[] = {fd, (long) data, size, 0};
    ssize_t ret = (ssize_t) semihosting_call(SEMIHOSTING_SYS_WRITE, args, &host_errno);
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
    ssize_t ret = (ssize_t) semihosting_call(SEMIHOSTING_SYS_READ, args, &host_errno);
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
    int ret = (int) semihosting_call(SEMIHOSTING_SYS_CLOSE, args, &host_errno);
    if (ret < 0) {
        errno = host_errno;
    }
    return ret;
}

static inline off_t semihosting_seek(int fd, off_t offset, int mode)
{
    int host_errno = 0;
    long args[] = {fd, offset, mode, 0};
    off_t ret = (off_t) semihosting_call(SEMIHOSTING_SYS_SEEK, args, &host_errno);
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
    } ver_info = { 1 };
    long args[] = {(long) &ver_info, sizeof(ver_info), 0, 0};
    int ret = (int) semihosting_call(SEMIHOSTING_SYS_DRVINFO, args, &host_errno);
    (void) host_errno;  /* errno not set by this call */
    return ret;
}

#ifdef __cplusplus
}
#endif
