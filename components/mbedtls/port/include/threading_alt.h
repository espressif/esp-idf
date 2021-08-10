/*
 * corePKCS11 V2.0.0
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF NY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef __THREADING_ALT_H__
#define __THREADING_ALT_H__


#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @brief Mutex struct used to synchronize mbed TLS operations.
 *
 */
typedef struct
{
    SemaphoreHandle_t mutex; /**< @brief FreeRTOS semaphore. */
    char is_valid;           /**< @brief Flag used by mbedTLS to track wether a mutex is valid. */
} mbedtls_threading_mutex_t;

extern void mbedtls_threading_set_alt( void ( * mutex_init )( mbedtls_threading_mutex_t * ),
                                       void ( * mutex_free )( mbedtls_threading_mutex_t * ),
                                       int ( * mutex_lock )( mbedtls_threading_mutex_t * ),
                                       int ( * mutex_unlock )( mbedtls_threading_mutex_t * ) );


#endif /* ifndef __THREADING_ALT_H__ */