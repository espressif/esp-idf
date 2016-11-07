// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "rom/libc_stubs.h"
#include "esp_vfs.h"
#include "esp_newlib.h"
#include "sdkconfig.h"

static struct _reent s_reent;

extern int _printf_float(struct _reent *rptr,
               void *pdata,
               FILE * fp,
               int (*pfunc) (struct _reent *, FILE *, _CONST char *, size_t len),
               va_list * ap);


extern int _scanf_float(struct _reent *rptr,
              void *pdata,
              FILE *fp,
              va_list *ap);


static struct syscall_stub_table s_stub_table = {
    .__getreent = &__getreent,
    ._malloc_r = &_malloc_r,
    ._free_r = &_free_r,
    ._realloc_r = &_realloc_r,
    ._calloc_r = &_calloc_r,
    ._abort = &abort,
    ._system_r = &_system_r,
    ._rename_r = &esp_vfs_rename,
    ._times_r = &_times_r,
    ._gettimeofday_r = &_gettimeofday_r,
    ._raise_r = (void (*)(struct _reent *r)) &_raise_r,
    ._unlink_r = &esp_vfs_unlink,
    ._link_r = &esp_vfs_link,
    ._stat_r = &esp_vfs_stat,
    ._fstat_r = &esp_vfs_fstat,
    ._sbrk_r = &_sbrk_r,
    ._getpid_r = &_getpid_r,
    ._kill_r = &_kill_r,
    ._exit_r = NULL,    // never called in ROM
    ._close_r = &esp_vfs_close,
    ._open_r = &esp_vfs_open,
    ._write_r = (int (*)(struct _reent *r, int, const void *, int)) &esp_vfs_write,
    ._lseek_r = (int (*)(struct _reent *r, int, int, int)) &esp_vfs_lseek,
    ._read_r = (int (*)(struct _reent *r, int, void *, int)) &esp_vfs_read,
    ._lock_init = &_lock_init,
    ._lock_init_recursive = &_lock_init_recursive,
    ._lock_close = &_lock_close,
    ._lock_close_recursive = &_lock_close,
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

void esp_setup_syscall_table()
{
    syscall_table_ptr_pro = &s_stub_table;
    syscall_table_ptr_app = &s_stub_table;
    _GLOBAL_REENT = &s_reent;
    environ = malloc(sizeof(char*));
    environ[0] = NULL;
}


