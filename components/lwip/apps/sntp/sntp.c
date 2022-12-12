/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include "esp_log.h"
#include "sntp.h"
#include "lwip/apps/sntp.h"

static const char *TAG = "sntp";

static volatile sntp_sync_mode_t sntp_sync_mode = SNTP_SYNC_MODE_IMMED;
static volatile sntp_sync_status_t sntp_sync_status = SNTP_SYNC_STATUS_RESET;
static sntp_sync_time_cb_t time_sync_notification_cb = NULL;
static uint32_t s_sync_interval = CONFIG_LWIP_SNTP_UPDATE_DELAY;

inline void sntp_set_sync_status(sntp_sync_status_t sync_status)
{
    sntp_sync_status = sync_status;
}

void __attribute__((weak)) sntp_sync_time(struct timeval *tv)
{
    if (sntp_sync_mode == SNTP_SYNC_MODE_IMMED) {
        settimeofday(tv, NULL);
        sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
    } else if (sntp_sync_mode == SNTP_SYNC_MODE_SMOOTH) {
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        int64_t cpu_time = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
        int64_t sntp_time = (int64_t)tv->tv_sec * 1000000L + (int64_t)tv->tv_usec;
        int64_t delta = sntp_time - cpu_time;
        struct timeval tv_delta = { .tv_sec = delta / 1000000L, .tv_usec = delta % 1000000L };
        if (adjtime(&tv_delta, NULL) == -1) {
            ESP_LOGD(TAG, "Function adjtime don't update time because the error is very big");
            settimeofday(tv, NULL);
            ESP_LOGD(TAG, "Time was synchronized through settimeofday");
            sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
        } else {
            sntp_set_sync_status(SNTP_SYNC_STATUS_IN_PROGRESS);
        }
    }
    if (time_sync_notification_cb) {
        time_sync_notification_cb(tv);
    }
}

void sntp_set_sync_mode(sntp_sync_mode_t sync_mode)
{
    sntp_sync_mode = sync_mode;
}

sntp_sync_mode_t sntp_get_sync_mode(void)
{
    return sntp_sync_mode;
}

// set a callback function for time synchronization notification
void sntp_set_time_sync_notification_cb(sntp_sync_time_cb_t callback)
{
    time_sync_notification_cb = callback;
}

sntp_sync_status_t sntp_get_sync_status(void)
{
    sntp_sync_status_t ret_sync_status = SNTP_SYNC_STATUS_RESET;
    sntp_sync_status_t sync_status = sntp_sync_status;
    if (sync_status == SNTP_SYNC_STATUS_COMPLETED) {
        sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);
        ret_sync_status = SNTP_SYNC_STATUS_COMPLETED;
    } else if (sync_status == SNTP_SYNC_STATUS_IN_PROGRESS) {
        struct timeval outdelta;
        adjtime(NULL, &outdelta);
        if (outdelta.tv_sec == 0 && outdelta.tv_usec == 0) {
            sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);
            ret_sync_status = SNTP_SYNC_STATUS_COMPLETED;
        } else {
            ret_sync_status = SNTP_SYNC_STATUS_IN_PROGRESS;
        }
    }
    return ret_sync_status;
}

void sntp_set_sync_interval(uint32_t interval_ms)
{
    if (interval_ms < 15000) {
        // SNTPv4 RFC 4330 enforces a minimum update time of 15 seconds
        interval_ms = 15000;
    }
    s_sync_interval = interval_ms;
}

uint32_t sntp_get_sync_interval(void)
{
    return s_sync_interval;
}

bool sntp_restart(void)
{
    if (sntp_enabled()) {
        sntp_stop();
        sntp_init();
        return true;
    }
    return false;
}

void sntp_set_system_time(uint32_t sec, uint32_t us)
{
    // Note: SNTP/NTP timestamp is defined as 64-bit fixed point int
    // in seconds from 1900 (integer part is the first 32 bits)
    // which overflows in 2036.
    // The lifetime of the NTP timestamps has been extended by convention
    // of the MSB bit 0 to span between 1968 and 2104. This is implemented
    // in lwip sntp module, so this API returns number of seconds/milliseconds
    // representing dates range from 1968 to 2104.
    // (see: RFC-4330#section-3 and https://github.com/lwip-tcpip/lwip/blob/239918cc/src/apps/sntp/sntp.c#L129-L134)
    // Warning: Here, we convert the 32 bit NTP timestamp to 64 bit representation
    // of system time (time_t). This won't work for timestamps in future
    // after some time in 2104
    struct timeval tv = { .tv_sec = sec, .tv_usec = us };
    sntp_sync_time(&tv);
}

void sntp_get_system_time(uint32_t *sec, uint32_t *us)
{
    struct timeval tv = { .tv_sec = 0, .tv_usec = 0 };
    gettimeofday(&tv, NULL);
    // Warning: Here, we convert 64 bit representation of system time (time_t) to
    // 32 bit NTP timestamp. This won't work for future timestamps after some time in 2104
    // (see: RFC-4330#section-3)
    *(sec) = tv.tv_sec;
    *(us) = tv.tv_usec;
    sntp_set_sync_status(SNTP_SYNC_STATUS_RESET);
}
