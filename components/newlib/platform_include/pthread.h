/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ESP_PLATFORM_PTHREAD_H__
#define __ESP_PLATFORM_PTHREAD_H__

#include <sys/types.h>
#include <sys/time.h>
#include <sys/features.h>

#include_next <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

int pthread_condattr_getclock(const pthread_condattr_t * attr, clockid_t * clock_id);

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);

#ifdef __cplusplus
}
#endif

#endif // __ESP_PLATFORM_PTHREAD_H__
