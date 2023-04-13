/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/reent.h>
#include <assert.h>
#include "esp_newlib.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/libc_stubs.h"
#endif

static struct _reent s_reent;

extern int _printf_float(struct _reent *rptr,
               void *pdata,
               FILE * fp,
               int (*pfunc) (struct _reent *, FILE *, const char *, size_t len),
               va_list * ap);


extern int _scanf_float(struct _reent *rptr,
              void *pdata,
              FILE *fp,
              va_list *ap);

static void raise_r_stub(struct _reent *rptr)
{
    _raise_r(rptr, 0);
}

static struct syscall_stub_table s_stub_table = {
    .__getreent = &__getreent,
    ._malloc_r = &_malloc_r,
    ._free_r = &_free_r,
    ._realloc_r = &_realloc_r,
    ._calloc_r = &_calloc_r,
    ._abort = &abort,
    ._system_r = &_system_r,
    ._rename_r = &_rename_r,
    ._times_r = &_times_r,
    ._gettimeofday_r = &_gettimeofday_r,
    ._raise_r = &raise_r_stub,
    ._unlink_r = &_unlink_r,
    ._link_r = &_link_r,
    ._stat_r = &_stat_r,
    ._fstat_r = &_fstat_r,
    ._sbrk_r = &_sbrk_r,
    ._getpid_r = &_getpid_r,
    ._kill_r = &_kill_r,
    ._exit_r = NULL,    // never called in ROM
    ._close_r = &_close_r,
    ._open_r = &_open_r,
    ._write_r = (int (*)(struct _reent *r, int, const void *, int)) &_write_r,
    ._lseek_r = (int (*)(struct _reent *r, int, int, int)) &_lseek_r,
    ._read_r = (int (*)(struct _reent *r, int, void *, int)) &_read_r,
#if ESP_ROM_HAS_RETARGETABLE_LOCKING
    ._retarget_lock_init = &__retarget_lock_init,
    ._retarget_lock_init_recursive = &__retarget_lock_init_recursive,
    ._retarget_lock_close = &__retarget_lock_close,
    ._retarget_lock_close_recursive = &__retarget_lock_close_recursive,
    ._retarget_lock_acquire = &__retarget_lock_acquire,
    ._retarget_lock_acquire_recursive = &__retarget_lock_acquire_recursive,
    ._retarget_lock_try_acquire = &__retarget_lock_try_acquire,
    ._retarget_lock_try_acquire_recursive = &__retarget_lock_try_acquire_recursive,
    ._retarget_lock_release = &__retarget_lock_release,
    ._retarget_lock_release_recursive = &__retarget_lock_release_recursive,
#else
    ._lock_init = &_lock_init,
    ._lock_init_recursive = &_lock_init_recursive,
    ._lock_close = &_lock_close,
    ._lock_close_recursive = &_lock_close_recursive,
    ._lock_acquire = &_lock_acquire,
    ._lock_acquire_recursive = &_lock_acquire_recursive,
    ._lock_try_acquire = &_lock_try_acquire,
    ._lock_try_acquire_recursive = &_lock_try_acquire_recursive,
    ._lock_release = &_lock_release,
    ._lock_release_recursive = &_lock_release_recursive,
#endif
#ifdef CONFIG_NEWLIB_NANO_FORMAT
    ._printf_float = &_printf_float,
    ._scanf_float = &_scanf_float,
#else
    ._printf_float = NULL,
    ._scanf_float = NULL,
#endif
#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 \
    || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
    /* TODO IDF-2570 : mark that this assert failed in ROM, to avoid confusion between IDF & ROM
       assertion failures (as function names & source file names will be similar)
    */
    .__assert_func = __assert_func,

    /* We don't expect either ROM code or IDF to ever call __sinit, so it's implemented as abort() for now.

       esp_reent_init() does this job inside IDF.

       Kept in the syscall table in case we find a need for it later.
    */
    .__sinit = (void *)abort,
    ._cleanup_r = &_cleanup_r,
#endif
};

void esp_newlib_init(void)
{
#if CONFIG_IDF_TARGET_ESP32
    syscall_table_ptr_pro = syscall_table_ptr_app = &s_stub_table;
#elif CONFIG_IDF_TARGET_ESP32S2
    syscall_table_ptr_pro = &s_stub_table;
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 \
    || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32H2
    syscall_table_ptr = &s_stub_table;
#endif

    _GLOBAL_REENT = &s_reent;

    environ = malloc(sizeof(char*));
    if (environ == 0) {
        // if allocation fails this early in startup process, there's nothing else other than to panic.
        abort();
    }
    environ[0] = NULL;

    esp_newlib_locks_init();
}

void esp_setup_newlib_syscalls(void) __attribute__((alias("esp_newlib_init")));
