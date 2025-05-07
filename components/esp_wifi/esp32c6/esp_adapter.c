/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
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
#include "freertos/portmacro.h"
#include "riscv/interrupt.h"
#include "esp_types.h"
#include "esp_random.h"
#include "esp_mac.h"
#include "esp_task.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_heap_caps.h"
#include "esp_timer.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/wifi_os_adapter.h"
#include "esp_private/wifi.h"
#ifdef CONFIG_ESP_PHY_ENABLED
#include "esp_phy_init.h"
#include "phy_init_data.h"
#endif
#include "soc/rtc_cntl_periph.h"
#include "soc/rtc.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "nvs.h"
#include "os.h"
#include "esp_smartconfig.h"
#ifdef CONFIG_ESP_COEX_ENABLED
#include "private/esp_coexist_internal.h"
#endif
#include "esp32c6/rom/ets_sys.h"
#include "private/esp_modem_wrapper.h"
#include "esp_private/esp_modem_clock.h"

#if SOC_PM_MODEM_RETENTION_BY_REGDMA
#include "esp_private/esp_regdma.h"
#include "esp_private/sleep_retention.h"
#endif

#define TAG "esp_adapter"

#ifdef CONFIG_PM_ENABLE
extern void wifi_apb80m_request(void);
extern void wifi_apb80m_release(void);
#endif

IRAM_ATTR void *wifi_malloc(size_t size)
{
    return malloc(size);
}

IRAM_ATTR void *wifi_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

IRAM_ATTR void *wifi_calloc(size_t n, size_t size)
{
    return calloc(n, size);
}

static void *IRAM_ATTR wifi_zalloc_wrapper(size_t size)
{
    void *ptr = wifi_calloc(1, size);
    return ptr;
}

wifi_static_queue_t *wifi_create_queue(int queue_len, int item_size)
{
    wifi_static_queue_t *queue = NULL;

    queue = (wifi_static_queue_t *)heap_caps_malloc(sizeof(wifi_static_queue_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!queue) {
        return NULL;
    }

    queue->handle = xQueueCreate(queue_len, item_size);
    return queue;
}

void wifi_delete_queue(wifi_static_queue_t *queue)
{
    if (queue) {
        vQueueDelete(queue->handle);
        free(queue);
    }
}

static void *wifi_create_queue_wrapper(int queue_len, int item_size)
{
    return wifi_create_queue(queue_len, item_size);
}

static void wifi_delete_queue_wrapper(void *queue)
{
    wifi_delete_queue(queue);
}

static void set_intr_wrapper(int32_t cpu_no, uint32_t intr_source, uint32_t intr_num, int32_t intr_prio)
{
    esp_rom_route_intr_matrix(cpu_no, intr_source, intr_num);
    esprv_int_set_priority(intr_num, intr_prio);
    esprv_int_set_type(intr_num, INTR_TYPE_LEVEL);
}

static void clear_intr_wrapper(uint32_t intr_source, uint32_t intr_num)
{

}

static void set_isr_wrapper(int32_t n, void *f, void *arg)
{
    intr_handler_set(n, (intr_handler_t)f, arg);
}

static void enable_intr_wrapper(uint32_t intr_mask)
{
    esprv_int_enable(intr_mask);
}

static void disable_intr_wrapper(uint32_t intr_mask)
{
    esprv_int_disable(intr_mask);
}

static bool IRAM_ATTR is_from_isr_wrapper(void)
{
    return !xPortCanYield();
}

static void wifi_thread_semphr_free(void *data)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t *)(data);

    if (sem) {
        vSemaphoreDelete(sem);
    }
}

static void *wifi_thread_semphr_get_wrapper(void)
{
    static bool s_wifi_thread_sem_key_init = false;
    static pthread_key_t s_wifi_thread_sem_key;
    SemaphoreHandle_t sem = NULL;

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
    return (void *)sem;
}

static void *recursive_mutex_create_wrapper(void)
{
    return (void *)xSemaphoreCreateRecursiveMutex();
}

static void *mutex_create_wrapper(void)
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

static void *queue_create_wrapper(uint32_t queue_len, uint32_t item_size)
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
    return (uint32_t)xTaskCreatePinnedToCore(task_func, name, stack_depth, param, prio, task_handle, (core_id < CONFIG_FREERTOS_NUMBER_OF_CORES ? core_id : tskNO_AFFINITY));
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

static int32_t esp_event_post_wrapper(const char *event_base, int32_t event_id, void *event_data, size_t event_data_size, uint32_t ticks_to_wait)
{
    if (ticks_to_wait == OSI_FUNCS_TIME_BLOCKING) {
        return (int32_t)esp_event_post(event_base, event_id, event_data, event_data_size, portMAX_DELAY);
    } else {
        return (int32_t)esp_event_post(event_base, event_id, event_data, event_data_size, ticks_to_wait);
    }
}

