/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
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

/* Dynamic Thread Scheduling Parameters Access */
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param);

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);

/* Set Scheduling Priority of a Thread */
int pthread_setschedprio(pthread_t thread, int prio);

int sched_get_priority_min(int policy);

int sched_get_priority_max(int policy);

#ifdef __cplusplus
}
#endif

#endif // __ESP_PLATFORM_PTHREAD_H__
