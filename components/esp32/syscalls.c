// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/reent.h>
#include <stdlib.h>
#include "rom/libc_stubs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

int uart_tx_one_char(uint8_t c);

void abort() {
    do
    {
        __asm__ ("break 0,0");
        *((int*) 0) = 0;
    } while(true);
}

void* _malloc_r(struct _reent *r, size_t size) {
    return pvPortMalloc(size);
}

void _free_r(struct _reent *r, void* ptr) {
    return vPortFree(ptr);
}

// TODO: improve realloc to grow buffer in place if possible
void* _realloc_r(struct _reent *r, void* ptr, size_t size) {
	void* new_chunk = pvPortMalloc(size);
	if (new_chunk) {
		memcpy(new_chunk, ptr, size);
		vPortFree(ptr);
	}
	// realloc behaviour: don't free original chunk if alloc failed
	return new_chunk;
}

void* _calloc_r(struct _reent *r, size_t count, size_t size) {
    void* result = pvPortMalloc(count * size);
    if (result)
    {
    	memset(result, 0, count * size);
    }
    return result;
}

int _system_r(struct _reent *r, const char *str) {
    abort();
    return 0;
}

int _rename_r(struct _reent *r, const char *src, const char *dst) {
    abort();
    return 0;
}

clock_t _times_r(struct _reent *r, struct tms *ptms) {
    abort();
    return 0;
}

// TODO: read time from RTC
int _gettimeofday_r(struct _reent *r, struct timeval *tv, void *tz) {
    abort();
    return 0;
}

void _raise_r(struct _reent *r) {
    abort();
}

int _unlink_r(struct _reent *r, const char *path) {
    abort();
    return 0;
}

int _link_r(struct _reent *r, const char* n1, const char* n2) {
    abort();
    return 0;
}

int _stat_r(struct _reent *r, const char * path, struct stat * st) {
    return 0;
}

int _fstat_r(struct _reent *r, int fd, struct stat * st) {
	st->st_mode = S_IFCHR;
    return 0;
}

void* _sbrk_r(struct _reent *r, ptrdiff_t sz) {
    abort();
    return 0;
}

int _getpid_r(struct _reent *r) {
    abort();
    return 0;
}

int _kill_r(struct _reent *r, int pid, int sig) {
    abort();
    return 0;
}

void _exit_r(struct _reent *r, int e) {
    abort();
}

int _close_r(struct _reent *r, int fd) {
    return 0;
}

int _open_r(struct _reent *r, const char * path, int flags, int mode) {
    return 0;
}

ssize_t _write_r(struct _reent *r, int fd, const void * data, size_t size) {
	const char* p = (const char*) data;
    if (fd == STDOUT_FILENO) {
        while(size--) {
            uart_tx_one_char(*p);
            ++p;
        }
    }
    return size;
}

_off_t _lseek_r(struct _reent *r, int fd, _off_t size, int mode) {
    return 0;
}

// TODO: implement reading from UART
ssize_t _read_r(struct _reent *r, int fd, void * dst, size_t size) {
    return 0;
}

// TODO: implement locks via FreeRTOS mutexes
void _lock_init(_lock_t *lock) {
}

void _lock_init_recursive(_lock_t *lock) {
}

void _lock_close(_lock_t *lock) {
}

void _lock_close_recursive(_lock_t *lock) {
}

void _lock_acquire(_lock_t *lock) {
}

void _lock_acquire_recursive(_lock_t *lock) {
}

int _lock_try_acquire(_lock_t *lock) {
    return 0;
}

int _lock_try_acquire_recursive(_lock_t *lock) {
    return 0;
}

void _lock_release(_lock_t *lock) {
}

void _lock_release_recursive(_lock_t *lock) {
}

static struct _reent s_reent;

/*
 General ToDo that the Xtensa newlib support code did but we do not: Close every open fd a running task had when the task
 is killed. Do we want that too? - JD
*/

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
    ._rename_r = &_rename_r,
    ._times_r = &_times_r,
    ._gettimeofday_r = &_gettimeofday_r,
    ._raise_r = &_raise_r,
    ._unlink_r = &_unlink_r,
    ._link_r = &_link_r,
    ._stat_r = &_stat_r,
    ._fstat_r = &_fstat_r,
    ._sbrk_r = &_sbrk_r,
    ._getpid_r = &_getpid_r,
    ._kill_r = &_kill_r,
    ._exit_r = &_exit_r,
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
    ._printf_float = &_printf_float,
    ._scanf_float = &_scanf_float,
};

void ets_setup_syscalls() {
	syscall_table_ptr_pro = &s_stub_table;
	syscall_table_ptr_app = &s_stub_table;
	_GLOBAL_REENT = &s_reent;
    environ = malloc(sizeof(char*));
    environ[0] = NULL;
}


