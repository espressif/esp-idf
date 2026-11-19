/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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
#include <esp_compiler.h>

static int syscall_not_implemented(struct _reent *r, ...)
{
    __errno_r(r) = ENOSYS;
    return -1;
}

static int syscall_not_implemented_aborts(void)
{
    abort();
}

/* The aliases below are to "syscall_not_implemented", which
 * doesn't have the same signature as the original function.
 * Disable type mismatch warnings for this reason.
 */
ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wattribute-alias")

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

ESP_COMPILER_DIAGNOSTIC_POP("-Wattribute-alias")

/* No-op function, used to force linking this file,
   instead of the syscalls implementation from libgloss.
 */
void esp_libc_include_reent_syscalls_impl(void)
{
}
