/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_rom_libc_stubs.h"
#include "esp_private/startup_internal.h"

extern int _printf_float(struct _reent *rptr,
                         void *pdata,
                         FILE * fp,
                         int (*pfunc)(struct _reent *, FILE *, const char *, size_t len),
                         va_list * ap);

extern int _scanf_float(struct _reent *rptr,
                        void *pdata,
                        FILE *fp,
                        va_list *ap);

static void raise_r_stub(struct _reent *rptr)
{
    _raise_r(rptr, 0);
}

static void esp_cleanup_r(struct _reent *rptr)
{
    if (_REENT_STDIN(rptr) != _REENT_STDIN(_GLOBAL_REENT)) {
        _fclose_r(rptr, _REENT_STDIN(rptr));
    }

    if (_REENT_STDOUT(rptr) != _REENT_STDOUT(_GLOBAL_REENT)) {
        _fclose_r(rptr, _REENT_STDOUT(rptr));
    }

    if (_REENT_STDERR(rptr) != _REENT_STDERR(_GLOBAL_REENT)) {
        _fclose_r(rptr, _REENT_STDERR(rptr));
    }
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
    ._write_r = (int (*)(struct _reent * r, int, const void *, int)) &_write_r,
    ._lseek_r = (int (*)(struct _reent * r, int, int, int)) &_lseek_r,
    ._read_r = (int (*)(struct _reent * r, int, void *, int)) &_read_r,
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
#if !CONFIG_IDF_TARGET_ESP32 && !CONFIG_IDF_TARGET_ESP32S2
    /* TODO IDF-2570 : mark that this assert failed in ROM, to avoid confusion between IDF & ROM
       assertion failures (as function names & source file names will be similar)
    */
    .__assert_func = __assert_func,

    /* We don't expect either ROM code to ever call __sinit, so it's implemented as abort() for now.

       __sinit may be called in IDF side only if /dev/console used as input/output. It called only
       once for _GLOBAL_REENT. Then reuse std file pointers from _GLOBAL_REENT in another reents.
       See esp_newlib_init() and esp_reent_init() for details.
    */
    .__sinit = (void *)abort,
    ._cleanup_r = &esp_cleanup_r,
#endif
};

void esp_newlib_init(void)
{
#if CONFIG_IDF_TARGET_ESP32
    syscall_table_ptr_pro = syscall_table_ptr_app = &s_stub_table;
#elif CONFIG_IDF_TARGET_ESP32S2
    syscall_table_ptr_pro = &s_stub_table;
#else
    syscall_table_ptr = &s_stub_table;
#endif

#if __NEWLIB__ > 4 || ( __NEWLIB__ == 4 && __NEWLIB_MINOR__ > 1 ) /* TODO: IDF-8134 */
    memset(&__sglue, 0, sizeof(__sglue));
    _global_impure_ptr = _GLOBAL_REENT;
#else
    static struct _reent s_reent;
    _GLOBAL_REENT = &s_reent;
#endif

    /* Ensure that the initialization of sfp is prevented until esp_newlib_init_global_stdio() is explicitly invoked. */
    _GLOBAL_REENT->__cleanup = esp_cleanup_r;
    _REENT_SDIDINIT(_GLOBAL_REENT) = 1;

    environ = malloc(sizeof(char*));
    if (environ == 0) {
        // if allocation fails this early in startup process, there's nothing else other than to panic.
        abort();
    }
    environ[0] = NULL;

    esp_newlib_locks_init();
}

ESP_SYSTEM_INIT_FN(init_newlib, CORE, BIT(0), 102)
{
    esp_newlib_init();
    return ESP_OK;
}

void esp_setup_newlib_syscalls(void) __attribute__((alias("esp_newlib_init")));

/**
 * Postponed _GLOBAL_REENT stdio FPs initialization.
 *
 * Can not be a part of esp_reent_init() because stdio device may not initialized yet.
 *
 * Called from startup code and FreeRTOS, not intended to be called from
 * application code.
 */
void esp_newlib_init_global_stdio(const char *stdio_dev)
{
    if (stdio_dev == NULL) {
        _GLOBAL_REENT->__cleanup = NULL;
        _REENT_SDIDINIT(_GLOBAL_REENT) = 0;
        __sinit(_GLOBAL_REENT);
        _GLOBAL_REENT->__cleanup = esp_cleanup_r;
        _REENT_SDIDINIT(_GLOBAL_REENT) = 1;
    } else {
        _REENT_STDIN(_GLOBAL_REENT) = fopen(stdio_dev, "r");
        _REENT_STDOUT(_GLOBAL_REENT) = fopen(stdio_dev, "w");
        _REENT_STDERR(_GLOBAL_REENT) = fopen(stdio_dev, "w");
#if ESP_ROM_NEEDS_SWSETUP_WORKAROUND
        /*
        - This workaround for printf functions using 32-bit time_t after the 64-bit time_t upgrade
        - The 32-bit time_t usage is triggered through ROM Newlib functions printf related functions calling __swsetup_r() on
          the first call to a particular file pointer (i.e., stdin, stdout, stderr)
        - Thus, we call the toolchain version of __swsetup_r() now (before any printf calls are made) to setup all of the
          file pointers. Thus, the ROM newlib code will never call the ROM version of __swsetup_r().
        - See IDFGH-7728 for more details
        */
        extern int __swsetup_r(struct _reent *, FILE *);
        __swsetup_r(_GLOBAL_REENT, _REENT_STDIN(_GLOBAL_REENT));
        __swsetup_r(_GLOBAL_REENT, _REENT_STDOUT(_GLOBAL_REENT));
        __swsetup_r(_GLOBAL_REENT, _REENT_STDERR(_GLOBAL_REENT));
#endif /* ESP_ROM_NEEDS_SWSETUP_WORKAROUND */
    }
}

ESP_SYSTEM_INIT_FN(init_newlib_stdio, CORE, BIT(0), 115)
{
#if defined(CONFIG_VFS_SUPPORT_IO) && !defined(CONFIG_ESP_CONSOLE_NONE)
    esp_newlib_init_global_stdio("/dev/console");
#else
    esp_newlib_init_global_stdio(NULL);
#endif
    return ESP_OK;
}

// Hook to force the linker to include this file
void newlib_include_init_funcs(void)
{
}
