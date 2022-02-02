/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __SYS_LOCK_H__
#define __SYS_LOCK_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


/* newlib locks implementation for CONFIG_IDF_TARGET_LINUX, single threaded.
 * Note, currently this doesn't implement the functions required
 * when _RETARGETABLE_LOCKING is defined. They should be added.
 */


/* Compatibility definitions for legacy newlib locking functions */
typedef int _lock_t;

static inline void _lock_init(_lock_t *plock) {}
static inline void _lock_init_recursive(_lock_t *plock) {}
static inline void _lock_close(_lock_t *plock) {}
static inline void _lock_close_recursive(_lock_t *plock) {}
static inline void _lock_acquire(_lock_t *plock) {}
static inline void _lock_acquire_recursive(_lock_t *plock) {}
static inline int _lock_try_acquire(_lock_t *plock)
{
    return 1;
}
static inline int _lock_try_acquire_recursive(_lock_t *plock)
{
    return 1;
}
static inline void _lock_release(_lock_t *plock) {}
static inline void _lock_release_recursive(_lock_t *plock) {}


#ifdef __cplusplus
}
#endif

#endif /* __SYS_LOCK_H__ */
