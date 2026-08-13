/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"

#if CONFIG_LIBC_NEWLIB

#include_next <reent.h>

#else // CONFIG_LIBC_NEWLIB

/* picolibc has no <reent.h>, but the newlib copy shipped in the same toolchain
 * sysroot is still reachable through the include search path and does not
 * compile against picolibc headers. Declare the reentrant syscalls IDF relies
 * on here instead, using the POSIX types the IDF implementations are defined
 * with.
 */

#include <stddef.h>
#include <sys/reent.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stat;
struct timeval;
struct tms;

extern int _close_r(struct _reent *, int);
extern int _fcntl_r(struct _reent *, int, int, int);
extern int _fstat_r(struct _reent *, int, struct stat *);
extern int _getentropy_r(struct _reent *, void *, size_t);
extern int _getpid_r(struct _reent *);
extern int _gettimeofday_r(struct _reent *, struct timeval *, void *);
extern int _isatty_r(struct _reent *, int);
extern int _kill_r(struct _reent *, int, int);
extern int _link_r(struct _reent *, const char *, const char *);
extern off_t _lseek_r(struct _reent *, int, off_t, int);
extern int _mkdir_r(struct _reent *, const char *, int);
extern int _open_r(struct _reent *, const char *, int, int);
extern ssize_t _read_r(struct _reent *, int, void *, size_t);
extern int _rename_r(struct _reent *, const char *, const char *);
extern void *_sbrk_r(struct _reent *, ptrdiff_t);
extern int _stat_r(struct _reent *, const char *, struct stat *);
extern clock_t _times_r(struct _reent *, struct tms *);
extern int _unlink_r(struct _reent *, const char *);
extern ssize_t _write_r(struct _reent *, int, const void *, size_t);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_LIBC_NEWLIB
