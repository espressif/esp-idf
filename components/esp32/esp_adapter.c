// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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
// limitations under the License.

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/xtensa_api.h"
#include "freertos/portmacro.h"
#include "freertos/xtensa_api.h"
#include "esp_types.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_private/wifi_os_adapter.h"
#include "esp_private/wifi.h"
#include "esp_phy_init.h"
#include "driver/periph_ctrl.h"
#include "nvs.h"
#include "os.h"
#include "esp_smartconfig.h"
#include "esp_coexist_internal.h"
#include "esp_coexist_adapter.h"


extern void esp_dport_access_stall_other_cpu_start_wrap(void);
extern void esp_dport_access_stall_other_cpu_end_wrap(void);

#define TAG "esp_adapter"

/*
 If CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP is enabled. Prefer to allocate a chunk of memory in SPIRAM firstly.
 If failed, try to allocate it in internal memory then.
 */
IRAM_ATTR void *wifi_malloc( size_t size )
{
#if CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP
    return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return malloc(size);
#endif
}

/*
 If CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP is enabled. Prefer to allocate a chunk of memory in SPIRAM firstly.
 If failed, try to allocate it in internal memory then.
 */
IRAM_ATTR void *wifi_realloc( void *ptr, size_t size )
{
#if CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP
    return heap_caps_realloc_prefer(ptr, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return realloc(ptr, size);
#endif
}

/*
 If CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP is enabled. Prefer to allocate a chunk of memory in SPIRAM firstly.
 If failed, try to allocate it in internal memory then.
 */
IRAM_ATTR void *wifi_calloc( size_t n, size_t size )
{
#if CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP
    return heap_caps_calloc_prefer(n, size, 2, MALLOC_CAP_DEFAULT|MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
#else
    return calloc(n, size);
#endif
}

static void * IRAM_ATTR wifi_zalloc_wrapper(size_t size)
{
    void *ptr = wifi_calloc(1, size);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

wifi_static_queue_t* wifi_create_queue( int queue_len, int item_size)
{
    wifi_static_queue_t *queue = NULL;

    queue = (wifi_static_queue_t*)heap_caps_malloc(sizeof(wifi_static_queue_t), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue) {
        return NULL;
    }

#if CONFIG_SPIRAM_USE_MALLOC

    queue->storage = heap_caps_calloc(1, sizeof(StaticQueue_t) + (queue_len*item_size), MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (!queue->storage) {
        goto _error;
    }

    queue->handle = xQueueCreateStatic( queue_len, item_size, ((uint8_t*)(queue->storage)) + sizeof(StaticQueue_t), (StaticQueue_t*)(queue->storage));

    if (!queue->handle) {
        goto _error;
    }

    return queue;

_error:
    if (queue) {
        if (queue->storage) {
            free(queue->storage);
        }

        free(queue);
    }

    return NULL;
#else
    queue->handle = xQueueCreate( queue_len, item_size);
    return queue;
#endif
}

void wifi_delete_queue(wifi_static_queue_t *queue)
{
    if (queue) {
        vQueueDelete(queue->handle);

#if CONFIG_SPIRAM_USE_MALLOC
        if (queue->storage) {
            free(queue->storage);
        }
#endif

        free(queue);
    }
}

static void * wifi_create_queue_wrapper(int queue_len, int item_size)
{
    return wifi_create_queue(queue_len, item_size);
}

static void wifi_delete_queue_wrapper(void *queue)
{
    wifi_delete_queue(queue);
}

static void set_isr_wrapper(int32_t n, void *f, void *arg)
{
    xt_set_interrupt_handler(n, (xt_handler)f, arg);
}

static void * spin_lock_create_wrapper(void)
{
    portMUX_TYPE tmp = portMUX_INITIALIZER_UNLOCKED;
    void *mux = malloc(sizeof(portMUX_TYPE));

    if (mux) {
        memcpy(mux,&tmp,sizeof(portMUX_TYPE));
        return mux;
    }
    return NULL;
}

static uint32_t IRAM_ATTR wifi_int_disable_wrapper(void *wifi_int_mux)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(wifi_int_mux);
    } else {
        portENTER_CRITICAL(wifi_int_mux);
    }

    return 0;
}

static void IRAM_ATTR wifi_int_restore_wrapper(void *wifi_int_mux, uint32_t tmp)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(wifi_int_mux);
    } else {
        portEXIT_CRITICAL(wifi_int_mux);
    }
}

