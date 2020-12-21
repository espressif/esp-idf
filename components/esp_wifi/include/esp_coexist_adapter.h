// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_COEXIST_ADAPTER_H__
#define __ESP_COEXIST_ADAPTER_H__

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COEX_ADAPTER_VERSION  0x00000002
#define COEX_ADAPTER_MAGIC    0xDEADBEAF

#define COEX_ADAPTER_FUNCS_TIME_BLOCKING      0xffffffff

typedef struct {
    int32_t _version;
#if CONFIG_IDF_TARGET_ESP32
    void *(* _spin_lock_create)(void);
    void (* _spin_lock_delete)(void *lock);
    uint32_t (*_int_disable)(void *mux);
    void (*_int_enable)(void *mux, uint32_t tmp);
#endif
    void (*_task_yield_from_isr)(void);
    void *(*_semphr_create)(uint32_t max, uint32_t init);
    void (*_semphr_delete)(void *semphr);
    int32_t (*_semphr_take_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_give_from_isr)(void *semphr, void *hptw);
    int32_t (*_semphr_take)(void *semphr, uint32_t block_time_tick);
    int32_t (*_semphr_give)(void *semphr);
    int (* _is_in_isr)(void);
    void * (* _malloc_internal)(size_t size);
    void (* _free)(void *p);
#if CONFIG_IDF_TARGET_ESP32
    void (* _timer_disarm)(void *timer);
    void (* _timer_done)(void *ptimer);
    void (* _timer_setfn)(void *ptimer, void *pfunction, void *parg);
    void (* _timer_arm_us)(void *ptimer, uint32_t us, bool repeat);
#endif
    int64_t (* _esp_timer_get_time)(void);
    int32_t _magic;
} coex_adapter_funcs_t;

extern coex_adapter_funcs_t g_coex_adapter_funcs;

#ifdef __cplusplus
}
#endif

#endif /* __ESP_COEXIST_ADAPTER_H__ */
