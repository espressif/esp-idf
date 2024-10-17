/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ROM_LIBC_STUBS_H_
#define _ROM_LIBC_STUBS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <reent.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
ESP32 ROM code contains implementations of some of C library functions.
Whenever a function in ROM needs to use a syscall, it calls a pointer to the corresponding syscall
implementation defined in the following struct.

The table itself, by default, is not allocated in RAM. There are two pointers, `syscall_table_ptr_pro` and
`syscall_table_ptr_app`, which can be set to point to the locations of syscall tables of CPU 0 (aka PRO CPU)
and CPU 1 (aka APP CPU). Location of these pointers in .bss segment of ROM code is defined in linker script.

So, before using any of the C library functions (except for pure functions and memcpy/memset functions),
application must allocate syscall table structure for each CPU being used, and populate it with pointers
to actual implementations of corresponding syscalls.
*/

struct syscall_stub_table
{
    struct _reent* (*__getreent)(void);
    void* (*_malloc_r)(struct _reent *r, size_t);
    void (*_free_r)(struct _reent *r, void*);
    void* (*_realloc_r)(struct _reent *r, void*, size_t);
    void* (*_calloc_r)(struct _reent *r, size_t, size_t);
    void (*_abort)(void);
    int (*_system_r)(struct _reent *r, const char*);
    int (*_rename_r)(struct _reent *r, const char*, const char*);
    clock_t (*_times_r)(struct _reent *r, struct tms *);
    int (*_gettimeofday_r) (struct _reent *r, struct timeval *, void *);
    void (*_raise_r)(struct _reent *r); /* function signature is incorrect in ROM */
    int (*_unlink_r)(struct _reent *r, const char*);
    int (*_link_r)(struct _reent *r, const char*, const char*);
    int (*_stat_r)(struct _reent *r, const char*, struct stat *);
    int (*_fstat_r)(struct _reent *r, int, struct stat *);
    void* (*_sbrk_r)(struct _reent *r, ptrdiff_t);
    int (*_getpid_r)(struct _reent *r);
    int (*_kill_r)(struct _reent *r, int, int);
    void (*_exit_r)(struct _reent *r, int);
    int (*_close_r)(struct _reent *r, int);
    int (*_open_r)(struct _reent *r, const char *, int, int);
    int (*_write_r)(struct _reent *r, int, const void *, int);
    int (*_lseek_r)(struct _reent *r, int, int, int);
    int (*_read_r)(struct _reent *r, int, void *, int);
    void (*_lock_init)(_lock_t *lock);
    void (*_lock_init_recursive)(_lock_t *lock);
    void (*_lock_close)(_lock_t *lock);
    void (*_lock_close_recursive)(_lock_t *lock);
    void (*_lock_acquire)(_lock_t *lock);
    void (*_lock_acquire_recursive)(_lock_t *lock);
    int (*_lock_try_acquire)(_lock_t *lock);
    int (*_lock_try_acquire_recursive)(_lock_t *lock);
    void (*_lock_release)(_lock_t *lock);
    void (*_lock_release_recursive)(_lock_t *lock);
    int (*_printf_float)(struct _reent *data, void *pdata, FILE * fp, int (*pfunc) (struct _reent *, FILE *, const char *, size_t len), va_list * ap);
    int (*_scanf_float) (struct _reent *rptr, void *pdata, FILE *fp, va_list *ap);
};

extern struct syscall_stub_table* syscall_table_ptr_pro;
extern struct syscall_stub_table* syscall_table_ptr_app;

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _ROM_LIBC_STUBS_H_ */
