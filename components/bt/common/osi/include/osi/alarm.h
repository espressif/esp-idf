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
#include <stdbool.h>
#include "esp_timer.h"

typedef struct alarm_t osi_alarm_t;
typedef uint64_t period_ms_t;
typedef esp_timer_cb_t osi_alarm_callback_t;

typedef enum {
    OSI_ALARM_ERR_PASS = 0,
    OSI_ALARM_ERR_FAIL = -1,
    OSI_ALARM_ERR_INVALID_ARG = -2,
    OSI_ALARM_ERR_INVALID_STATE = -3,
} osi_alarm_err_t;

#define ALARM_CBS_NUM   50
#define ALARM_ID_BASE   1000

int osi_alarm_create_mux(void);
int osi_alarm_delete_mux(void);
void osi_alarm_init(void);
void osi_alarm_deinit(void);

// Creates a new alarm object. The returned object must be freed by calling
// |alarm_free|. Returns NULL on failure.
osi_alarm_t *osi_alarm_new(const char *alarm_name, osi_alarm_callback_t callback, void *data, period_ms_t timer_expire);

// Frees an alarm object created by |alarm_new|. |alarm| may be NULL. If the
// alarm is pending, it will be cancelled. It is not safe to call |alarm_free|
// from inside the callback of |alarm|.
void osi_alarm_free(osi_alarm_t *alarm);

// Sets an alarm to fire |cb| after the given |deadline|. Note that |deadline| is the
// number of milliseconds relative to the current time. |data| is a context variable
// for the callback and may be NULL. |cb| will be called back in the context of an
// unspecified thread (i.e. it will not be called back in the same thread as the caller).
// |alarm| and |cb| may not be NULL.
osi_alarm_err_t osi_alarm_set(osi_alarm_t *alarm, period_ms_t timeout);

// Sets an periodic alarm to fire |cb| each given |period|.
osi_alarm_err_t osi_alarm_set_periodic(osi_alarm_t *alarm, period_ms_t period);

// This function cancels the |alarm| if it was previously set. When this call
// returns, the caller has a guarantee that the callback is not in progress and
// will not be called if it hasn't already been called. This function is idempotent.
// |alarm| may not be NULL.
osi_alarm_err_t osi_alarm_cancel(osi_alarm_t *alarm);

// Figure out how much time until next expiration.
// Returns 0 if not armed. |alarm| may not be NULL.
// only for oneshot alarm, not for periodic alarm
// TODO: Remove this function once PM timers can be re-factored
period_ms_t osi_alarm_get_remaining_ms(const osi_alarm_t *alarm);

// Alarm-related state cleanup
//void alarm_cleanup(void);

uint32_t osi_time_get_os_boottime_ms(void);

// This function returns whether the alarm which encapsulated 
// a one-shot timer is active or not
bool osi_alarm_is_active(osi_alarm_t *alarm);

#endif /*_ALARM_H_*/
