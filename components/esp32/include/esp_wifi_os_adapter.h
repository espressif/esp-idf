// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef ESP_WIFI_OS_ADAPTER_H_
#define ESP_WIFI_OS_ADAPTER_H_

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_WIFI_OS_ADAPTER_VERSION  0x00000005
#define ESP_WIFI_OS_ADAPTER_MAGIC    0xDEADBEAF

#define OSI_FUNCS_TIME_BLOCKING      0xffffffff

#define OSI_QUEUE_SEND_FRONT         0
#define OSI_QUEUE_SEND_BACK          1
#define OSI_QUEUE_SEND_OVERWRITE     2

typedef struct {
    int32_t _version;
    void (*_set_isr)(int32_t n, void *f, void *arg);
    void (*_ints_on)(uint32_t mask);
    void (*_ints_off)(uint32_t mask);
    void *(* _spin_lock_create)(void);
    void (* _spin_lock_delete)(void *lock);
    uint32_t (*_wifi_int_disable)(void *wifi_int_mux);
    void (*_wifi_int_restore)(void *wifi_int_mux, uint32_t tmp);
    void (*_task_yield_from_isr)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    void (*_semphr_delete)(void *semphr);
    int32_t (*_semphr_take)(void *semphr, uint32_t block_time_tick);
    int32_t (*_semphr_give)(void *semphr);
    void *(*_wifi_thread_semphr_get)(void);
    void *(*_mutex_create)(void);
    void *(*_recursive_mutex_create)(void);
    void (*_mutex_delete)(void *mutex);
    int32_t (*_mutex_lock)(void *mutex);
    int32_t (*_mutex_unlock)(void *mutex);
    void *(* _queue_create)(uint32_t queue_len, uint32_t item_size);
    void (* _queue_delete)(void *queue);
    int32_t (* _queue_send)(void *queue, void *item, uint32_t block_time_tick);
    int32_t (* _queue_send_from_isr)(void *queue, void *item, void *hptw);
    int32_t (* _queue_send_to_back)(void *queue, void *item, uint32_t block_time_tick);
    int32_t (* _queue_send_to_front)(void *queue, void *item, uint32_t block_time_tick);
    int32_t (* _queue_recv)(void *queue, void *item, uint32_t block_time_tick);
    uint32_t (* _queue_msg_waiting)(void *queue);
    void *(* _event_group_create)(void);
    void (* _event_group_delete)(void *event);
    uint32_t (* _event_group_set_bits)(void *event, uint32_t bits);
    uint32_t (* _event_group_clear_bits)(void *event, uint32_t bits);
    uint32_t (* _event_group_wait_bits)(void *event, uint32_t bits_to_wait_for, int32_t clear_on_exit, int32_t wait_for_all_bits, uint32_t block_time_tick);
    int32_t (* _task_create_pinned_to_core)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle, uint32_t core_id);
    int32_t (* _task_create)(void *task_func, const char *name, uint32_t stack_depth, void *param, uint32_t prio, void *task_handle);
    void (* _task_delete)(void *task_handle);
    void (* _task_delay)(uint32_t tick);
    int32_t (* _task_ms_to_tick)(uint32_t ms);
    void *(* _task_get_current_task)(void);
    int32_t (* _task_get_max_priority)(void);
    void *(* _malloc)(uint32_t size);
    void (* _free)(void *p);
    uint32_t (* _get_free_heap_size)(void);
    uint32_t (* _rand)(void);
    void (* _dport_access_stall_other_cpu_start_wrap)(void);
    void (* _dport_access_stall_other_cpu_end_wrap)(void);
    int32_t (* _phy_rf_deinit)(uint32_t module);
    void (* _phy_load_cal_and_init)(uint32_t module);
    void (* _phy_common_clock_enable)(void);
    void (* _phy_common_clock_disable)(void);
    int32_t (* _read_mac)(uint8_t* mac, uint32_t type);
    void (* _timer_arm)(void *timer, uint32_t tmout, bool repeat);
    void (* _timer_disarm)(void *timer);
    void (* _timer_done)(void *ptimer);
    void (* _timer_setfn)(void *ptimer, void *pfunction, void *parg);
    void (* _timer_arm_us)(void *ptimer, uint32_t us, bool repeat);
    void (* _periph_module_enable)(uint32_t periph);
    void (* _periph_module_disable)(uint32_t periph);
    int64_t (* _esp_timer_get_time)(void);
    int32_t (* _nvs_set_i8)(uint32_t handle, const char* key, int8_t value);
    int32_t (* _nvs_get_i8)(uint32_t handle, const char* key, int8_t* out_value);
    int32_t (* _nvs_set_u8)(uint32_t handle, const char* key, uint8_t value);
    int32_t (* _nvs_get_u8)(uint32_t handle, const char* key, uint8_t* out_value);
    int32_t (* _nvs_set_u16)(uint32_t handle, const char* key, uint16_t value);
    int32_t (* _nvs_get_u16)(uint32_t handle, const char* key, uint16_t* out_value);
    int32_t (* _nvs_open)(const char* name, uint32_t open_mode, uint32_t *out_handle);
    void (* _nvs_close)(uint32_t handle); 
    int32_t (* _nvs_commit)(uint32_t handle);
    int32_t (* _nvs_set_blob)(uint32_t handle, const char* key, const void* value, size_t length);
    int32_t (* _nvs_get_blob)(uint32_t handle, const char* key, void* out_value, size_t* length);
    int32_t (* _nvs_erase_key)(uint32_t handle, const char* key);
    int32_t (* _get_random)(uint8_t *buf, size_t len);
    int32_t (* _get_time)(void *t);
    unsigned long (* _random)(void);
    void (* _log_write)(uint32_t level, const char* tag, const char* format, ...);
    void (* _log_writev)(uint32_t level, const char* tag, const char* format, va_list args);
    uint32_t (* _log_timestamp)(void);
    void * (* _malloc_internal)(size_t size);
    void * (* _realloc_internal)(void *ptr, size_t size);
    void * (* _calloc_internal)(size_t n, size_t size);
    void * (* _zalloc_internal)(size_t size);
    void * (* _wifi_malloc)(size_t size);
    void * (* _wifi_realloc)(void *ptr, size_t size);
    void * (* _wifi_calloc)(size_t n, size_t size);
    void * (* _wifi_zalloc)(size_t size);
    void * (* _wifi_create_queue)(int32_t queue_len, int32_t item_size);
    void (* _wifi_delete_queue)(void * queue);
    int32_t (* _modem_sleep_enter)(uint32_t module);
    int32_t (* _modem_sleep_exit)(uint32_t module);
    int32_t (* _modem_sleep_register)(uint32_t module);
    int32_t (* _modem_sleep_deregister)(uint32_t module);
    void (* _sc_ack_send)(void *param);
    void (* _sc_ack_send_stop)(void);
    uint32_t (* _coex_status_get)(void);
    void (* _coex_condition_set)(uint32_t type, bool dissatisfy);
    int32_t (* _coex_wifi_request)(uint32_t event, uint32_t latency, uint32_t duration);
    int32_t (* _coex_wifi_release)(uint32_t event);
    int (* _coex_wifi_channel_set)(uint8_t primary, uint8_t secondary);
    void (* _coex_schm_status_bit_clear)(uint32_t type, uint32_t status);
    void (* _coex_schm_status_bit_set)(uint32_t type, uint32_t status);
    int (* _coex_schm_interval_set)(uint32_t interval);
    uint32_t (* _coex_schm_interval_get)(void);
    uint8_t (* _coex_schm_curr_period_get)(void);
    void * (* _coex_schm_curr_phase_get)(void);
    int (* _coex_schm_curr_phase_idx_set)(int idx);
    int (* _coex_schm_curr_phase_idx_get)(void);
    int32_t _magic;
} wifi_osi_funcs_t;

extern wifi_osi_funcs_t g_wifi_osi_funcs;

#ifdef __cplusplus
}
#endif

#endif /* ESP_WIFI_OS_ADAPTER_H_ */
