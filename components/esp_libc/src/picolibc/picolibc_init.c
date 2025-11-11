/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include <signal.h>
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
int __retarget_lock_try_acquire(struct __lock * p);
int __retarget_lock_try_acquire_recursive(struct __lock *p);
#endif

#if CONFIG_SECURE_ENABLE_TEE
struct _reent_stub {
    int _errno;
    __FILE *_stdin, *_stdout, *_stderr;
    int _inc;
    char *_emergency;
    int _reserved_0;
    int _reserved_1;
    struct __locale_t *_locale;
    void *_mp;
    void (*__cleanup)(struct _reent *);
    int _gamma_signgam;
    int _cvtlen;
    char *_cvtbuf;
    struct _rand48 *_r48;
#if 0 /* unlikely used fields in ROM implementation */
    struct __tm *_localtime_buf;
    char *_asctime_buf;
    void (** _sig_func)(int);
    struct _atexit *_reserved_6;
    struct _atexit _reserved_7;
    struct _glue _reserved_8;
    __FILE *__sf;
    struct _misc_reent *_misc;
    char *_signal_buf;
#endif
};

void *__getreent_rom_stub(void)
{
    static struct _reent_stub reent_stub;
    return &reent_stub;
}
#endif // SECURE_ENABLE_TEE

static struct syscall_stub_table s_stub_table = {
#if CONFIG_SECURE_ENABLE_TEE
    /*
     * ESP-TEE uses snprintf() from ROM, which requires at least a fake __getreent stub.
     *
     * NOTE: If floating-point variables are intended to be used,
     *       the following fields must be specified in the syscall_stub_table:
     *   ._printf_float =
     *   ._scanf_float =
     */
    .__getreent = (void *)__getreent_rom_stub,
#else
    .__getreent = (void *)abort,
#endif
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

/*
 * Initialize stdin, stdout, and stderr using static memory allocation.
 * Creating them with fopen() would call malloc() internally.
 */
static char write_buf[BUFSIZ];
static char read_buf[BUFSIZ];

static struct __file_bufio __stdin = FDEV_SETUP_BUFIO(0, read_buf, BUFSIZ, read, write, lseek, close, __SRD, 0);
static struct __file_bufio __stdout = FDEV_SETUP_BUFIO(1, write_buf, BUFSIZ, read, write, lseek, close, __SWR, __BLBF);

FILE *stdin = &__stdin.xfile.cfile.file;
FILE *stdout = &__stdout.xfile.cfile.file;
FILE *stderr = &__stdout.xfile.cfile.file;

#if CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY
__thread FILE* tls_stdin = &__stdin.xfile.cfile.file;
__thread FILE* tls_stdout = &__stdout.xfile.cfile.file;
__thread FILE* tls_stderr = &__stdout.xfile.cfile.file;
#endif

#if CONFIG_VFS_SUPPORT_IO
void esp_libc_init_global_stdio(const char *stdio_dev)
{
    int stdin_fd = open(stdio_dev, O_RDONLY);
    assert(stdin_fd > 0);
    __stdin.ptr = (void *)(intptr_t)(stdin_fd);

    int stdout_fd = open(stdio_dev, O_WRONLY);
    assert(stdout_fd > 0);
    __stdout.ptr = (void *)(intptr_t)(stdout_fd);
}
#else  /* CONFIG_VFS_SUPPORT_IO */
void esp_libc_init_global_stdio(void)
{
    /* Nothing to do. */
}
#endif  /* CONFIG_VFS_SUPPORT_IO */

// Hook to force the linker to include this file
void esp_libc_include_init_funcs(void)
{
}
