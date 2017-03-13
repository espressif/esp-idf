/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bt_defs.h"
#include "bt_trace.h"
#include "alarm.h"
#include "allocator.h"
#include "list.h"
#include "thread.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/xtensa_api.h"
#include "rom/ets_sys.h"

#define RTC_TIMER_TICKS_TO_MS(ticks)            (((ticks/625)<<1) + (ticks-(ticks/625)*625)/312)


#define BT_ALARM_START_WAIT_TICKS   100
#define BT_ALARM_STOP_WAIT_TICKS    100
#define BT_ALARM_FREE_WAIT_TICKS    100
#define BT_ALARM_CHG_PERIOD_WAIT_TICKS  100

static struct alarm_t alarm_cbs[ALARM_CBS_NUM];

void osi_alarm_init(void)
{
    memset(&alarm_cbs[0], 0x00, sizeof(alarm_cbs));
}

static struct alarm_t *alarm_cbs_lookfor_available(void)
{
    int i;

    for (i = 0; i < ALARM_CBS_NUM; i++) {
        if (alarm_cbs[i].alarm_hdl == NULL) { //available
            LOG_DEBUG("%s %d %p\n", __func__, i, &alarm_cbs[i]);
            return &alarm_cbs[i];
        }
    }

    return NULL;
}

static void alarm_cb_handler(TimerHandle_t xTimer)
{
    struct alarm_t *alarm;

    if (!xTimer) {
        LOG_ERROR("TimerName: NULL\n");
        return;
    }

    alarm = pvTimerGetTimerID(xTimer);
    LOG_DEBUG("TimerID %p, Name %s\n", alarm, pcTimerGetTimerName(xTimer));
    if (alarm->cb) {
        alarm->cb(alarm->cb_data);
    }
}

osi_alarm_t *osi_alarm_new(char *alarm_name, osi_alarm_callback_t callback, void *data, period_ms_t timer_expire)
{
    struct alarm_t *timer_id;
    TimerHandle_t t;

    if (timer_expire == 0) {
        timer_expire = 1000;
    }

    /* TODO mutex lock */
    timer_id = alarm_cbs_lookfor_available();
    if (!timer_id) {
        LOG_ERROR("%s full\n", __func__);
        return NULL;
    }

    t = xTimerCreate(alarm_name, timer_expire / portTICK_PERIOD_MS, pdFALSE, timer_id, alarm_cb_handler);
    if (!t) {
        LOG_ERROR("%s error\n", __func__);
        return NULL;
    }

    timer_id->alarm_hdl = t;
    timer_id->cb = callback;
    timer_id->cb_data = data;
    /* TODO mutex unlock */

    return timer_id;
}

int osi_alarm_free(osi_alarm_t *alarm)
{
    if (!alarm) {
        LOG_ERROR("%s null\n", __func__);
        return -1;
    }

    if (xTimerDelete(alarm->alarm_hdl, BT_ALARM_FREE_WAIT_TICKS) != pdPASS) {
        LOG_ERROR("%s error\n", __func__);
        return -2;
    }

    /* TODO mutex lock */
    memset(alarm, 0x00, sizeof(osi_alarm_t));
    /* TODO mutex unlock */

    return 0;
}


int osi_alarm_set(osi_alarm_t *alarm, period_ms_t timeout)
{
    if (!alarm) {
        LOG_ERROR("%s null\n", __func__);
        return -1;
    }

    if (xTimerChangePeriod(alarm->alarm_hdl, timeout / portTICK_PERIOD_MS, BT_ALARM_CHG_PERIOD_WAIT_TICKS) != pdPASS) {
        LOG_ERROR("%s chg period error\n", __func__);
        return -2;
    }

    if (xTimerStart(alarm->alarm_hdl, BT_ALARM_START_WAIT_TICKS) != pdPASS) {
        LOG_ERROR("%s start error\n", __func__);
        return -3;
    }

    return 0;
}


int osi_alarm_cancel(osi_alarm_t *alarm)
{
    if (!alarm) {
        LOG_ERROR("%s null\n", __func__);
        return -1;
    }

    if (xTimerStop(alarm->alarm_hdl, BT_ALARM_STOP_WAIT_TICKS) != pdPASS) {
        LOG_ERROR("%s error\n", __func__);
        return -2;
    }

    return 0;
}

static uint32_t alarm_current_tick(void)
{
    return xTaskGetTickCount();
}

// todo: this is not accurate
// max return value: 0xffffffff / 312 = 13765920 = 0xD20D20
period_ms_t osi_alarm_now(void)
{
    return RTC_TIMER_TICKS_TO_MS((alarm_current_tick()));
}

period_ms_t osi_alarm_get_remaining_ms(const osi_alarm_t *alarm)
{
    /* TODO: use FreeRTOS timer.c implement ??? */
    return 0xffffffff;
}

// pre-condition: 0 <= t1, t2 <= 0xD20D20
// return value: 0<= ret <=0XD20D20
period_ms_t osi_alarm_time_diff(period_ms_t t1, period_ms_t t2)
{
#define MAX_ALARM_TIME_MS     (0xD20D20)
    int32_t diff = (int32_t)(t1) - (int32_t)(t2);
    if (diff < 0) {
        diff += MAX_ALARM_TIME_MS;
    }
    return (period_ms_t)diff;
}
