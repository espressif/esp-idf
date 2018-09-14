// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __THREAD_H__
#define __THREAD_H__

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_task.h"
#include "common/bt_defs.h"

#define portBASE_TYPE int

struct osi_thread;

typedef struct osi_thread osi_thread_t;

typedef void (*osi_thread_func_t)(void *context);

typedef enum {
    OSI_THREAD_CORE_0 = 0,
    OSI_THREAD_CORE_1,
    OSI_THREAD_CORE_AFFINITY,
} osi_thread_core_t;

typedef enum {
    OSI_THREAD_NON_BLOCKING = 0,
    OSI_THREAD_BLOCKING,
} osi_thread_blocking_t;

osi_thread_t *osi_thread_create(const char *name, size_t stack_size, int priority, osi_thread_core_t core, uint8_t work_queue_num);

void osi_thread_free(osi_thread_t *thread);

bool osi_thread_post(osi_thread_t *thread, osi_thread_func_t func, void *context, int queue_idx, osi_thread_blocking_t blocking);

bool osi_thread_set_priority(osi_thread_t *thread, int priority);

const char *osi_thread_name(osi_thread_t *thread);

int osi_thread_queue_wait_size(osi_thread_t *thread, int wq_idx);

#endif /* __THREAD_H__ */
