/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <reent.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "esp_system_console.h"

static int syscall_not_implemented(struct _reent *r, ...)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

static int syscall_not_implemented_aborts(void)
{
    abort();
}

ssize_t _write_r_console(struct _reent *r, int fd, const void * data, size_t size)
{
#if !CONFIG_ESP_CONSOLE_NONE
    const char* cdata = (const char*) data;
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        for (size_t i = 0; i < size; ++i) {
            if (cdata[i] == '\n') {
                esp_system_console_put_char('\r');
            }
            esp_system_console_put_char(cdata[i]);
        }
        return size;
    }
#endif //!CONFIG_ESP_CONSOLE_NONE
    __errno_r(r) = EBADF;
    return -1;
}

ssize_t _read_r_console(struct _reent *r, int fd, void * data, size_t size)
{
#if !CONFIG_ESP_CONSOLE_NONE
    char* cdata = (char*) data;
    if (fd == STDIN_FILENO) {
        size_t received;
        for (received = 0; received < size; ++received) {
            int status = esp_rom_output_rx_one_char((uint8_t*) &cdata[received]);
            if (status != 0) {
                break;
            }
            if (cdata[received] == '\r') {
                cdata[received] = '\n';
            }
        }
        if (received == 0) {
            __errno_r(r) = EWOULDBLOCK;
            return -1;
        }
        return received;
    }
#endif //!CONFIG_ESP_CONSOLE_NONE
    __errno_r(r) = EBADF;
    return -1;
}

static ssize_t _fstat_r_console(struct _reent *r, int fd, struct stat * st)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        memset(st, 0, sizeof(*st));
        /* This needs to be set so that stdout and stderr are line buffered. */
        st->st_mode = S_IFCHR;
        return 0;
    }
    __errno_r(r) = EBADF;
    return -1;
}

static int _fsync_console(int fd)
{
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        esp_rom_output_flush_tx(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
        return 0;
    }
    errno = EBADF;
    return -1;
}

/* The following weak definitions of syscalls will be used unless
 * another definition is provided. That definition may come from
 * VFS, LWIP, or the application.
 */
ssize_t _read_r(struct _reent *r, int fd, void *dst, size_t size)
__attribute__((weak, alias("_read_r_console")));
ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t size)
__attribute__((weak, alias("_write_r_console")));
int _fstat_r(struct _reent *r, int fd, struct stat *st)
__attribute__((weak, alias("_fstat_r_console")));
int fsync(int fd)
__attribute__((weak, alias("_fsync_console")));

/* The aliases below are to "syscall_not_implemented", which
 * doesn't have the same signature as the original function.
 * Disable type mismatch warnings for this reason.
 */
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattribute-alias"
#endif

int _open_r(struct _reent *r, const char * path, int flags, int mode)
__attribute__((weak, alias("syscall_not_implemented")));
int _close_r(struct _reent *r, int fd)
__attribute__((weak, alias("syscall_not_implemented")));
off_t _lseek_r(struct _reent *r, int fd, off_t size, int mode)
__attribute__((weak, alias("syscall_not_implemented")));
int _fcntl_r(struct _reent *r, int fd, int cmd, int arg)
__attribute__((weak, alias("syscall_not_implemented")));
int _stat_r(struct _reent *r, const char * path, struct stat * st)
__attribute__((weak, alias("syscall_not_implemented")));
int _link_r(struct _reent *r, const char* n1, const char* n2)
__attribute__((weak, alias("syscall_not_implemented")));
int _unlink_r(struct _reent *r, const char *path)
__attribute__((weak, alias("syscall_not_implemented")));
int _rename_r(struct _reent *r, const char *src, const char *dst)
__attribute__((weak, alias("syscall_not_implemented")));
int _isatty_r(struct _reent *r, int fd)
__attribute__((weak, alias("syscall_not_implemented")));

/* These functions are not expected to be overridden */
int _system_r(struct _reent *r, const char *str)
__attribute__((alias("syscall_not_implemented")));
int _raise_r(struct _reent *r, int sig)
__attribute__((alias("syscall_not_implemented_aborts")));
void* _sbrk_r(struct _reent *r, ptrdiff_t sz)
__attribute__((alias("syscall_not_implemented_aborts")));
int _getpid_r(struct _reent *r)
__attribute__((alias("syscall_not_implemented")));
int _kill_r(struct _reent *r, int pid, int sig)
__attribute__((alias("syscall_not_implemented")));

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

/* No-op function, used to force linking this file,
   instead of the syscalls implementation from libgloss.
 */
void esp_libc_include_reent_syscalls_impl(void)
{
}
