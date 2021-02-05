// Copyright 2016-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include "esp_pm.h"
#include "esp_system.h"
#include "sys/queue.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/pm_impl.h"
#include "esp_timer.h"
#include "sdkconfig.h"


typedef struct esp_pm_lock {
    esp_pm_lock_type_t type;        /*!< type passed to esp_pm_lock_create */
    int arg;                        /*!< argument passed to esp_pm_lock_create */
    pm_mode_t mode;                 /*!< implementation-defined mode for this type of lock*/
    const char* name;               /*!< used to identify the lock */
    SLIST_ENTRY(esp_pm_lock) next;  /*!< linked list pointer */
    size_t count;                   /*!< lock count */
    portMUX_TYPE spinlock;          /*!< spinlock used when operating on 'count' */
#ifdef WITH_PROFILING
    pm_time_t last_taken;           /*!< time what the lock was taken (valid if count > 0) */
    pm_time_t time_held;            /*!< total time the lock was taken.
                                         If count > 0, this doesn't include the time since last_taken */
    size_t times_taken;             /*!< number of times the lock was ever taken */
#endif
} esp_pm_lock_t;


static const char* s_lock_type_names[] = {
        "CPU_FREQ_MAX",
        "APB_FREQ_MAX",
        "NO_LIGHT_SLEEP"
};

/* List of all existing locks, used for esp_pm_dump_locks */
static SLIST_HEAD(esp_pm_locks_head, esp_pm_lock) s_list =
        SLIST_HEAD_INITIALIZER(s_head);
/* Protects the above list */
static _lock_t s_list_lock;


esp_err_t esp_pm_lock_create(esp_pm_lock_type_t lock_type, int arg,
        const char* name, esp_pm_lock_handle_t* out_handle)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

    if (out_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_pm_lock_t* new_lock = (esp_pm_lock_t*) calloc(1, sizeof(*new_lock));
    if (!new_lock) {
        return ESP_ERR_NO_MEM;
    }
    new_lock->type = lock_type;
    new_lock->arg = arg;
    new_lock->mode = esp_pm_impl_get_mode(lock_type, arg);
    new_lock->name = name;
    new_lock->spinlock = (portMUX_TYPE) portMUX_INITIALIZER_UNLOCKED;
    *out_handle = new_lock;

    _lock_acquire(&s_list_lock);
    SLIST_INSERT_HEAD(&s_list, new_lock, next);
    _lock_release(&s_list_lock);
    return ESP_OK;
}

esp_err_t esp_pm_lock_delete(esp_pm_lock_handle_t handle)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (handle->count > 0) {
        return ESP_ERR_INVALID_STATE;
    }
    _lock_acquire(&s_list_lock);
    SLIST_REMOVE(&s_list, handle, esp_pm_lock, next);
    _lock_release(&s_list_lock);
    free(handle);
    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_pm_lock_acquire(esp_pm_lock_handle_t handle)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&handle->spinlock);
    if (handle->count++ == 0) {
        pm_time_t now = 0;
#ifdef WITH_PROFILING
        now = pm_get_time();
#endif
        esp_pm_impl_switch_mode(handle->mode, MODE_LOCK, now);
#ifdef WITH_PROFILING
        handle->last_taken = now;
        handle->times_taken++;
#endif
    }
    portEXIT_CRITICAL_SAFE(&handle->spinlock);
    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_pm_lock_release(esp_pm_lock_handle_t handle)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    portENTER_CRITICAL_SAFE(&handle->spinlock);
    if (handle->count == 0) {
        ret = ESP_ERR_INVALID_STATE;
        goto out;
    }
    if (--handle->count == 0) {
        pm_time_t now = 0;
#ifdef WITH_PROFILING
        now = pm_get_time();
        handle->time_held += now - handle->last_taken;
#endif
        esp_pm_impl_switch_mode(handle->mode, MODE_UNLOCK, now);
    }
out:
    portEXIT_CRITICAL_SAFE(&handle->spinlock);
    return ret;
}

esp_err_t esp_pm_dump_locks(FILE* stream)
{
#ifndef CONFIG_PM_ENABLE
    return ESP_ERR_NOT_SUPPORTED;
#endif

#ifdef WITH_PROFILING
    pm_time_t cur_time = pm_get_time();
    pm_time_t cur_time_d100 = cur_time / 100;
#endif // WITH_PROFILING

    _lock_acquire(&s_list_lock);
#ifdef WITH_PROFILING
    fprintf(stream, "Time: %lld\n", cur_time);
#endif

    fprintf(stream, "Lock stats:\n");
    esp_pm_lock_t* it;
    char line[80];
    SLIST_FOREACH(it, &s_list, next) {
        char *buf = line;
        size_t len = sizeof(line);
        size_t cb;

        portENTER_CRITICAL(&it->spinlock);
        if (it->name == NULL) {
            cb = snprintf(buf, len, "lock@%p ", it);
        } else {
            cb = snprintf(buf, len, "%-15.15s ", it->name);
        }
        assert(cb <= len); // above formats should fit into sizeof(line)
        buf += cb;
        len -= cb;
#ifdef WITH_PROFILING
        pm_time_t time_held = it->time_held;
        if (it->count > 0) {
            time_held += cur_time - it->last_taken;
        }
        snprintf(buf, len, "%14s  %3d  %3d  %9d  %12lld  %3lld%%\n",
                s_lock_type_names[it->type], it->arg,
                it->count, it->times_taken, time_held,
                (time_held + cur_time_d100 - 1) / cur_time_d100);
#else
        snprintf(buf, len, "%14s  %3d  %3d\n", s_lock_type_names[it->type], it->arg, it->count);
#endif // WITH_PROFILING
        portEXIT_CRITICAL(&it->spinlock);
        fputs(line, stream);
    }
    _lock_release(&s_list_lock);
#ifdef WITH_PROFILING
    esp_pm_impl_dump_stats(stream);
#endif
    return ESP_OK;
}
