/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <reent.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

#if CONFIG_IDF_TOOLCHAIN_GCC
#include <sys/statvfs.h>
#endif

#if CONFIG_LIBC_PICOLIBC
int open(const char *pathname, int flags, ...)
{
    /* The mode argument must be supplied if O_CREAT or O_TMPFILE is specified
     * in flags.
     * But O_TMPFILE is a linux flag. Ignore it
     */
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        va_end(args);

        return _open_r(__getreent(), pathname, flags, mode);
    }

    return _open_r(__getreent(), pathname, flags, 0);
}

int close(int fd)
{
    return _close_r(__getreent(), fd);
}

ssize_t read(int fd, void *dst, size_t size)
{
    return _read_r(__getreent(), fd, dst, size);
}

ssize_t write(int fd, const void *dst, size_t size)
{
    return _write_r(__getreent(), fd, dst, size);
}

off_t lseek(int fd, off_t size, int mode)
{
    return _lseek_r(__getreent(), fd, size, mode);
}

int creat(const char *path, mode_t mode)
{
    return _open_r(__getreent(), path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int gettimeofday(struct timeval *restrict tv, void *restrict tz)
{
    return _gettimeofday_r(__getreent(), tv, tz);
}

int unlink(const char *path)
{
    return _unlink_r(__getreent(), path);
}

int stat(const char * path, struct stat * st)
{
    return _stat_r(__getreent(), path, st);
}

int link(const char* n1, const char* n2)
{
    return _link_r(__getreent(), n1, n2);
}

int rename(const char *src, const char *dst)
{
    return _rename_r(__getreent(), src, dst);
}

int isatty(int fd)
{
    struct stat buf;

    if (_fstat_r(__getreent(), fd, &buf) < 0) {
        return 0;
    }

    if (S_ISCHR(buf.st_mode)) {
        return 1;
    }

    return 0;
}

int fcntl(int fd, int cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int arg = va_arg(args, int);
    va_end(args);
    return _fcntl_r(__getreent(), fd, cmd, arg);
}

#endif // CONFIG_LIBC_PICOLIBC

void _exit(int __status)
{
    abort();
}

int fstat(int fd, struct stat *st)
{
    return _fstat_r(__getreent(), fd, st);
}

int raise(int sig)
{
    return _raise_r(__getreent(), sig);
}

int system(const char* str)
{
    return _system_r(__getreent(), str);
}

#if CONFIG_IDF_TOOLCHAIN_GCC
int statvfs(const char *restrict path, struct statvfs *restrict buf)
{
    /* TODO IDF-9879 */
    errno = ENOSYS;
    return -1;
}

int fstatvfs(int fd, struct statvfs *buf)
{
    /* TODO IDF-9879 */
    errno = ENOSYS;
    return -1;
}
#endif

void esp_libc_include_syscalls_impl(void)
{
}
