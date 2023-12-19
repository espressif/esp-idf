/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbedtls/build_info.h"
#include "mbedtls/platform_time.h"

#ifdef MBEDTLS_PLATFORM_MS_TIME_ALT
mbedtls_ms_time_t mbedtls_ms_time(void)
{
    int ret;
    struct timespec tv = {};
    mbedtls_ms_time_t current_ms;

    ret = clock_gettime(CLOCK_MONOTONIC, &tv);
    if (ret) {
        return time(NULL) * 1000L;
    }

    current_ms = tv.tv_sec;
    return current_ms * 1000L + tv.tv_nsec / 1000000L;
}
#endif // MBEDTLS_PLATFORM_MS_TIME_ALT
