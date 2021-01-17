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

#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "osi/semaphore.h"
#include "esp_task.h"
#include "bt_common.h"

#define portBASE_TYPE int

#define OSI_THREAD_MAX_TIMEOUT OSI_SEM_MAX_TIMEOUT

struct osi_thread;

typedef struct osi_thread osi_thread_t;

typedef void (*osi_thread_func_t)(void *context);

typedef enum {
    OSI_THREAD_CORE_0 = 0,
    OSI_THREAD_CORE_1,
    OSI_THREAD_CORE_AFFINITY,
} osi_thread_core_t;

/*
 * brief: Create a thread or task
 * param name: thread name
 * param stack_size: thread stack size
 * param priority: thread priority
 * param core: the CPU core which this thread run, OSI_THREAD_CORE_AFFINITY means unspecific CPU core
 * param work_queue_num: speicify queue number, the queue[0] has highest priority, and the priority is decrease by index
 * return : if create successfully, return thread handler; otherwise return NULL.
 */
osi_thread_t *osi_thread_create(const char *name, size_t stack_size, int priority, osi_thread_core_t core, uint8_t work_queue_num);

/*
 * brief: Destroy a thread or task
 * param thread: point of thread handler
 */
void osi_thread_free(osi_thread_t *thread);

/*
 * brief: Post an msg to a thread and told the thread call the function
 * param thread: point of thread handler
 * param func: callback function that called by target thread
 * param context: argument of callback function
 * param queue_idx: the queue which the msg send to
 * param timeout: post timeout, OSI_THREAD_MAX_TIMEOUT means blocking forever, 0 means never blocking, others means block millisecond
 * return : if post successfully, return true, otherwise return false
 */
bool osi_thread_post(osi_thread_t *thread, osi_thread_func_t func, void *context, int queue_idx, uint32_t timeout);

/*
 * brief: Set the priority of thread
 * param thread: point of thread handler
 * param priority: priority
 * return : if set successfully, return true, otherwise return false
 */
bool osi_thread_set_priority(osi_thread_t *thread, int priority);

/* brief: Get thread name
 * param thread: point of thread handler
 * return: constant point of thread name
 */
const char *osi_thread_name(osi_thread_t *thread);

/* brief: Get the size of the specified queue
 * param thread: point of thread handler
 * param wq_idx: the queue index of the thread
 * return: queue size
 */
int osi_thread_queue_wait_size(osi_thread_t *thread, int wq_idx);

#endif /* __THREAD_H__ */
