/*
 *  Portable interface to the CPU cycle counter
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

/*
 * mbedtls_timing_get_timer()m mbedtls_timing_set_delay() and
 * mbedtls_timing_set_delay only abstracted from mbedtls/library/timing.c
 * as that does not build on ESP-IDF but these 2 functions are needed for
 * DTLS (in particular mbedtls_ssl_set_timer_cb() must be called for DTLS
 * which requires these 2 delay functions).
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if !defined(MBEDTLS_ESP_TIMING_C)

#include <sys/time.h>
#include "mbedtls/timing.h"

struct _hr_time
{
    struct timeval start;
};

unsigned long mbedtls_timing_get_timer( struct mbedtls_timing_hr_time *val, int reset )
{
    struct _hr_time *t = (struct _hr_time *) val;

    if( reset )
    {
        gettimeofday( &t->start, NULL );
        return( 0 );
    }
    else
    {
        unsigned long delta;
        struct timeval now;
        gettimeofday( &now, NULL );
        delta = ( now.tv_sec  - t->start.tv_sec  ) * 1000ul
              + ( now.tv_usec - t->start.tv_usec ) / 1000;
        return( delta );
    }
}

/*
 * Set delays to watch
 */
void mbedtls_timing_set_delay( void *data, uint32_t int_ms, uint32_t fin_ms )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;

    ctx->int_ms = int_ms;
    ctx->fin_ms = fin_ms;

    if( fin_ms != 0 )
        (void) mbedtls_timing_get_timer( &ctx->timer, 1 );
}

/*
 * Get number of delays expired
 */
int mbedtls_timing_get_delay( void *data )
{
    mbedtls_timing_delay_context *ctx = (mbedtls_timing_delay_context *) data;
    unsigned long elapsed_ms;

    if( ctx->fin_ms == 0 )
        return( -1 );

    elapsed_ms = mbedtls_timing_get_timer( &ctx->timer, 0 );

    if( elapsed_ms >= ctx->fin_ms )
        return( 2 );

    if( elapsed_ms >= ctx->int_ms )
        return( 1 );

    return( 0 );
}

#endif /* MBEDTLS_ESP_TIMING_C */
