/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <pthread.h>
#include <dlfcn.h>
#include <sys/types.h>
#include "esp_err.h"
#include "errno.h"

/** This module addresses the FreeRTOS simulator's coexistence with linux system calls from user apps.
 *  It's only included when building without lwIP, so we need to use linux system's select() which would receive
 *  EINTR event on every FreeRTOS interrupt; we workaround this problem by wrapping select()
 *  to bypass and silence these events.
 */
typedef int (*select_func_t) (int fd, fd_set * rfds, fd_set * wfds, fd_set *efds, struct timeval *tval);

static inline int64_t get_us(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_nsec / 1000 + spec.tv_sec * 1000000;
}

int select (int fd, fd_set * rfds, fd_set * wfds, fd_set *efds, struct timeval *tval)
{
    int ret;
    struct timeval *tv = tval;
    struct timeval timeval_local = {};
    int64_t start = 0;
    int64_t timeout_us = 0;
    select_func_t real_select = (select_func_t) dlsym(RTLD_NEXT, "select");
    if (tv != NULL) {
        start = get_us();
        timeout_us = tval->tv_sec * 1000000 + tval->tv_usec;
        timeval_local.tv_sec = tval->tv_sec;
        timeval_local.tv_usec = tval->tv_usec;
        tv = &timeval_local;  // this (tv != NULL) indicates that we should handle timeouts
    }
    while ((ret = real_select(fd, rfds, wfds, efds, tv)) < 0 && errno == EINTR) {
        if (tv != NULL) {
            int64_t now = get_us();
            timeout_us -= now - start;
            if (timeout_us < 0) {
                errno = 0;
                ret = 0;
                break;
            }
            start = now;
            tv->tv_usec = timeout_us % 1000000;
            tv->tv_sec = timeout_us / 1000000;
        }
    }
    return ret;
}
