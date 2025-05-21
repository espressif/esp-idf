/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "esp_random.h"

// NOTE: Remove compile-time warnings for the below newlib-provided functions
struct _reent *__getreent(void)
{
    return _GLOBAL_REENT;
}

int _fstat_r(struct _reent *r, int fd, struct stat *st)
{
    errno = ENOSYS;
    return -1;
}

int _close_r(struct _reent *r, int fd)
{
    errno = ENOSYS;
    return -1;
}

off_t _lseek_r(struct _reent *r, int fd, off_t offset, int whence)
{
    errno = ENOSYS;
    return -1;
}

ssize_t _read_r(struct _reent *r, int fd, void *ptr, size_t len)
{
    errno = ENOSYS;
    return -1;
}

ssize_t _write_r(struct _reent *r, int fd, const void *ptr, size_t len)
{
    errno = ENOSYS;
    return -1;
}

int _getpid_r(struct _reent *r)
{
    return 1;
}

int _kill_r(struct _reent *r, int pid, int sig)
{
    errno = ENOSYS;
    return -1;
}

int _getentropy_r(struct _reent *r, void *buffer, size_t length)
{
    esp_fill_random(buffer, length);
    return 0;
}

void *pthread_getspecific(pthread_key_t key)
{
    return NULL;
}

int pthread_setspecific(pthread_key_t key, const void *value)
{
    return 0;
}

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
{
    errno = ENOSYS;
    return -1;
}

int pthread_key_delete(pthread_key_t key)
{
    errno = ENOSYS;
    return -1;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    errno = ENOSYS;
    return -1;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    errno = ENOSYS;
    return -1;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    errno = ENOSYS;
    return -1;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    errno = ENOSYS;
    return -1;
}

void *__cxa_get_globals(void)
{
    return NULL;
}

void *__cxa_get_globals_fast(void)
{
    return NULL;
}

int __cxa_thread_atexit(void (*func)(void *), void *arg, void *dso)
{
    return 0;
}

#if CONFIG_IDF_TARGET_ESP32H2
void *_sbrk(ptrdiff_t incr)
{
    return (void *) -1;
}
#endif

void esp_tee_include_syscalls_impl(void)
{

}