static void IRAM_ATTR task_yield_from_isr_wrapper(void)
{
    portYIELD_FROM_ISR();
}

static void * semphr_create_wrapper(uint32_t max, uint32_t init)
{
    return (void *)xSemaphoreCreateCounting(max, init);
}

static void semphr_delete_wrapper(void *semphr)
{
    vSemaphoreDelete(semphr);
}

static void wifi_thread_semphr_free(void* data)
{
    xSemaphoreHandle *sem = (xSemaphoreHandle*)(data);

    if (sem) {
        vSemaphoreDelete(sem);
    }
}

static void * wifi_thread_semphr_get_wrapper(void)
{
    static bool s_wifi_thread_sem_key_init = false;
    static pthread_key_t s_wifi_thread_sem_key;
    xSemaphoreHandle sem = NULL;

    if (s_wifi_thread_sem_key_init == false) {
        if (0 != pthread_key_create(&s_wifi_thread_sem_key, wifi_thread_semphr_free)) {
            return NULL;
        }
        s_wifi_thread_sem_key_init = true;
    }

    sem = pthread_getspecific(s_wifi_thread_sem_key);
    if (!sem) {
        sem = xSemaphoreCreateCounting(1, 0);
        if (sem) {
            pthread_setspecific(s_wifi_thread_sem_key, sem);
            ESP_LOGV(TAG, "thread sem create: sem=%p", sem);
        }
    }

    ESP_LOGV(TAG, "thread sem get: sem=%p", sem);
    return (void*)sem;
}

static int32_t IRAM_ATTR semphr_take_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreTakeFromISR(semphr, hptw);
}

static int32_t IRAM_ATTR semphr_give_from_isr_wrapper(void *semphr, void *hptw)
{
    return (int32_t)xSemaphoreGiveFromISR(semphr, hptw);
}

static int32_t semphr_take_wrapper(void *semphr, uint32_t block_time_tick)
{
    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xSemaphoreTake(semphr, portMAX_DELAY);
    } else {
        return (int32_t)xSemaphoreTake(semphr, block_time_tick);
    }
}

static int32_t semphr_give_wrapper(void *semphr)
{
    return (int32_t)xSemaphoreGive(semphr);
}

static void * recursive_mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateRecursiveMutex();
}

static void * mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateMutex();
}

static void mutex_delete_wrapper(void *mutex)
{
    vSemaphoreDelete(mutex);
}

static int32_t IRAM_ATTR mutex_lock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
}

static int32_t IRAM_ATTR mutex_unlock_wrapper(void *mutex)
{
    return (int32_t)xSemaphoreGiveRecursive(mutex);
}

static void * queue_create_wrapper(uint32_t queue_len, uint32_t item_size)
{
    return (void *)xQueueCreate(queue_len, item_size);
}

static int32_t queue_send_wrapper(void *queue, void *item, uint32_t block_time_tick)
{
    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xQueueSend(queue, item, portMAX_DELAY);
    } else {
        return (int32_t)xQueueSend(queue, item, block_time_tick);
    }
}

static int32_t IRAM_ATTR queue_send_from_isr_wrapper(void *queue, void *item, void *hptw)
{
    return (int32_t)xQueueSendFromISR(queue, item, hptw);
}

static int32_t queue_send_to_back_wrapper(void *queue, void *item, uint32_t block_time_tick)
{
    return (int32_t)xQueueGenericSend(queue, item, block_time_tick, queueSEND_TO_BACK);
}

