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
 * @file utils.h
 * @brief Utility functions used by FreeRTOS+POSIX.
 */

#ifndef _FREERTOS_POSIX_UTILS_
#define _FREERTOS_POSIX_UTILS_

/* C standard library includes. */
#include <stdbool.h>
#include <stdint.h>

#include <time.h>

#define MICROSECONDS_PER_SECOND    ( 1000000LL )                                   /**< Microseconds per second. */
#define NANOSECONDS_PER_SECOND     ( 1000000000LL )                                /**< Nanoseconds per second. */
#define NANOSECONDS_PER_TICK       ( NANOSECONDS_PER_SECOND / configTICK_RATE_HZ )

/**
 * @brief Calculates the length of pcString, up to xMaxLength.
 *
 * @param[in] pcString The string to find the length of.
 * @param[in] xMaxLength The limit when searching for the end of pcString.
 *
 * @return 0 if pcString is NULL; otherwise, the length of pcString or xMaxLength,
 * whichever is smaller.
 */
size_t UTILS_strnlen( const char * const pcString,
                      size_t xMaxLength );

/**
 * @brief Calculates the number of ticks between now and a given timespec.
 *
 * @param[in] pxAbsoluteTime A time in the future, specified as seconds and
 * nanoseconds since CLOCK_REALTIME's 0.
 * @param[out] pxResult Where the result of the conversion is stored. The result
 * is rounded up for fractional ticks.
 *
 * @return 0 on success. Otherwise, ETIMEDOUT if pxAbsoluteTime is in the past,
 * or EINVAL for invalid parameters.
 */
int UTILS_AbsoluteTimespecToTicks( const struct timespec * const pxAbsoluteTime,
                                   TickType_t * const pxResult );

/**
 * @brief Converts a struct timespec to FreeRTOS ticks.
 *
 * @param[in] pxTimespec The timespec to convert.
 * @param[out] pxResult Where the result of the conversion is stored. The result is rounded
 * up for fractional ticks.
 *
 * @return 0 on success. Otherwise, EINVAL for invalid parameters.
 */
int UTILS_TimespecToTicks( const struct timespec * const pxTimespec,
                           TickType_t * const pxResult );

/**
 * @brief Converts an integer value to a timespec.
 *
 * @param[in] llSource The value to convert.
 * @param[out] pxDestination Where to store the converted value.
 *
 * @return No return value.
 */
void UTILS_NanosecondsToTimespec( int64_t llSource,
                                  struct timespec * const pxDestination );

/**
 * @brief Calculates pxResult = x + y.
 *
 * @param[out] pxResult Where the result of the calculation is stored.
 * @param[in] x The first argument for addition.
 * @param[in] y The second argument for addition.
 *
 * @return -1 if any argument was NULL; 1 if result is negative; otherwise, 0.
 */
int UTILS_TimespecAdd( struct timespec * const pxResult,
                       const struct timespec * const x,
                       const struct timespec * const y );

/**
 * @brief Calculates pxResult = x + ( struct timespec ) nanosec.
 *
 * @param[out] pxResult Where the result of the calculation is stored.
 * @param[in] x The first argument for addition.
 * @param[in] llNanoseconds The second argument for addition.
 *
 * @return -1 if pxResult or x was NULL; 1 if result is negative; otherwise, 0.
 */
int UTILS_TimespecAddNanoseconds( struct timespec * const pxResult,
                                  const struct timespec * const x,
                                  int64_t llNanoseconds );

/**
 * @brief Calculates pxResult = x - y.
 *
 * @param[out] pxResult Where the result of the calculation is stored.
 * @param[in] x The first argument for subtraction.
 * @param[in] y The second argument for subtraction.
 *
 * @return -1 if any argument was NULL; 1 if result is negative; otherwise, 0.
 */
int UTILS_TimespecSubtract( struct timespec * const pxResult,
                            const struct timespec * const x,
                            const struct timespec * const y );

/**
 * @brief Checks that a timespec conforms to POSIX.
 *
 * A valid timespec must have 0 <= tv_nsec < 1000000000.
 *
 * @param[in] pxTimespec The timespec to validate.
 *
 * @return true if the pxTimespec is valid, false otherwise.
 */
bool UTILS_ValidateTimespec( const struct timespec * const pxTimespec );

#endif /* ifndef _FREERTOS_POSIX_UTILS_ */
