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
#include "esp_attr.h"
#include "rom/libc_stubs.h"
#include "rom/uart.h"
#include "soc/cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

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

void* _realloc_r(struct _reent *r, void* ptr, size_t size) {
	void* new_chunk;
	if (size == 0) {
		if (ptr) {
			vPortFree(ptr);
		}
		return NULL;
	}

	new_chunk = pvPortMalloc(size);
	if (new_chunk && ptr) {
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
        static _lock_t stdout_lock; /* lazily initialised */
        /* Even though newlib does stream locking on stdout, we need
           a dedicated stdout UART lock...

           This is because each task has its own _reent structure with
           unique FILEs for stdin/stdout/stderr, so these are
           per-thread (lazily initialised by __sinit the first time a
           stdio function is used, see findfp.c:235.

           It seems like overkill to allocate a FILE-per-task and lock
           a thread-local stream, but I see no easy way to fix this
           (pre-__sinit_, tasks have "fake" FILEs ie __sf_fake_stdout
           which aren't fully valid.)
        */
        _lock_acquire_recursive(&stdout_lock);
        while(size--) {
#if CONFIG_NEWLIB_STDOUT_ADDCR
            if (*p=='\n') {
                uart_tx_one_char('\r');
            }
#endif
            uart_tx_one_char(*p);
            ++p;
        }
        _lock_release_recursive(&stdout_lock);
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

/* Notes on our newlib lock implementation:
 *
 * - Use FreeRTOS mutex semaphores as locks.
 * - lock_t is int, but we store an xSemaphoreHandle there.
 * - Locks are no-ops until the FreeRTOS scheduler is running.
 * - Due to this, locks need to be lazily initialised the first time
 *   they are acquired. Initialisation/deinitialisation of locks is
 *   protected by lock_init_spinlock.
 * - Race conditions around lazy initialisation (via lock_acquire) are
 *   protected against.
 * - Anyone calling lock_close is reponsible for ensuring noone else
 *   is holding the lock at this time.
 * - Race conditions between lock_close & lock_init (for the same lock)
 *   are the responsibility of the caller.
 */

static portMUX_TYPE lock_init_spinlock = portMUX_INITIALIZER_UNLOCKED;

/* Initialise the given lock by allocating a new mutex semaphore
   as the _lock_t value.
*/
static void IRAM_ATTR lock_init_generic(_lock_t *lock, uint8_t mutex_type) {
    portENTER_CRITICAL(&lock_init_spinlock);
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        /* nothing to do until the scheduler is running */
        *lock = 0; /* ensure lock is zeroed out, in case it's an automatic variable */
        portEXIT_CRITICAL(&lock_init_spinlock);
        return;
    }

    if (*lock) {
         /* Lock already initialised (either we didn't check earlier,
          or it got initialised while we were waiting for the
          spinlock.) */
    }
    else
    {
        /* Create a new semaphore

           this is a bit of an API violation, as we're calling the
           private function xQueueCreateMutex(x) directly instead of
           the xSemaphoreCreateMutex / xSemaphoreCreateRecursiveMutex
           wrapper functions...

           The better alternative would be to pass pointers to one of
           the two xSemaphoreCreate___Mutex functions, but as FreeRTOS
           implements these as macros instead of inline functions
           (*party like it's 1998!*) it's not possible to do this
           without writing wrappers. Doing it this way seems much less
           spaghetti-like.
        */
        xSemaphoreHandle new_sem = xQueueCreateMutex(mutex_type);
        if (!new_sem) {
            abort(); /* No more semaphores available or OOM */
        }
        *lock = (_lock_t)new_sem;
    }
    portEXIT_CRITICAL(&lock_init_spinlock);
}

void IRAM_ATTR _lock_init(_lock_t *lock) {
    lock_init_generic(lock, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR _lock_init_recursive(_lock_t *lock) {
    lock_init_generic(lock, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

/* Free the mutex semaphore pointed to by *lock, and zero it out.

   Note that FreeRTOS doesn't account for deleting mutexes while they
   are held, and neither do we... so take care not to delete newlib
   locks while they may be held by other tasks!
*/
void IRAM_ATTR _lock_close(_lock_t *lock) {
    portENTER_CRITICAL(&lock_init_spinlock);
    if (*lock) {
        xSemaphoreHandle h = (xSemaphoreHandle)(*lock);
#if (INCLUDE_xSemaphoreGetMutexHolder == 1)
        configASSERT(xSemaphoreGetMutexHolder(h) == NULL); /* mutex should not be held */
#endif
        vSemaphoreDelete(h);
        *lock = 0;
    }
    portEXIT_CRITICAL(&lock_init_spinlock);
}

/* Acquire the mutex semaphore for lock. wait up to delay ticks.
   mutex_type is queueQUEUE_TYPE_RECURSIVE_MUTEX or queueQUEUE_TYPE_MUTEX
*/
static int IRAM_ATTR lock_acquire_generic(_lock_t *lock, uint32_t delay, uint8_t mutex_type) {
    xSemaphoreHandle h = (xSemaphoreHandle)(*lock);
    if (!h) {
        if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
            return 0; /* locking is a no-op before scheduler is up, so this "succeeds" */
        }
        /* lazy initialise lock - might have had a static initializer in newlib (that we don't use),
           or _lock_init might have been called before the scheduler was running... */
        lock_init_generic(lock, mutex_type);
        h = (xSemaphoreHandle)(*lock);
        configASSERT(h != NULL);
    }

    BaseType_t success;
    if (cpu_in_interrupt_context()) {
        /* In ISR Context */
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            abort(); /* recursive mutexes make no sense in ISR context */
        }
        BaseType_t higher_task_woken = false;
        success = xSemaphoreTakeFromISR(h, &higher_task_woken);
        if (!success && delay > 0) {
            abort(); /* Tried to block on mutex from ISR, couldn't... rewrite your program to avoid libc interactions in ISRs! */
        }
        if (higher_task_woken) {
            portYIELD_FROM_ISR();
        }
    }
    else {
        /* In task context */
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            success = xSemaphoreTakeRecursive(h, delay);
        } else {
            success = xSemaphoreTake(h, delay);
        }
    }

    return (success == pdTRUE) ? 0 : -1;
}

void IRAM_ATTR _lock_acquire(_lock_t *lock) {
    lock_acquire_generic(lock, portMAX_DELAY, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR _lock_acquire_recursive(_lock_t *lock) {
    lock_acquire_generic(lock, portMAX_DELAY, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

int IRAM_ATTR _lock_try_acquire(_lock_t *lock) {
    return lock_acquire_generic(lock, 0, queueQUEUE_TYPE_MUTEX);
}

int IRAM_ATTR _lock_try_acquire_recursive(_lock_t *lock) {
    return lock_acquire_generic(lock, 0, queueQUEUE_TYPE_RECURSIVE_MUTEX);
}

/* Release the mutex semaphore for lock.
   mutex_type is queueQUEUE_TYPE_RECURSIVE_MUTEX or queueQUEUE_TYPE_MUTEX
*/
static void IRAM_ATTR lock_release_generic(_lock_t *lock, uint8_t mutex_type) {
    xSemaphoreHandle h = (xSemaphoreHandle)(*lock);
    if (h == NULL) {
        /* This is probably because the scheduler isn't running yet,
           or the scheduler just started running and some code was
           "holding" a not-yet-initialised lock... */
        return;
    }

    if (cpu_in_interrupt_context()) {
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            abort(); /* indicates logic bug, it shouldn't be possible to lock recursively in ISR */
        }
        BaseType_t higher_task_woken = false;
        xSemaphoreGiveFromISR(h, &higher_task_woken);
        if (higher_task_woken) {
            portYIELD_FROM_ISR();
        }
    } else {
        if (mutex_type == queueQUEUE_TYPE_RECURSIVE_MUTEX) {
            xSemaphoreGiveRecursive(h);
        } else {
            xSemaphoreGive(h);
        }
    }
}

void IRAM_ATTR _lock_release(_lock_t *lock) {
    lock_release_generic(lock, queueQUEUE_TYPE_MUTEX);
}

void IRAM_ATTR _lock_release_recursive(_lock_t *lock) {
    lock_release_generic(lock, queueQUEUE_TYPE_RECURSIVE_MUTEX);
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
    ._lock_close_recursive = &_lock_close,
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


