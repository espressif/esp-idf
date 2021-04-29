// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License


#include "esp_openthread_alarm.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_openthread_common_macro.h"
#include "esp_timer.h"
#include "common/logging.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "openthread/platform/alarm-micro.h"
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/time.h"

static uint64_t s_alarm_ms_t0   = 0;
static uint64_t s_alarm_ms_dt   = 0;
static bool     s_is_ms_running = false;
static uint64_t s_alarm_us_t0   = 0;
static uint64_t s_alarm_us_dt   = 0;
static bool     s_is_us_running = false;

uint64_t otPlatTimeGet(void)
{
    struct timeval tv_now;

    int err = gettimeofday(&tv_now, NULL);
    assert(err == 0);

    return (uint64_t)tv_now.tv_sec * US_PER_S + tv_now.tv_usec;
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_alarm_ms_t0   = aT0;
    s_alarm_ms_dt   = aDt;
    s_is_ms_running = true;

    otLogDebgPlat("Millisecond timer alarm start running, t0=%llu, dt=%llu", s_alarm_ms_t0, s_alarm_ms_dt);
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_is_ms_running = false;
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return esp_timer_get_time() / US_PER_MS;
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_alarm_us_t0 = aT0;
    s_alarm_us_dt = aDt;
    s_is_us_running = true;

    otLogDebgPlat("Microsecond timer alarm start running, t0=%llu, dt=%llu", s_alarm_us_t0, s_alarm_us_dt);
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    s_is_us_running = false;
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return esp_timer_get_time();
}

void esp_openthread_alarm_update(esp_openthread_mainloop_context_t *mainloop)
{
    struct timeval *timeout = &mainloop->timeout;
    uint32_t        now     = otPlatAlarmMicroGetNow();
    int64_t         remain_min_time_us  = INT64_MAX;
    int64_t         remaining_us = 0;
    if (s_is_ms_running) {
        remaining_us = (s_alarm_ms_dt + s_alarm_ms_t0) * US_PER_MS - now;
        if (remain_min_time_us > remaining_us) {
            remain_min_time_us = remaining_us;
        }
    }
    if (s_is_us_running) {
        remaining_us = s_alarm_us_dt + s_alarm_us_t0 - now;
        if (remain_min_time_us > remaining_us) {
            remain_min_time_us = remaining_us;
        }
    }
    if (remain_min_time_us > 0) {
        timeout->tv_sec    = remain_min_time_us / US_PER_S;
        timeout->tv_usec   = remain_min_time_us % US_PER_S;
    } else {
        timeout->tv_sec  = 0;
        timeout->tv_usec = 0;
    }
}

void esp_openthread_alarm_process(otInstance *aInstance)
{
    if (s_is_ms_running && s_alarm_ms_t0 + s_alarm_ms_dt <= otPlatAlarmMilliGetNow()) {
        s_is_ms_running = false;

#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet()) {
            otPlatDiagAlarmFired(aInstance);
        } else
#endif
        {
            otPlatAlarmMilliFired(aInstance);
        }

        otLogDebgPlat("Millisecond timer alarm fired");
    }
    if (s_is_us_running && s_alarm_us_t0 + s_alarm_us_dt <= otPlatAlarmMicroGetNow()) {
        s_is_us_running = false;
        otPlatAlarmMicroFired(aInstance);
        otLogDebgPlat("Microsecond timer alarm fired");
    }
}