static int32_t queue_send_to_front_wrapper(void *queue, void *item, uint32_t block_time_tick)
{
    return (int32_t)xQueueGenericSend(queue, item, block_time_tick, queueSEND_TO_FRONT);
}

static int32_t queue_recv_wrapper(void *queue, void *item, uint32_t block_time_tick)
{
    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)xQueueReceive(queue, item, portMAX_DELAY);
    } else {
        return (int32_t)xQueueReceive(queue, item, block_time_tick);
    }
}

static uint32_t event_group_wait_bits_wrapper(void *event, uint32_t bits_to_wait_for, int clear_on_exit, int wait_for_all_bits, uint32_t block_time_tick)
{
    if (block_time_tick == OSI_FUNCS_TIME_BLOCKING) {
        return (uint32_t)xEventGroupWaitBits(event, bits_to_wait_for, clear_on_exit, wait_for_all_bits, portMAX_DELAY);
    } else {
        return (uint32_t)xEventGroupWaitBits(event, bits_to_wait_for, clear_on_exit, wait_for_all_bits, block_time_tick);
    }
}

static int32_t task_create_pinned_to_core_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id)
{
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < portNUM_PROCESSORS ? core_id : tskNO_AFFINITY));
}

static int32_t task_create_wrapper(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle)
{
    return (uint32_t)xTaskCreate(task_func, name, stack_depth, param, prio, task_handle);
}

static int32_t IRAM_ATTR task_ms_to_tick_wrapper(uint32_t ms)
{
    return (int32_t)(ms / portTICK_PERIOD_MS);
}


static int32_t task_get_max_priority_wrapper(void)
{
    return (int32_t)(configMAX_PRIORITIES);
}

static int32_t esp_event_post_wrapper(const char* event_base, int32_t event_id, void* event_data, size_t event_data_size, uint32_t ticks_to_wait)
{
    if (ticks_to_wait == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)esp_event_post(event_base, event_id, event_data, event_data_size, portMAX_DELAY);
    } else {
        return (int32_t)esp_event_post(event_base, event_id, event_data, event_data_size, ticks_to_wait);
    }
}

static void IRAM_ATTR timer_arm_wrapper(void *timer, uint32_t tmout, bool repeat)
{
    ets_timer_arm(timer, tmout, repeat);
}

static void IRAM_ATTR timer_disarm_wrapper(void *timer)
{
    ets_timer_disarm(timer);
}

static void timer_done_wrapper(void *ptimer)
{
    ets_timer_done(ptimer);
}

static void timer_setfn_wrapper(void *ptimer, void *pfunction, void *parg)
{
    ets_timer_setfn(ptimer, pfunction, parg);
}

static void IRAM_ATTR timer_arm_us_wrapper(void *ptimer, uint32_t us, bool repeat)
{
    ets_timer_arm_us(ptimer, us, repeat);
}

static int get_time_wrapper(void *t)
{
    return os_get_time(t);
}

static void * IRAM_ATTR malloc_internal_wrapper(size_t size)
{
    return heap_caps_malloc(size, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
}

static void * IRAM_ATTR realloc_internal_wrapper(void *ptr, size_t size)
{
    return heap_caps_realloc(ptr, size, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
}

static void * IRAM_ATTR calloc_internal_wrapper(size_t n, size_t size)
{
    return heap_caps_calloc(n, size, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
}

static void * IRAM_ATTR zalloc_internal_wrapper(size_t size)
{
    void *ptr = heap_caps_calloc(1, size, MALLOC_CAP_DEFAULT|MALLOC_CAP_INTERNAL);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

static uint32_t coex_status_get_wrapper(void)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_status_get();
#else
    return 0;
#endif
}

static int coex_wifi_request_wrapper(uint32_t event, uint32_t latency, uint32_t duration)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_wifi_request(event, latency, duration);
#else
    return 0;
#endif
}

static int coex_wifi_release_wrapper(uint32_t event)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_wifi_release(event);
#else
    return 0;
#endif
}

int IRAM_ATTR coex_bt_request_wrapper(uint32_t event, uint32_t latency, uint32_t duration)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_bt_request(event, latency, duration);
#else
    return 0;
#endif
}

