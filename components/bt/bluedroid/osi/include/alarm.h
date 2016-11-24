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

#ifndef _ALARM_H_
#define _ALARM_H_

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

typedef uint32_t period_ms_t;
typedef void (*osi_alarm_callback_t)(void *data);

#define ALARM_CBS_NUM   20
#define ALARM_ID_BASE   1000
typedef struct alarm_t {
    /* timer id point to here */
    TimerHandle_t alarm_hdl;
    osi_alarm_callback_t cb;
    void *cb_data;
} osi_alarm_t;

void osi_alarm_init(void);

// Creates a new alarm object. The returned object must be freed by calling
// |alarm_free|. Returns NULL on failure.
osi_alarm_t *osi_alarm_new(char *alarm_name, osi_alarm_callback_t callback, void *data, period_ms_t timer_expire);

// Frees an alarm object created by |alarm_new|. |alarm| may be NULL. If the
// alarm is pending, it will be cancelled. It is not safe to call |alarm_free|
// from inside the callback of |alarm|.
int osi_alarm_free(osi_alarm_t *alarm);

// Sets an alarm to fire |cb| after the given |deadline|. Note that |deadline| is the
// number of milliseconds relative to the current time. |data| is a context variable
// for the callback and may be NULL. |cb| will be called back in the context of an
// unspecified thread (i.e. it will not be called back in the same thread as the caller).
// |alarm| and |cb| may not be NULL.
int osi_alarm_set(osi_alarm_t *alarm, period_ms_t timeout);

// This function cancels the |alarm| if it was previously set. When this call
// returns, the caller has a guarantee that the callback is not in progress and
// will not be called if it hasn't already been called. This function is idempotent.
// |alarm| may not be NULL.
int osi_alarm_cancel(osi_alarm_t *alarm);

period_ms_t osi_alarm_now(void);

// Figure out how much time until next expiration.
// Returns 0 if not armed. |alarm| may not be NULL.
// TODO: Remove this function once PM timers can be re-factored
period_ms_t osi_alarm_get_remaining_ms(const osi_alarm_t *alarm);

// Alarm-related state cleanup
//void alarm_cleanup(void);

// Compute time difference (t1-t2) considering tick counter wrap
// t1 and t2 should be no greater than the time of MAX ticks
period_ms_t osi_alarm_time_diff(period_ms_t t1, period_ms_t t2);

#endif /*_ALARM_H_*/
