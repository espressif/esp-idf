/*
 * SPDX-FileCopyrightText: 2015-2019 Cadence Design Systems, Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
/*
 * Copyright (c) 2015-2019 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * This utility helps benchmarking interrupt latency and context switches.
 * In order to enable it, set configBENCHMARK to 1 in FreeRTOSConfig.h.
 * You will also need to download the FreeRTOS_trace patch that contains
 * portbenchmark.c and the complete version of portbenchmark.h
 */

#ifndef PORTBENCHMARK_H
#define PORTBENCHMARK_H

#if configBENCHMARK
    #error "You need to download the FreeRTOS_trace patch that overwrites this file"
#endif

#define portbenchmarkINTERRUPT_DISABLE()
#define portbenchmarkINTERRUPT_RESTORE(newstate)
#define portbenchmarkIntLatency()
#define portbenchmarkIntWait()
#define portbenchmarkReset()
#define portbenchmarkPrint()

#endif /* PORTBENCHMARK */