static void IRAM_ATTR wifi_apb80m_request_wrapper(void)
{
#ifdef CONFIG_PM_ENABLE
    wifi_apb80m_request();
#endif
}

static void IRAM_ATTR wifi_apb80m_release_wrapper(void)
{
#ifdef CONFIG_PM_ENABLE
    wifi_apb80m_release();
#endif
}

static void IRAM_ATTR timer_arm_wrapper(void *timer, uint32_t tmout, bool repeat)
{
    ets_timer_arm(timer, tmout, repeat);
}

static void wifi_reset_mac_wrapper(void)
{
    modem_clock_module_mac_reset(PERIPH_WIFI_MODULE);
}

static void wifi_clock_enable_wrapper(void)
{
    wifi_module_enable();
}

static void wifi_clock_disable_wrapper(void)
{
    wifi_module_disable();
}

static int get_time_wrapper(void *t)
{
    return os_get_time(t);
}

static void *IRAM_ATTR realloc_internal_wrapper(void *ptr, size_t size)
{
    return heap_caps_realloc(ptr, size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

static void *IRAM_ATTR calloc_internal_wrapper(size_t n, size_t size)
{
    return heap_caps_calloc(n, size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

static void *IRAM_ATTR zalloc_internal_wrapper(size_t size)
{
    void *ptr = heap_caps_calloc(1, size, MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    return ptr;
}

static esp_err_t nvs_open_wrapper(const char *name, unsigned int open_mode, nvs_handle_t *out_handle)
{
    return nvs_open(name, (nvs_open_mode_t)open_mode, out_handle);
}

static void esp_log_writev_wrapper(unsigned int level, const char *tag, const char *format, va_list args)
{
    return esp_log_writev((esp_log_level_t)level, tag, format, args);
}

static void esp_log_write_wrapper(unsigned int level, const char *tag, const char *format, ...)
{
    va_list list;
    va_start(list, format);
    esp_log_writev((esp_log_level_t)level, tag, format, list);
    va_end(list);
}

static esp_err_t esp_read_mac_wrapper(uint8_t *mac, unsigned int type)
{
    return esp_read_mac(mac, (esp_mac_type_t)type);
}

static int coex_init_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_init();
#else
    return 0;
#endif
}

static void coex_deinit_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    coex_deinit();
#endif
}

static int coex_enable_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_enable();
#else
    return 0;
#endif
}

static void coex_disable_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    coex_disable();
#endif
}

static IRAM_ATTR uint32_t coex_status_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_status_get(COEX_STATUS_GET_WIFI_BITMAP);
#else
    return 0;
#endif
}

static int coex_wifi_request_wrapper(uint32_t event, uint32_t latency, uint32_t duration)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_wifi_request(event, latency, duration);
#else
    return 0;
#endif
}

static IRAM_ATTR int coex_wifi_release_wrapper(uint32_t event)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_wifi_release(event);
#else
    return 0;
#endif
}

static int coex_wifi_channel_set_wrapper(uint8_t primary, uint8_t secondary)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_wifi_channel_set(primary, secondary);
#else
    return 0;
#endif
}

static IRAM_ATTR int coex_event_duration_get_wrapper(uint32_t event, uint32_t *duration)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_event_duration_get(event, duration);
#else
    return 0;
#endif
}

static int coex_pti_get_wrapper(uint32_t event, uint8_t *pti)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_pti_get(event, pti);
#else
    return 0;
#endif
}

static void coex_schm_status_bit_clear_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    coex_schm_status_bit_clear(type, status);
#endif
}

static void coex_schm_status_bit_set_wrapper(uint32_t type, uint32_t status)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    coex_schm_status_bit_set(type, status);
#endif
}

static IRAM_ATTR int coex_schm_interval_set_wrapper(uint32_t interval)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_interval_set(interval);
#else
    return 0;
#endif
}

static uint32_t coex_schm_interval_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_interval_get();
#else
    return 0;
#endif
}

static uint8_t coex_schm_curr_period_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_curr_period_get();
#else
    return 0;
#endif
}

static void *coex_schm_curr_phase_get_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_curr_phase_get();
#else
    return NULL;
#endif
}

static int coex_register_start_cb_wrapper(int (* cb)(void))
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_register_start_cb(cb);
#else
    return 0;
#endif
}

static int coex_schm_process_restart_wrapper(void)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_process_restart();
#else
    return 0;
#endif
}

static int coex_schm_register_cb_wrapper(int type, int(*cb)(int))
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_register_callback(type, cb);
#else
    return 0;
#endif
}

static int coex_schm_flexible_period_set_wrapper(uint8_t period)
{
#if CONFIG_ESP_COEX_POWER_MANAGEMENT
    return coex_schm_flexible_period_set(period);
#else
    return 0;
#endif
}

