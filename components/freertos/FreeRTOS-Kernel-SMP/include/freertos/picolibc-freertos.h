/*
 * FreeRTOS Kernel V11.1.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-FileCopyrightText: 2021 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef INC_PICOLIBC_FREERTOS_H
#define INC_PICOLIBC_FREERTOS_H

/* Use picolibc TLS support to allocate space for __thread variables,
 * initialize them at thread creation and set the TLS context at
 * thread switch time.
 *
 * See the picolibc TLS docs:
 * https://github.com/picolibc/picolibc/blob/main/doc/tls.md
 * for additional information. */

#include <picotls.h>

#define configUSE_C_RUNTIME_TLS_SUPPORT    1

#define configTLS_BLOCK_TYPE               void *

#define picolibcTLS_SIZE                   ( ( portPOINTER_SIZE_TYPE ) _tls_size() )
#define picolibcSTACK_ALIGNMENT_MASK       ( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK )

#if __PICOLIBC_MAJOR__ > 1 || __PICOLIBC_MINOR__ >= 8

/* Picolibc 1.8 and newer have explicit alignment values provided
 * by the _tls_align() inline */
    #define picolibcTLS_ALIGNMENT_MASK    ( ( portPOINTER_SIZE_TYPE ) ( _tls_align() - 1 ) )
#else

/* For older Picolibc versions, use the general port alignment value */
    #define picolibcTLS_ALIGNMENT_MASK    ( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK )
#endif

/* Allocate thread local storage block off the end of the
 * stack. The picolibcTLS_SIZE macro returns the size (in
 * bytes) of the total TLS area used by the application.
 * Calculate the top of stack address. */
#if ( portSTACK_GROWTH < 0 )

    #define configINIT_TLS_BLOCK( xTLSBlock, pxTopOfStack )                                  \
    do {                                                                                     \
        xTLSBlock = ( void * ) ( ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) -              \
                                   picolibcTLS_SIZE ) &                                      \
                                 ~picolibcTLS_ALIGNMENT_MASK );                              \
        pxTopOfStack = ( StackType_t * ) ( ( ( ( portPOINTER_SIZE_TYPE ) xTLSBlock ) - 1 ) & \
                                           ~picolibcSTACK_ALIGNMENT_MASK );                  \
        _init_tls( xTLSBlock );                                                              \
    } while( 0 )
#else /* portSTACK_GROWTH */
    #define configINIT_TLS_BLOCK( xTLSBlock, pxTopOfStack )                                          \
    do {                                                                                             \
        xTLSBlock = ( void * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack +                          \
                                   picolibcTLS_ALIGNMENT_MASK ) & ~picolibcTLS_ALIGNMENT_MASK );     \
        pxTopOfStack = ( StackType_t * ) ( ( ( ( ( portPOINTER_SIZE_TYPE ) xTLSBlock ) +             \
                                               picolibcTLS_SIZE ) + picolibcSTACK_ALIGNMENT_MASK ) & \
                                           ~picolibcSTACK_ALIGNMENT_MASK );                          \
        _init_tls( xTLSBlock );                                                                      \
    } while( 0 )
#endif /* portSTACK_GROWTH */

#define configSET_TLS_BLOCK( xTLSBlock )    _set_tls( xTLSBlock )

#define configDEINIT_TLS_BLOCK( xTLSBlock )

#endif /* INC_PICOLIBC_FREERTOS_H */
