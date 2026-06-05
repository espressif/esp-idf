/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Lock type backed by pthread mutexes.  A zero-initialized _lock_t is valid
 * and will be lazily created on first acquire (matching newlib/esp_libc
 * semantics).
 */
typedef void * _lock_t;

void _lock_init(_lock_t *plock);
void _lock_init_recursive(_lock_t *plock);
void _lock_close(_lock_t *plock);
void _lock_close_recursive(_lock_t *plock);
void _lock_acquire(_lock_t *plock);
void _lock_acquire_recursive(_lock_t *plock);
int  _lock_try_acquire(_lock_t *plock);
int  _lock_try_acquire_recursive(_lock_t *plock);
void _lock_release(_lock_t *plock);
void _lock_release_recursive(_lock_t *plock);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_LOCK_H__ */
