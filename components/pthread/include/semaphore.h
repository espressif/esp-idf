/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int sem_t;

/**
 * This is the maximum value to which any POSIX semaphore can count on ESP chips.
 */
#define SEM_VALUE_MAX 0x7FFF

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 *
 * Must NOT be called if threads are still blocked on semaphore!
 */
int sem_destroy(sem_t *sem);

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 *
 * Note that on ESP chips, pshared is ignored. Semaphores can always be shared between FreeRTOS tasks.
 */
int sem_init(sem_t *sem, int pshared, unsigned value);

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 *
 * Note that, unlike specified in POSIX, this implementation returns -1 and sets errno to
 * EAGAIN if the semaphore can not be unlocked (posted) due to its value being SEM_VALUE_MAX.
 */
int sem_post(sem_t *sem);

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 *
 * Note the following three deviations/issues originating from the underlying FreeRTOS implementation:
 * * The time value passed by abstime will be rounded up to the next FreeRTOS tick.
 * * The actual timeout will happen after the tick the time was rounded to
 *   and before the following tick.
 * * It is possible, though unlikely, that the task is preempted directly after the timeout calculation,
 *   delaying timeout of the following blocking operating system call by the duration of the preemption.
 */
int sem_timedwait(sem_t *semaphore, const struct timespec *abstime);

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 */
int sem_trywait(sem_t *sem);

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 */
int sem_wait(sem_t *sem);

/**
 * This is a POSIX function, please refer to the POSIX specification for a detailed description.
 */
int sem_getvalue(sem_t *sem, int *sval);

#ifdef __cplusplus
}
#endif
