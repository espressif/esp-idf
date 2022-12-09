// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
#include "esp_vfs.h"
#include "esp_newlib.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/libc_stubs.h"
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
#ifdef CONFIG_NEWLIB_NANO_FORMAT
    ._printf_float = &_printf_float,
    ._scanf_float = &_scanf_float,
#else
    ._printf_float = NULL,
    ._scanf_float = NULL,
#endif
};

void esp_setup_syscall_table(void)
{
    syscall_table_ptr_pro = &s_stub_table;
#if SOC_CPU_CORES_NUM == 2
    syscall_table_ptr_app = &s_stub_table;
#endif
    _GLOBAL_REENT = &s_reent;
    environ = malloc(sizeof(char*));
    environ[0] = NULL;
}


