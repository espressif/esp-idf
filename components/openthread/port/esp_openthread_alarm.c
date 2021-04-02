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
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/time.h"

static uint64_t s_alarm_t0   = 0;
static uint64_t s_alarm_dt   = 0;
static bool     s_is_running = false;

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

    s_alarm_t0   = aT0;
    s_alarm_dt   = aDt;
    s_is_running = true;

    otLogDebgPlat("alarm start running, t0=%llu, dt=%llu", s_alarm_t0, s_alarm_dt);
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_is_running = false;
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    return esp_timer_get_time() / US_PER_MS;
}

void esp_openthread_alarm_update(esp_openthread_mainloop_context_t *mainloop)
{
    struct timeval *timeout = &mainloop->timeout;
    uint32_t        now     = otPlatAlarmMilliGetNow();

    if (!s_is_running) {
        timeout->tv_sec  = INT32_MAX;
        timeout->tv_usec = 0;
    } else if (s_alarm_t0 + s_alarm_dt > now) {
        uint64_t remaining = s_alarm_dt + s_alarm_t0 - now;
        timeout->tv_sec    = remaining / MS_PER_S;
        timeout->tv_usec   = (remaining % MS_PER_S) * US_PER_MS;
    } else {
        timeout->tv_sec  = 0;
        timeout->tv_usec = 0;
    }
}

void esp_openthread_alarm_process(otInstance *aInstance)
{
    if (s_is_running && s_alarm_t0 + s_alarm_dt <= otPlatAlarmMilliGetNow()) {
        s_is_running = false;

#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet()) {
            otPlatDiagAlarmFired(aInstance);
        } else
#endif
        {
            otPlatAlarmMilliFired(aInstance);
        }

        otLogDebgPlat("alarm fired");
    }
}
