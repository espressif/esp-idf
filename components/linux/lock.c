/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include <stdlib.h>
#include <pthread.h>

/*
 * Protects lazy initialization of locks that were zero-initialized
 * (e.g. static _lock_t my_lock = 0;).  Two threads calling
 * _lock_acquire on such a lock race to create the underlying mutex;
 * this guard serialises the creation.
 */
static pthread_mutex_t s_lock_init_guard = PTHREAD_MUTEX_INITIALIZER;

static void lock_init_generic(_lock_t *plock, int recursive)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    if (recursive) {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    }
    pthread_mutex_t *m = calloc(1, sizeof(pthread_mutex_t));
    if (m == NULL) {
        pthread_mutexattr_destroy(&attr);
        abort();
    }
    pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);
    *plock = (_lock_t)m;
}

static void lazy_init(_lock_t *plock, int recursive)
{
    if (__atomic_load_n(plock, __ATOMIC_ACQUIRE) == NULL) {
        pthread_mutex_lock(&s_lock_init_guard);
        if (*plock == NULL) {
            lock_init_generic(plock, recursive);
        }
        pthread_mutex_unlock(&s_lock_init_guard);
    }
}

void _lock_init(_lock_t *plock)
{
    *plock = NULL;
    lock_init_generic(plock, 0);
}

void _lock_init_recursive(_lock_t *plock)
{
    *plock = NULL;
    lock_init_generic(plock, 1);
}

void _lock_close(_lock_t *plock)
{
    if (*plock) {
        pthread_mutex_destroy((pthread_mutex_t *)(*plock));
        free(*plock);
        *plock = NULL;
    }
}

void _lock_close_recursive(_lock_t *plock)
{
    _lock_close(plock);
}

void _lock_acquire(_lock_t *plock)
{
    lazy_init(plock, 0);
    pthread_mutex_lock((pthread_mutex_t *)(*plock));
}

void _lock_acquire_recursive(_lock_t *plock)
{
    lazy_init(plock, 1);
    pthread_mutex_lock((pthread_mutex_t *)(*plock));
}

int _lock_try_acquire(_lock_t *plock)
{
    lazy_init(plock, 0);
    return (pthread_mutex_trylock((pthread_mutex_t *)(*plock)) == 0) ? 0 : -1;
}

int _lock_try_acquire_recursive(_lock_t *plock)
{
    lazy_init(plock, 1);
    return (pthread_mutex_trylock((pthread_mutex_t *)(*plock)) == 0) ? 0 : -1;
}

void _lock_release(_lock_t *plock)
{
    if (*plock) {
        pthread_mutex_unlock((pthread_mutex_t *)(*plock));
    }
}

void _lock_release_recursive(_lock_t *plock)
{
    _lock_release(plock);
}
