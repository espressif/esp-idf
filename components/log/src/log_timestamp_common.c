/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include "esp_log_timestamp.h"
#include "esp_private/log_util.h"
#include "esp_private/log_timestamp.h"
#include "sdkconfig.h"

#ifndef NON_OS_BUILD
#include <sys/lock.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

char *esp_log_system_timestamp(void)
{
    static char buffer[18] = {0};
    static _lock_t bufferLock = 0;

#if CONFIG_IDF_TARGET_LINUX
    const bool early_log = false;
#else
    const bool early_log = xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED;
#endif
    if (early_log) {
        uint32_t timestamp = esp_log_early_timestamp();
        for (uint8_t i = 0; i < sizeof(buffer); i++) {
            if ((timestamp > 0) || (i == 0)) {
                for (uint8_t j = sizeof(buffer) - 1; j > 0; j--) {
                    buffer[j] = buffer[j - 1];
                }
                buffer[0] = (char)(timestamp % 10) + '0';
                timestamp /= 10;
            } else {
                buffer[i] = 0;
                break;
            }
        }
    } else {
        struct timeval tv;
        struct tm timeinfo;

        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &timeinfo);

        _lock_acquire(&bufferLock);
        unsigned msec = tv.tv_usec / 1000;
        snprintf(buffer, sizeof(buffer),
                 "%02d:%02d:%02d.%03d",
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec,
                 msec);
        _lock_release(&bufferLock);
    }
    return buffer;
}
#endif // !NON_OS_BUILD

uint64_t esp_log_timestamp64(bool critical)
{
    uint64_t timestamp_ms;
#if CONFIG_BOOTLOADER_LOG_TIMESTAMP_SOURCE_NONE || CONFIG_LOG_TIMESTAMP_SOURCE_NONE
    (void) critical;
    timestamp_ms = 0;
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM || CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM_FULL
    if (critical) {
        timestamp_ms = esp_log_early_timestamp();
    } else {
#if CONFIG_IDF_TARGET_LINUX
        struct timespec tv;
        int result = clock_gettime(CLOCK_MONOTONIC, &tv);
        assert(result == 0);
        timestamp_ms = tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        timestamp_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
    }
#else
    (void) critical;
    timestamp_ms = esp_log_timestamp();
#endif
    return timestamp_ms;
}

char* esp_log_timestamp_str(bool critical, uint64_t timestamp_ms, char* buffer)
{
    char* out_buffer = buffer;
#if CONFIG_BOOTLOADER_LOG_TIMESTAMP_SOURCE_NONE || CONFIG_LOG_TIMESTAMP_SOURCE_NONE
    (void)critical;
    *buffer = '\0';
#elif CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM || CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM_FULL
    if (critical) {
        esp_log_util_cvt_dec(timestamp_ms, 0, buffer);
    } else {
        struct tm timeinfo;
        time_t sec = timestamp_ms / 1000;
        uint64_t msec = timestamp_ms % 1000;
        localtime_r(&sec, &timeinfo);
#if CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM_FULL
        // it takes 22 bytes to output it in the format: "YY-MM-DD HH:MM:SS.sss"
        buffer += esp_log_util_cvt_dec(timeinfo.tm_year, 2, buffer);
        *buffer++ = '-';
        buffer += esp_log_util_cvt_dec(timeinfo.tm_mon + 1, 2, buffer);
        *buffer++ = '-';
        buffer += esp_log_util_cvt_dec(timeinfo.tm_mday, 2, buffer);
        *buffer++ = ' ';
#endif // CONFIG_LOG_TIMESTAMP_SOURCE_SYSTEM_FULL
        buffer += esp_log_util_cvt_dec(timeinfo.tm_hour, 2, buffer);
        *buffer++ = ':';
        buffer += esp_log_util_cvt_dec(timeinfo.tm_min,  2, buffer);
        *buffer++ = ':';
        buffer += esp_log_util_cvt_dec(timeinfo.tm_sec,  2, buffer);
        *buffer++ = '.';
        esp_log_util_cvt_dec(msec, 3, buffer); // (ms)
    }
#else
    (void)critical;
    esp_log_util_cvt_dec(timestamp_ms, 0, buffer);
#endif
    return out_buffer;
}
