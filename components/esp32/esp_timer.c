// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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

#include <sys/param.h>
#include "esp_types.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_task.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "sdkconfig.h"

#include "esp_timer_impl.h"

#ifdef CONFIG_ESP_TIMER_PROFILING
#define WITH_PROFILING 1
#endif

#ifndef NDEBUG
// Enable built-in checks in queue.h in debug builds
#define INVARIANTS
#endif
#include "rom/queue.h"

#define TIMER_EVENT_QUEUE_SIZE      16

struct esp_timer {
    uint64_t alarm;
    uint64_t period;
    esp_timer_cb_t callback;
    void* arg;
#if WITH_PROFILING
    const char* name;
    size_t times_triggered;
    size_t times_armed;
    uint64_t total_callback_run_time;
#endif // WITH_PROFILING
    LIST_ENTRY(esp_timer) list_entry;
};

static bool is_initialized();
static esp_err_t timer_insert(esp_timer_handle_t timer);
static esp_err_t timer_remove(esp_timer_handle_t timer);
static bool timer_armed(esp_timer_handle_t timer);
static void timer_list_lock();
static void timer_list_unlock();

#if WITH_PROFILING
static void timer_insert_inactive(esp_timer_handle_t timer);
static void timer_remove_inactive(esp_timer_handle_t timer);
#endif // WITH_PROFILING

static const char* TAG = "esp_timer";

// list of currently armed timers
static LIST_HEAD(esp_timer_list, esp_timer) s_timers =
        LIST_HEAD_INITIALIZER(s_timers);
#if WITH_PROFILING
// list of unarmed timers, used only to be able to dump statistics about
// all the timers
static LIST_HEAD(esp_inactive_timer_list, esp_timer) s_inactive_timers =
        LIST_HEAD_INITIALIZER(s_timers);
#endif
// task used to dispatch timer callbacks
static TaskHandle_t s_timer_task;
// counting semaphore used to notify the timer task from ISR
static SemaphoreHandle_t s_timer_semaphore;
// lock protecting s_timers and s_inactive_timers
static portMUX_TYPE s_timer_lock = portMUX_INITIALIZER_UNLOCKED;



esp_err_t esp_timer_create(const esp_timer_create_args_t* args,
                           esp_timer_handle_t* out_handle)
{
    if (!is_initialized()) {
        return ESP_ERR_INVALID_STATE;
    }
    if (args->callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_timer_handle_t result = (esp_timer_handle_t) calloc(1, sizeof(*result));
    if (result == NULL) {
        return ESP_ERR_NO_MEM;
    }
    result->callback = args->callback;
    result->arg = args->arg;
#if WITH_PROFILING
    result->name = args->name;
    timer_insert_inactive(result);
#endif
    *out_handle = result;
    return ESP_OK;
}

esp_err_t esp_timer_start_once(esp_timer_handle_t timer, uint64_t timeout_us)
{
    if (!is_initialized() || timer_armed(timer)) {
        return ESP_ERR_INVALID_STATE;
    }
    timer->alarm = esp_timer_get_time() + timeout_us;
    timer->period = 0;
#if WITH_PROFILING
    timer->times_armed++;
#endif
    return timer_insert(timer);
}

esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period_us)
{
    if (!is_initialized() || timer_armed(timer)) {
        return ESP_ERR_INVALID_STATE;
    }
    period_us = MAX(period_us, esp_timer_impl_get_min_period_us());
    timer->alarm = esp_timer_get_time() + period_us;
    timer->period = period_us;
#if WITH_PROFILING
    timer->times_armed++;
#endif
    return timer_insert(timer);
}

esp_err_t esp_timer_stop(esp_timer_handle_t timer)
{
    if (!is_initialized() || !timer_armed(timer)) {
        return ESP_ERR_INVALID_STATE;
    }
    return timer_remove(timer);
}

