/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_alarm.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_timer.h"
#include "openthread-core-config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "openthread/platform/alarm-micro.h"
#include "openthread/platform/alarm-milli.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/radio.h"
#include "openthread/platform/time.h"

static uint32_t s_alarm_ms = 0;
static bool s_is_ms_running = false;
static uint32_t s_alarm_us = 0;
static bool s_is_us_running = false;
static const char *alarm_workflow = "alarm";

static inline bool is_expired(uint32_t target, uint32_t now)
{
    return (((now - target) & (1 << 31)) == 0);
}

static inline uint32_t calculate_duration(uint32_t target, uint32_t now)
{
    return is_expired(target, now) ? 0 : target - now;
}

uint64_t otPlatTimeGet(void)
{
    return (uint64_t)esp_timer_get_time();
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_alarm_ms = aT0 + aDt;
    s_is_ms_running = true;

    ESP_LOGD(OT_PLAT_LOG_TAG, "Millisecond timer alarm start running, t0=%"PRIu32", dt=%"PRIu32"", aT0, aDt);
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_is_ms_running = false;
}

uint32_t inline otPlatAlarmMilliGetNow(void)
{
    return otPlatTimeGet() / US_PER_MS;
}

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_alarm_us = aT0 + aDt;
    s_is_us_running = true;

    ESP_LOGD(OT_PLAT_LOG_TAG, "Microsecond timer alarm start running, t0=%"PRIu32", dt=%"PRIu32"", aT0, aDt);
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    s_is_us_running = false;
}

uint32_t inline otPlatAlarmMicroGetNow(void)
{
    return otPlatTimeGet();
}

esp_err_t esp_openthread_alarm_init(void)
{
    return esp_openthread_platform_workflow_register(&esp_openthread_alarm_update, &esp_openthread_alarm_process,
                                                     alarm_workflow);
}

void esp_openthread_alarm_deinit(void)
{
    esp_openthread_platform_workflow_unregister(alarm_workflow);
}

void esp_openthread_alarm_update(esp_openthread_mainloop_context_t *mainloop)
{
    struct timeval *timeout = &mainloop->timeout;
    int32_t remain_min_time_us = INT32_MAX;
    int32_t remaining_us = 0;
    if (s_is_ms_running) {
        remaining_us = calculate_duration(s_alarm_ms, otPlatAlarmMilliGetNow()) * US_PER_MS;
        if (remain_min_time_us > remaining_us) {
            remain_min_time_us = remaining_us;
        }
    }
    if (s_is_us_running) {
        remaining_us = calculate_duration(s_alarm_us, otPlatAlarmMicroGetNow());
        if (remain_min_time_us > remaining_us) {
            remain_min_time_us = remaining_us;
        }
    }
    if (remain_min_time_us > 0) {
        timeout->tv_sec = remain_min_time_us / US_PER_S;
        timeout->tv_usec = remain_min_time_us % US_PER_S;
    } else {
        timeout->tv_sec = 0;
        timeout->tv_usec = 0;
    }
}

esp_err_t esp_openthread_alarm_process(otInstance *aInstance, const esp_openthread_mainloop_context_t *mainloop)
{
    if (s_is_ms_running && is_expired(s_alarm_ms, otPlatAlarmMilliGetNow())) {
        s_is_ms_running = false;

#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet()) {
            otPlatDiagAlarmFired(aInstance);
        } else
#endif
        {
            otPlatAlarmMilliFired(aInstance);
        }

        ESP_LOGD(OT_PLAT_LOG_TAG, "Millisecond timer alarm fired");
    }
    if (s_is_us_running && is_expired(s_alarm_us, otPlatAlarmMicroGetNow())) {
        s_is_us_running = false;
        otPlatAlarmMicroFired(aInstance);
        ESP_LOGD(OT_PLAT_LOG_TAG, "Microsecond timer alarm fired");
    }
    return ESP_OK;
}
