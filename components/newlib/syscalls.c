/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
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
#include "sdkconfig.h"
#include "esp_rom_uart.h"

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
    const char* cdata = (const char*) data;
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
        for (size_t i = 0; i < size; ++i) {
            esp_rom_uart_tx_one_char(cdata[i]);
        }
        return size;
    }
    __errno_r(r) = EBADF;
    return -1;
}

ssize_t _read_r_console(struct _reent *r, int fd, void * data, size_t size)
{
    char* cdata = (char*) data;
    if (fd == STDIN_FILENO) {
        size_t received;
        for (received = 0; received < size; ++received) {
            int status = esp_rom_uart_rx_one_char((uint8_t*) &cdata[received]);
            if (status != 0) {
                break;
            }
        }
        if (received == 0) {
            errno = EWOULDBLOCK;
            return -1;
        }
        return received;
    }
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
#ifdef CONFIG_ESP_CONSOLE_UART
        esp_rom_uart_flush_tx(CONFIG_ESP_CONSOLE_UART_NUM);
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
        esp_rom_uart_flush_tx(CONFIG_ESP_ROM_USB_SERIAL_DEVICE_NUM);
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
        esp_rom_uart_flush_tx(CONFIG_ESP_ROM_USB_OTG_NUM);
#endif
        return 0;
    }
    errno = EBADF;
    return -1;
}


/* The following weak definitions of syscalls will be used unless
 * another definition is provided. That definition may come from
 * VFS, LWIP, or the application.
 */
ssize_t _read_r(struct _reent *r, int fd, void * dst, size_t size)
    __attribute__((weak,alias("_read_r_console")));
ssize_t _write_r(struct _reent *r, int fd, const void * data, size_t size)
    __attribute__((weak,alias("_write_r_console")));
int _fstat_r (struct _reent *r, int fd, struct stat *st)
    __attribute__((weak,alias("_fstat_r_console")));
int fsync(int fd)
    __attribute__((weak,alias("_fsync_console")));


/* The aliases below are to "syscall_not_implemented", which
 * doesn't have the same signature as the original function.
 * Disable type mismatch warnings for this reason.
 */
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattribute-alias"
#endif

int _open_r(struct _reent *r, const char * path, int flags, int mode)
    __attribute__((weak,alias("syscall_not_implemented")));
int _close_r(struct _reent *r, int fd)
    __attribute__((weak,alias("syscall_not_implemented")));
off_t _lseek_r(struct _reent *r, int fd, off_t size, int mode)
    __attribute__((weak,alias("syscall_not_implemented")));
int _fcntl_r(struct _reent *r, int fd, int cmd, int arg)
    __attribute__((weak,alias("syscall_not_implemented")));
int _stat_r(struct _reent *r, const char * path, struct stat * st)
    __attribute__((weak,alias("syscall_not_implemented")));
int _link_r(struct _reent *r, const char* n1, const char* n2)
    __attribute__((weak,alias("syscall_not_implemented")));
int _unlink_r(struct _reent *r, const char *path)
    __attribute__((weak,alias("syscall_not_implemented")));
int _rename_r(struct _reent *r, const char *src, const char *dst)
    __attribute__((weak,alias("syscall_not_implemented")));
int _isatty_r(struct _reent *r, int fd)
    __attribute__((weak,alias("syscall_not_implemented")));


/* These functions are not expected to be overridden */
int _system_r(struct _reent *r, const char *str)
    __attribute__((alias("syscall_not_implemented")));
int raise(int sig)
    __attribute__((alias("syscall_not_implemented_aborts")));
int _raise_r(struct _reent *r, int sig)
    __attribute__((alias("syscall_not_implemented_aborts")));
void* _sbrk_r(struct _reent *r, ptrdiff_t sz)
    __attribute__((alias("syscall_not_implemented_aborts")));
int _getpid_r(struct _reent *r)
    __attribute__((alias("syscall_not_implemented")));
int _kill_r(struct _reent *r, int pid, int sig)
    __attribute__((alias("syscall_not_implemented")));
void _exit(int __status)
    __attribute__((alias("syscall_not_implemented_aborts")));

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

/* Similar to syscall_not_implemented, but not taking struct _reent argument */
int system(const char* str)
{
    errno = ENOSYS;
    return -1;
}

/* Replaces newlib fcntl, which has been compiled without HAVE_FCNTL */
int fcntl(int fd, int cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int arg = va_arg(args, int);
    va_end(args);
    struct _reent* r = __getreent();
    return _fcntl_r(r, fd, cmd, arg);
}

/* No-op function, used to force linking this file,
   instead of the syscalls implementation from libgloss.
 */
void newlib_include_syscalls_impl(void)
{
}