esp_err_t esp_timer_delete(esp_timer_handle_t timer)
{
    if (timer_armed(timer)) {
        return ESP_ERR_INVALID_STATE;
    }
#if WITH_PROFILING
    timer_remove_inactive(timer);
#endif
    if (timer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    free(timer);
    return ESP_OK;
}

static esp_err_t timer_insert(esp_timer_handle_t timer)
{
    timer_list_lock();
#if WITH_PROFILING
    timer_remove_inactive(timer);
#endif
    esp_timer_handle_t it, last = NULL;
    if (LIST_FIRST(&s_timers) == NULL) {
        LIST_INSERT_HEAD(&s_timers, timer, list_entry);
    } else {
        LIST_FOREACH(it, &s_timers, list_entry) {
            if (timer->alarm < it->alarm) {
                LIST_INSERT_BEFORE(it, timer, list_entry);
                break;
            }
            last = it;
        }
        if (it == NULL) {
            assert(last);
            LIST_INSERT_AFTER(last, timer, list_entry);
        }
    }
    if (timer == LIST_FIRST(&s_timers)) {
        esp_timer_impl_set_alarm(timer->alarm);
    }
    timer_list_unlock();
    return ESP_OK;
}

static esp_err_t timer_remove(esp_timer_handle_t timer)
{
    timer_list_lock();
    LIST_REMOVE(timer, list_entry);
    timer->alarm = 0;
    timer->period = 0;
#if WITH_PROFILING
    timer_insert_inactive(timer);
#endif
    timer_list_unlock();
    return ESP_OK;
}

#if WITH_PROFILING

static void timer_insert_inactive(esp_timer_handle_t timer)
{
    /* May be locked or not, depending on where this is called from.
     * Lock recursively.
     */
    timer_list_lock();
    esp_timer_handle_t head = LIST_FIRST(&s_inactive_timers);
    if (head == NULL) {
        LIST_INSERT_HEAD(&s_inactive_timers, timer, list_entry);
    } else {
        /* Insert as head element as this is the fastest thing to do.
         * Removal is O(1) anyway.
         */
        LIST_INSERT_BEFORE(head, timer, list_entry);
    }
    timer_list_unlock();
}

static void timer_remove_inactive(esp_timer_handle_t timer)
{
    timer_list_lock();
    LIST_REMOVE(timer, list_entry);
    timer_list_unlock();
}

#endif // WITH_PROFILING

static bool timer_armed(esp_timer_handle_t timer)
{
    return timer->alarm > 0;
}

static void timer_list_lock()
{
    portENTER_CRITICAL(&s_timer_lock);
}

static void timer_list_unlock()
{
    portEXIT_CRITICAL(&s_timer_lock);
}

static void timer_process_alarm(esp_timer_dispatch_t dispatch_method)
{
    /* unused, provision to allow running callbacks from ISR */
    (void) dispatch_method;

    timer_list_lock();
    uint64_t now = esp_timer_impl_get_time();
    esp_timer_handle_t it = LIST_FIRST(&s_timers);
    while (it != NULL &&
            it->alarm < now) {
        LIST_REMOVE(it, list_entry);
        if (it->period > 0) {
            it->alarm += it->period;
            timer_insert(it);
        } else {
            it->alarm = 0;
#if WITH_PROFILING
            timer_insert_inactive(it);
#endif
        }
#if WITH_PROFILING
        uint64_t callback_start = now;
#endif
        timer_list_unlock();
        (*it->callback)(it->arg);
        timer_list_lock();
        now = esp_timer_impl_get_time();
#if WITH_PROFILING
        it->times_triggered++;
        it->total_callback_run_time += now - callback_start;
#endif
        it = LIST_FIRST(&s_timers);
    }
    esp_timer_handle_t first = LIST_FIRST(&s_timers);
    if (first) {
        esp_timer_impl_set_alarm(first->alarm);
    }
    timer_list_unlock();
}

static void timer_task(void* arg)
{
    while (true){
        int res = xSemaphoreTake(s_timer_semaphore, portMAX_DELAY);
        assert(res == pdTRUE);
        timer_process_alarm(ESP_TIMER_TASK);
    }
}

static void IRAM_ATTR timer_alarm_handler(void* arg)
{
    int need_yield;
    if (xSemaphoreGiveFromISR(s_timer_semaphore, &need_yield) != pdPASS) {
        ESP_EARLY_LOGD(TAG, "timer queue overflow");
        return;
    }
    if (need_yield == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static bool is_initialized()
{
    return s_timer_task != NULL;
}


esp_err_t esp_timer_init(void)
{
    if (is_initialized()) {
        return ESP_ERR_INVALID_STATE;
    }

    s_timer_semaphore = xSemaphoreCreateCounting(TIMER_EVENT_QUEUE_SIZE, 0);
    if (!s_timer_semaphore) {
        return ESP_ERR_NO_MEM;
    }

    int ret = xTaskCreatePinnedToCore(&timer_task, "esp_timer",
            ESP_TASK_TIMER_STACK, NULL, ESP_TASK_TIMER_PRIO, &s_timer_task, PRO_CPU_NUM);
    if (ret != pdPASS) {
        vSemaphoreDelete(s_timer_semaphore);
        s_timer_semaphore = NULL;
        return ESP_ERR_NO_MEM;
    }

    esp_err_t err = esp_timer_impl_init(&timer_alarm_handler);
    if (err != ESP_OK) {
        vTaskDelete(s_timer_task);
        s_timer_task = NULL;
        vSemaphoreDelete(s_timer_semaphore);
        s_timer_semaphore = NULL;
        return err;
    }

    return ESP_OK;
}

esp_err_t esp_timer_deinit(void)
{
    if (!is_initialized()) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Check if there are any active timers */
    if (!LIST_EMPTY(&s_timers)) {
        return ESP_ERR_INVALID_STATE;
    }

    /* We can only check if there are any timers which are not deleted if
     * profiling is enabled.
     */
#if WITH_PROFILING
    if (!LIST_EMPTY(&s_inactive_timers)) {
        return ESP_ERR_INVALID_STATE;
    }
#endif

    esp_timer_impl_deinit();

    vTaskDelete(s_timer_task);
    s_timer_task = NULL;
    vSemaphoreDelete(s_timer_semaphore);
    s_timer_semaphore = NULL;
    return ESP_OK;
}

static void print_timer_info(esp_timer_handle_t t, char** dst, size_t* dst_size)
{
    size_t cb = snprintf(*dst, *dst_size,
#if WITH_PROFILING
            "%-12s  %12lld  %12lld  %9d  %9d  %12lld\n",
            t->name, t->period, t->alarm,
            t->times_armed, t->times_triggered, t->total_callback_run_time);
    /* keep this in sync with the format string, used in esp_timer_dump */
#define TIMER_INFO_LINE_LEN 78
#else
            "timer@%p  %12lld  %12lld\n", t, t->period, t->alarm);
#define TIMER_INFO_LINE_LEN 46
#endif
    *dst += cb;
    *dst_size -= cb;
}


esp_err_t esp_timer_dump(FILE* stream)
{
    /* Since timer lock is a critical section, we don't want to print directly
     * to stdout, since that may cause a deadlock if stdout is interrupt-driven
     * (via the UART driver). Allocate sufficiently large chunk of memory first,
     * print to it, then dump this memory to stdout.
     */

    esp_timer_handle_t it;

    /* First count the number of timers */
    size_t timer_count = 0;
    timer_list_lock();
    LIST_FOREACH(it, &s_timers, list_entry) {
        ++timer_count;
    }
#if WITH_PROFILING
    LIST_FOREACH(it, &s_inactive_timers, list_entry) {
        ++timer_count;
    }
#endif
    timer_list_unlock();

    /* Allocate the memory for this number of timers. Since we have unlocked,
     * we may find that there are more timers. There's no bulletproof solution
     * for this (can't allocate from a critical section), but we allocate
     * slightly more and the output will be truncated if that is not enough.
     */
    size_t buf_size = TIMER_INFO_LINE_LEN * (timer_count + 3);
    char* print_buf = calloc(1, buf_size + 1);
    if (print_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }

    /* Print to the buffer */
    timer_list_lock();
    char* pos = print_buf;
    LIST_FOREACH(it, &s_timers, list_entry) {
        print_timer_info(it, &pos, &buf_size);
    }
#if WITH_PROFILING
    LIST_FOREACH(it, &s_inactive_timers, list_entry) {
        print_timer_info(it, &pos, &buf_size);
    }
#endif
    timer_list_unlock();

    /* Print the buffer */
    fputs(print_buf, stream);

    free(print_buf);
    return ESP_OK;
}

int64_t IRAM_ATTR esp_timer_get_time()
{
    return (int64_t) esp_timer_impl_get_time();
}