static uint8_t coex_schm_flexible_period_get_wrapper(void)
{
#if CONFIG_ESP_COEX_POWER_MANAGEMENT
    return coex_schm_flexible_period_get();
#else
    return 1;
#endif
}

static void * coex_schm_get_phase_by_idx_wrapper(int phase_idx)
{
#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
    return coex_schm_get_phase_by_idx(phase_idx);
#else
    return NULL;
#endif
}

static void IRAM_ATTR esp_empty_wrapper(void)
{

}

static void esp_phy_enable_wrapper(void)
{
    esp_phy_enable(PHY_MODEM_WIFI);
    phy_wifi_enable_set(1);
}

static void esp_phy_disable_wrapper(void)
{
    phy_wifi_enable_set(0);
    esp_phy_disable(PHY_MODEM_WIFI);
}

#if SOC_PM_MODEM_RETENTION_BY_REGDMA
static void regdma_link_set_write_wait_content_wrapper(void *addr, uint32_t value, uint32_t mask)
{
    regdma_link_set_write_wait_content(addr, value, mask);
}

static void *sleep_retention_find_link_by_id_wrapper(int id)
{
    return sleep_retention_find_link_by_id(id);
}
#endif

wifi_osi_funcs_t g_wifi_osi_funcs = {
    ._version = ESP_WIFI_OS_ADAPTER_VERSION,
    ._env_is_chip = esp_coex_common_env_is_chip_wrapper,
    ._set_intr = set_intr_wrapper,
    ._clear_intr = clear_intr_wrapper,
    ._set_isr = set_isr_wrapper,
    ._ints_on = enable_intr_wrapper,
    ._ints_off = disable_intr_wrapper,
    ._is_from_isr = is_from_isr_wrapper,
    ._spin_lock_create = esp_coex_common_spin_lock_create_wrapper,
    ._spin_lock_delete = free,
    ._wifi_int_disable = esp_coex_common_int_disable_wrapper,
    ._wifi_int_restore = esp_coex_common_int_restore_wrapper,
    ._task_yield_from_isr = esp_coex_common_task_yield_from_isr_wrapper,
    ._semphr_create = esp_coex_common_semphr_create_wrapper,
    ._semphr_delete = esp_coex_common_semphr_delete_wrapper,
    ._semphr_take = esp_coex_common_semphr_take_wrapper,
    ._semphr_give = esp_coex_common_semphr_give_wrapper,
    ._wifi_thread_semphr_get = wifi_thread_semphr_get_wrapper,
    ._mutex_create = mutex_create_wrapper,
    ._recursive_mutex_create = recursive_mutex_create_wrapper,
    ._mutex_delete = mutex_delete_wrapper,
    ._mutex_lock = mutex_lock_wrapper,
    ._mutex_unlock = mutex_unlock_wrapper,
    ._queue_create = queue_create_wrapper,
    ._queue_delete = (void(*)(void *))vQueueDelete,
    ._queue_send = queue_send_wrapper,
    ._queue_send_from_isr = queue_send_from_isr_wrapper,
    ._queue_send_to_back = queue_send_to_back_wrapper,
    ._queue_send_to_front = queue_send_to_front_wrapper,
    ._queue_recv = queue_recv_wrapper,
    ._queue_msg_waiting = (uint32_t(*)(void *))uxQueueMessagesWaiting,
    ._event_group_create = (void *(*)(void))xEventGroupCreate,
    ._event_group_delete = (void(*)(void *))vEventGroupDelete,
    ._event_group_set_bits = (uint32_t(*)(void *, uint32_t))xEventGroupSetBits,
    ._event_group_clear_bits = (uint32_t(*)(void *, uint32_t))xEventGroupClearBits,
    ._event_group_wait_bits = event_group_wait_bits_wrapper,
    ._task_create_pinned_to_core = task_create_pinned_to_core_wrapper,
    ._task_create = task_create_wrapper,
    ._task_delete = (void(*)(void *))vTaskDelete,
    ._task_delay = vTaskDelay,
    ._task_ms_to_tick = task_ms_to_tick_wrapper,
    ._task_get_current_task = (void *(*)(void))xTaskGetCurrentTaskHandle,
    ._task_get_max_priority = task_get_max_priority_wrapper,
    ._malloc = malloc,
    ._free = free,
    ._event_post = esp_event_post_wrapper,
    ._get_free_heap_size = esp_get_free_internal_heap_size,
    ._rand = esp_random,
    ._dport_access_stall_other_cpu_start_wrap = esp_empty_wrapper,
    ._dport_access_stall_other_cpu_end_wrap = esp_empty_wrapper,
    ._wifi_apb80m_request = wifi_apb80m_request_wrapper,
    ._wifi_apb80m_release = wifi_apb80m_release_wrapper,
    ._phy_disable = esp_phy_disable_wrapper,
    ._phy_enable = esp_phy_enable_wrapper,
    ._phy_update_country_info = esp_phy_update_country_info,
    ._read_mac = esp_read_mac_wrapper,
    ._timer_arm = timer_arm_wrapper,
    ._timer_disarm = esp_coex_common_timer_disarm_wrapper,
    ._timer_done = esp_coex_common_timer_done_wrapper,
    ._timer_setfn = esp_coex_common_timer_setfn_wrapper,
    ._timer_arm_us = esp_coex_common_timer_arm_us_wrapper,
    ._wifi_reset_mac = wifi_reset_mac_wrapper,
    ._wifi_clock_enable = wifi_clock_enable_wrapper,
    ._wifi_clock_disable = wifi_clock_disable_wrapper,
    ._wifi_rtc_enable_iso = esp_empty_wrapper,
    ._wifi_rtc_disable_iso = esp_empty_wrapper,
    ._esp_timer_get_time = esp_timer_get_time,
    ._nvs_set_i8 = nvs_set_i8,
    ._nvs_get_i8 = nvs_get_i8,
    ._nvs_set_u8 = nvs_set_u8,
    ._nvs_get_u8 = nvs_get_u8,
    ._nvs_set_u16 = nvs_set_u16,
    ._nvs_get_u16 = nvs_get_u16,
    ._nvs_open = nvs_open_wrapper,
    ._nvs_close = nvs_close,
    ._nvs_commit = nvs_commit,
    ._nvs_set_blob = nvs_set_blob,
    ._nvs_get_blob = nvs_get_blob,
    ._nvs_erase_key = nvs_erase_key,
    ._get_random = os_get_random,
    ._get_time = get_time_wrapper,
    ._random = os_random,
    ._slowclk_cal_get = esp_coex_common_clk_slowclk_cal_get_wrapper,
    ._log_write = esp_log_write_wrapper,
    ._log_writev = esp_log_writev_wrapper,
    ._log_timestamp = esp_log_timestamp,
    ._malloc_internal =  esp_coex_common_malloc_internal_wrapper,
    ._realloc_internal = realloc_internal_wrapper,
    ._calloc_internal = calloc_internal_wrapper,
    ._zalloc_internal = zalloc_internal_wrapper,
    ._wifi_malloc = wifi_malloc,
    ._wifi_realloc = wifi_realloc,
    ._wifi_calloc = wifi_calloc,
    ._wifi_zalloc = wifi_zalloc_wrapper,
    ._wifi_create_queue = wifi_create_queue_wrapper,
    ._wifi_delete_queue = wifi_delete_queue_wrapper,
    ._coex_init = coex_init_wrapper,
    ._coex_deinit = coex_deinit_wrapper,
    ._coex_enable = coex_enable_wrapper,
    ._coex_disable = coex_disable_wrapper,
    ._coex_status_get = coex_status_get_wrapper,
    ._coex_wifi_request = coex_wifi_request_wrapper,
    ._coex_wifi_release = coex_wifi_release_wrapper,
    ._coex_wifi_channel_set = coex_wifi_channel_set_wrapper,
    ._coex_event_duration_get = coex_event_duration_get_wrapper,
    ._coex_pti_get = coex_pti_get_wrapper,
    ._coex_schm_status_bit_clear = coex_schm_status_bit_clear_wrapper,
    ._coex_schm_status_bit_set = coex_schm_status_bit_set_wrapper,
    ._coex_schm_interval_set = coex_schm_interval_set_wrapper,
    ._coex_schm_interval_get = coex_schm_interval_get_wrapper,
    ._coex_schm_curr_period_get = coex_schm_curr_period_get_wrapper,
    ._coex_schm_curr_phase_get = coex_schm_curr_phase_get_wrapper,
    ._coex_register_start_cb = coex_register_start_cb_wrapper,
#if SOC_PM_MODEM_RETENTION_BY_REGDMA
    ._regdma_link_set_write_wait_content = regdma_link_set_write_wait_content_wrapper,
    ._sleep_retention_find_link_by_id = sleep_retention_find_link_by_id_wrapper,
#endif
    ._coex_schm_process_restart = coex_schm_process_restart_wrapper,
    ._coex_schm_register_cb = coex_schm_register_cb_wrapper,
    ._coex_schm_flexible_period_set = coex_schm_flexible_period_set_wrapper,
    ._coex_schm_flexible_period_get = coex_schm_flexible_period_get_wrapper,
    ._coex_schm_get_phase_by_idx = coex_schm_get_phase_by_idx_wrapper,
    ._magic = ESP_WIFI_OS_ADAPTER_MAGIC,
};
