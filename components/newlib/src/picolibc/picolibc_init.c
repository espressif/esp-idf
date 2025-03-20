/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio-bufio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/unistd.h>
#include <sys/reent.h>
#include <assert.h>
#include <fcntl.h>
#include "esp_newlib.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "esp_heap_caps.h"
#include "rom/libc_stubs.h"

#if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
static void esp_cleanup_r(struct _reent *rptr)
{
    return;
}
#endif

#if ESP_ROM_HAS_RETARGETABLE_LOCKING
static int __retarget_lock_try_acquire(struct __lock * p)
{
    __retarget_lock_acquire(p);
    return 0;
}

static int __retarget_lock_try_acquire_recursive(struct __lock *p)
{
    __retarget_lock_acquire_recursive(p);
    return 0;
}
#endif

static struct syscall_stub_table s_stub_table = {
    .__getreent = (void *)abort,
    ._malloc_r = (void *)abort,
    ._free_r = (void *)abort,
    ._realloc_r = (void *)abort,
    ._calloc_r = (void *)abort,
    ._abort = &abort,
    ._system_r = (void *)abort,
    ._rename_r = (void *)abort,
    ._times_r = (void *)abort,
    ._gettimeofday_r = (void *)abort,
    ._raise_r = (void *)abort,
    ._unlink_r = (void *)abort,
    ._link_r = (void *)abort,
    ._stat_r = (void *)abort,
    ._fstat_r = (void *)abort,
    ._sbrk_r = (void *)abort,
    ._getpid_r = (void *)abort,
    ._kill_r = (void *)abort,
    ._exit_r = NULL,    // never called in ROM
    ._close_r = (void *)abort,
    ._open_r = (void *)abort,
    ._write_r = (void *)abort,
    ._lseek_r = (void *)abort,
    ._read_r = (void *)abort,
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
    ._printf_float = NULL,
    ._scanf_float = NULL,
#if !(CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
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

/* TODO IDF-11226 */
void esp_newlib_init(void) __attribute__((alias("esp_libc_init")));
void esp_libc_init(void)
{
#if CONFIG_IDF_TARGET_ESP32
    syscall_table_ptr_pro = syscall_table_ptr_app = &s_stub_table;
#elif CONFIG_IDF_TARGET_ESP32S2
    syscall_table_ptr_pro = &s_stub_table;
#else
    syscall_table_ptr = &s_stub_table;
#endif

    environ = malloc(sizeof(char*));
    if (environ == 0) {
        // if allocation fails this early in startup process, there's nothing else other than to panic.
        abort();
    }
    environ[0] = NULL;

    esp_libc_locks_init();
}

void esp_setup_newlib_syscalls(void) __attribute__((alias("esp_libc_init")));

/* dummy function for backward compatibility */
void esp_reent_cleanup(void)
{
    return;
}

#if CONFIG_VFS_SUPPORT_IO
FILE *stdin;
FILE *stdout;
FILE *stderr;
void esp_libc_init_global_stdio(const char *stdio_dev)
{
    stdin = fopen(stdio_dev, "r");
    stdout = fopen(stdio_dev, "w");
    assert(stdin);
    assert(stdout);
    setlinebuf(stdout);
    stderr = stdout;
}
#else  /* CONFIG_VFS_SUPPORT_IO */
static char write_buf[BUFSIZ];
static char read_buf[BUFSIZ];

static struct __file_bufio __stdin = FDEV_SETUP_BUFIO(0, read_buf, BUFSIZ, read, write, lseek, close, __SRD, 0);
static struct __file_bufio __stdout = FDEV_SETUP_BUFIO(1, write_buf, BUFSIZ, read, write, lseek, close, __SWR, __BLBF);

FILE *stdin = &__stdin.xfile.cfile.file;
FILE *stdout = &__stdout.xfile.cfile.file;
FILE *stderr = &__stdout.xfile.cfile.file;
void esp_libc_init_global_stdio(void)
{
    __lock_init_recursive(stdin->lock);
    __lock_init_recursive(stdout->lock);
}
#endif  /* CONFIG_VFS_SUPPORT_IO */

// Hook to force the linker to include this file
void esp_libc_include_init_funcs(void)
{
}