int IRAM_ATTR coex_bt_release_wrapper(uint32_t event)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_bt_release(event);
#else
    return 0;
#endif
}

int coex_register_bt_cb_wrapper(coex_func_cb_t cb)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_register_bt_cb(cb);
#else
    return 0;
#endif
}

uint32_t IRAM_ATTR coex_bb_reset_lock_wrapper(void)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    return coex_bb_reset_lock();
#else
    return 0;
#endif
}

void IRAM_ATTR coex_bb_reset_unlock_wrapper(uint32_t restore)
{
#if CONFIG_ESP32_WIFI_SW_COEXIST_ENABLE
    coex_bb_reset_unlock(restore);
#endif
}

wifi_osi_funcs_t g_wifi_osi_funcs = {
    ._version = ESP_WIFI_OS_ADAPTER_VERSION,
    ._set_isr = set_isr_wrapper,
    ._ints_on = xt_ints_on,
    ._ints_off = xt_ints_off,
    ._spin_lock_create = spin_lock_create_wrapper,
    ._spin_lock_delete = free,
    ._wifi_int_disable = wifi_int_disable_wrapper,
    ._wifi_int_restore = wifi_int_restore_wrapper,
    ._task_yield_from_isr = task_yield_from_isr_wrapper,
    ._semphr_create = semphr_create_wrapper,
    ._semphr_delete = semphr_delete_wrapper,
    ._semphr_take = semphr_take_wrapper,
    ._semphr_give = semphr_give_wrapper,
    ._wifi_thread_semphr_get = wifi_thread_semphr_get_wrapper,
    ._mutex_create = mutex_create_wrapper,
    ._recursive_mutex_create = recursive_mutex_create_wrapper,
    ._mutex_delete = mutex_delete_wrapper,
    ._mutex_lock = mutex_lock_wrapper,
    ._mutex_unlock = mutex_unlock_wrapper,
    ._queue_create = queue_create_wrapper,
    ._queue_delete = vQueueDelete,
    ._queue_send = queue_send_wrapper,
    ._queue_send_from_isr = queue_send_from_isr_wrapper,
    ._queue_send_to_back = queue_send_to_back_wrapper,
    ._queue_send_to_front = queue_send_to_front_wrapper,
    ._queue_recv = queue_recv_wrapper,
    ._queue_msg_waiting = uxQueueMessagesWaiting,
    ._event_group_create = xEventGroupCreate,
    ._event_group_delete = vEventGroupDelete,
    ._event_group_set_bits = xEventGroupSetBits,
    ._event_group_clear_bits = xEventGroupClearBits,
    ._event_group_wait_bits = event_group_wait_bits_wrapper,
    ._task_create_pinned_to_core = task_create_pinned_to_core_wrapper,
    ._task_create = task_create_wrapper,
    ._task_delete = vTaskDelete,
    ._task_delay = vTaskDelay,
    ._task_ms_to_tick = task_ms_to_tick_wrapper,
    ._task_get_current_task = xTaskGetCurrentTaskHandle,
    ._task_get_max_priority = task_get_max_priority_wrapper,
    ._malloc = malloc,
    ._free = free,
    ._event_post = esp_event_post_wrapper,
    ._get_free_heap_size = esp_get_free_heap_size,
    ._rand = esp_random,
    ._dport_access_stall_other_cpu_start_wrap = esp_dport_access_stall_other_cpu_start_wrap,
    ._dport_access_stall_other_cpu_end_wrap = esp_dport_access_stall_other_cpu_end_wrap,
    ._phy_rf_deinit = esp_phy_rf_deinit,
    ._phy_load_cal_and_init = esp_phy_load_cal_and_init,
    ._read_mac = esp_read_mac,
    ._timer_arm = timer_arm_wrapper,
    ._timer_disarm = timer_disarm_wrapper,
    ._timer_done = timer_done_wrapper,
    ._timer_setfn = timer_setfn_wrapper,
    ._timer_arm_us = timer_arm_us_wrapper,
    ._periph_module_enable = periph_module_enable,
    ._periph_module_disable = periph_module_disable,
    ._esp_timer_get_time = esp_timer_get_time,
    ._nvs_set_i8 = nvs_set_i8,
    ._nvs_get_i8 = nvs_get_i8,
    ._nvs_set_u8 = nvs_set_u8,
    ._nvs_get_u8 = nvs_get_u8,
    ._nvs_set_u16 = nvs_set_u16,
    ._nvs_get_u16 = nvs_get_u16,
    ._nvs_open = nvs_open,
    ._nvs_close = nvs_close,
    ._nvs_commit = nvs_commit,
    ._nvs_set_blob = nvs_set_blob,
    ._nvs_get_blob = nvs_get_blob,
    ._nvs_erase_key = nvs_erase_key,
    ._get_random = os_get_random,
    ._get_time = get_time_wrapper,
    ._random = os_random,
    ._log_write = esp_log_write,
    ._log_timestamp = esp_log_timestamp,
    ._malloc_internal =  malloc_internal_wrapper,
    ._realloc_internal = realloc_internal_wrapper,
    ._calloc_internal = calloc_internal_wrapper,
    ._zalloc_internal = zalloc_internal_wrapper,
    ._wifi_malloc = wifi_malloc,
    ._wifi_realloc = wifi_realloc,
    ._wifi_calloc = wifi_calloc,
    ._wifi_zalloc = wifi_zalloc_wrapper,
    ._wifi_create_queue = wifi_create_queue_wrapper,
    ._wifi_delete_queue = wifi_delete_queue_wrapper,
    ._modem_sleep_enter = esp_modem_sleep_enter,
    ._modem_sleep_exit = esp_modem_sleep_exit,
    ._modem_sleep_register = esp_modem_sleep_register,
    ._modem_sleep_deregister = esp_modem_sleep_deregister,
    ._coex_status_get = coex_status_get_wrapper,
    ._coex_wifi_request = coex_wifi_request_wrapper,
    ._coex_wifi_release = coex_wifi_release_wrapper,
    ._magic = ESP_WIFI_OS_ADAPTER_MAGIC,
};

coex_adapter_funcs_t g_coex_adapter_funcs = {
    ._version = COEX_ADAPTER_VERSION,
    ._spin_lock_create = spin_lock_create_wrapper,
    ._spin_lock_delete = free,
    ._int_disable = wifi_int_disable_wrapper,
    ._int_enable = wifi_int_restore_wrapper,
    ._task_yield_from_isr = task_yield_from_isr_wrapper,
    ._semphr_create = semphr_create_wrapper,
    ._semphr_delete = semphr_delete_wrapper,
    ._semphr_take_from_isr = semphr_take_from_isr_wrapper,
    ._semphr_give_from_isr = semphr_give_from_isr_wrapper,
    ._semphr_take = semphr_take_wrapper,
    ._semphr_give = semphr_give_wrapper,
    ._is_in_isr = xPortInIsrContext,
    ._malloc_internal =  malloc_internal_wrapper,
    ._free = free,
    ._timer_disarm = timer_disarm_wrapper,
    ._timer_done = timer_done_wrapper,
    ._timer_setfn = timer_setfn_wrapper,
    ._timer_arm_us = timer_arm_us_wrapper,
    ._esp_timer_get_time = esp_timer_get_time,
    ._magic = COEX_ADAPTER_MAGIC,
};
