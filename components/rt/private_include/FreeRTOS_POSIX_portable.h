/*
 * Amazon FreeRTOS+POSIX V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2018 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file FreeRTOS_POSIX_portable.h
 * @brief Port-specific configuration of FreeRTOS+POSIX.
 */

#ifndef _FREERTOS_POSIX_PORTABLE_H_
#define _FREERTOS_POSIX_PORTABLE_H_

/* ESP-IDF already defines the following types. */
#define posixconfigENABLE_CLOCK_T                0
#define posixconfigENABLE_CLOCKID_T              0
#define posixconfigENABLE_MODE_T                 0
#define posixconfigENABLE_PTHREAD_ATTR_T         0
#define posixconfigENABLE_PTHREAD_COND_T         0
#define posixconfigENABLE_PTHREAD_CONDATTR_T     0
#define posixconfigENABLE_PTHREAD_MUTEX_T        0
#define posixconfigENABLE_PTHREAD_MUTEXATTR_T    0
#define posixconfigENABLE_PTHREAD_T              0
#define posixconfigENABLE_TIME_T                 0
#define posixconfigENABLE_TIMER_T                0
#define posixconfigENABLE_TIMESPEC               0
#define posixconfigENABLE_ITIMERSPEC             0

/* ESP-IDF already provides the header sched.h. Exclude them by
 * activating the double inclusion guards. */
#define _FREERTOS_POSIX_SCHED_H_

#endif /* _FREERTOS_POSIX_PORTABLE_H_ */
