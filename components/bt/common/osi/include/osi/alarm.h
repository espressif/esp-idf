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
#include "esp_timer.h"
#include "bt_user_config.h"
#include "esp_assert.h"
#include "osi/thread.h"

typedef struct alarm_t osi_alarm_t;
typedef uint64_t period_ms_t;
typedef esp_timer_cb_t osi_alarm_callback_t;

typedef enum {
    OSI_ALARM_ERR_PASS = 0,
    OSI_ALARM_ERR_FAIL = -1,
    OSI_ALARM_ERR_INVALID_ARG = -2,
    OSI_ALARM_ERR_INVALID_STATE = -3,
} osi_alarm_err_t;

#define ALARM_CBS_NUM   UC_ALARM_MAX_NUM
/* dispatch_id is uint32_t and skips 0, so a full pool still needs one free id. */
ESP_STATIC_ASSERT(ALARM_CBS_NUM < UINT32_MAX, "BT_ALARM_MAX_NUM leaves no free dispatch id");
#define ALARM_ID_BASE   1000

int osi_alarm_create_mux(void);
int osi_alarm_delete_mux(void);
void osi_alarm_init(void);
void osi_alarm_deinit(void);

// Creates a new alarm object. The returned object must be freed by calling
// |alarm_free|. Returns NULL on failure. |post_task| selects the thread that
// runs |callback|; NULL posts to the BTC task. Creation fails when that
// thread does not exist yet. A NULL slot post_task after creation means that
// thread has been detached and the callback is dropped.
osi_alarm_t *osi_alarm_new(const char *alarm_name, osi_alarm_callback_t callback, void *data, osi_thread_t *post_task);

// Drop every alarm whose post target is |thread|. The timer stays allocated
// until osi_alarm_free(). Safe to call when the alarm subsystem is closed.
void osi_alarm_detach_post_task(osi_thread_t *thread);

// Frees an alarm object created by |alarm_new|. |alarm| may be NULL. Pending
// timer ticks and callbacks that have not started are dropped. A free while a
// callback still holds the object takes ownership and releases it when that
// callback drops its ref, so a timeout handler may free its own alarm. The
// caller must not use |alarm| after OSI_ALARM_ERR_PASS. Returns
// OSI_ALARM_ERR_FAIL when the underlying timer is still deleting; the object
// stays allocated. Existing callers discard the return value, so this path
// does not guarantee the slot is reclaimed.
osi_alarm_err_t osi_alarm_free(osi_alarm_t *alarm);

// hash_map data_free_fn adapter. The free result is ignored.
static inline void osi_alarm_free_cb(void *alarm)
{
    (void)osi_alarm_free((osi_alarm_t *)alarm);
}

// Sets an alarm to fire |cb| after the given |deadline|. Note that |deadline| is the
// number of milliseconds relative to the current time. |data| is a context variable
// for the callback and may be NULL. |cb| will be called back in the context of an
// unspecified thread (i.e. it will not be called back in the same thread as the caller).
// An armed alarm, including a post-failure retry, is stopped first. A callback that
// was queued but had not started is dropped. Fails when a cancel already owns |alarm|.
// |alarm| and |cb| may not be NULL.
osi_alarm_err_t osi_alarm_set(osi_alarm_t *alarm, period_ms_t timeout);

// Sets an periodic alarm to fire |cb| each given |period|. Replaces an armed alarm
// the same way as osi_alarm_set().
osi_alarm_err_t osi_alarm_set_periodic(osi_alarm_t *alarm, period_ms_t period);

// Stops |alarm|. When this returns, an old timer tick will not post or re-arm,
// and a callback that was queued or dequeued but had not started is dropped.
// Returns OSI_ALARM_ERR_FAIL only when the user callback had already started;
// that callback still runs, and later ticks stay stopped. Idempotent.
// |alarm| may not be NULL.
osi_alarm_err_t osi_alarm_cancel(osi_alarm_t *alarm);

// Stop |alarm| only while its esp_timer is active. NULL |sooner_than_ms| stops
// any active timer, including a periodic alarm. Otherwise stop only when the
// remaining time is greater than |*sooner_than_ms|; a sooner timer is left armed.
// A periodic alarm cannot provide a remaining time, so a non-NULL |sooner_than_ms|
// returns FAIL and leaves it armed.
// PASS: it was active and is now stopped.
// FAIL: the callback had already started, the armed timer is sooner, or |alarm|
// is periodic with a non-NULL |sooner_than_ms|.
// INVALID_STATE: the timer was not active, so nothing was stopped.
osi_alarm_err_t osi_alarm_cancel_active(osi_alarm_t *alarm, const period_ms_t *sooner_than_ms);

// Overwrites the phase a periodic alarm resumes on after a failed post.
// |deadline_us| is an esp_timer timestamp; 0 clears it. A one-shot ignores
// the write: that word is its retry-grade baseline. |alarm| may not be NULL.
osi_alarm_err_t osi_alarm_set_deadline_us(osi_alarm_t *alarm, uint64_t deadline_us);

// Time until the next one-shot expiration, in milliseconds.
// A periodic alarm returns 0, including while a failed post retries it
// with a one-shot. Returns 0 if not armed. |alarm| may not be NULL.
// TODO: Remove this function once PM timers can be re-factored
period_ms_t osi_alarm_get_remaining_ms(const osi_alarm_t *alarm);

// Alarm-related state cleanup
//void alarm_cleanup(void);

uint32_t osi_time_get_os_boottime_ms(void);

// This function returns whether the given |alarm| is active or not.
// Return true if active, false otherwise.
bool osi_alarm_is_active(osi_alarm_t *alarm);

#endif /*_ALARM_H_*/
