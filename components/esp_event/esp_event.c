// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "esp_log.h"

#include "esp_event.h"
#include "esp_event_internal.h"
#include "esp_event_private.h"

#ifdef CONFIG_EVENT_LOOP_PROFILING
#include "esp_timer.h"
#endif

/* ---------------------------- Definitions --------------------------------- */

#ifdef CONFIG_EVENT_LOOP_PROFILING
// loop@<address,name> rx:<total_recieved> dr:<total_dropped> inv:<total_number_of_invocations> run:<total_runtime>
#define LOOP_DUMP_FORMAT              "loop@%p,%s rx:%u dr:%u inv:%u run:%lld us\n"
// event@<base:id> proc:<total_processed> run:<total_runtime>
#define EVENT_DUMP_FORMAT             "\tevent@%s:%d proc:%u run:%lld us\n"
 // handler@<address> inv:<total_invoked> run:<total_runtime>
#define HANDLER_DUMP_FORMAT           "\t\thandler@%p inv:%u run:%lld us\n"

#define PRINT_DUMP_INFO(dst, sz, ...)  do { \
                                            int cb = snprintf(dst, sz, __VA_ARGS__); \
                                            dst += cb; \
                                            sz -= cb; \
                                        } while(0);
#endif

/* ------------------------- Static Variables ------------------------------- */

static const char* TAG = "event";
static const char* esp_event_any_base = "any";

#ifdef CONFIG_EVENT_LOOP_PROFILING
static SLIST_HEAD(esp_event_loop_instance_list_t, esp_event_loop_instance) s_event_loops =
        SLIST_HEAD_INITIALIZER(s_event_loops);

static portMUX_TYPE s_event_loops_spinlock = portMUX_INITIALIZER_UNLOCKED;
#endif


/* ------------------------- Static Functions ------------------------------- */

#ifdef CONFIG_EVENT_LOOP_PROFILING
static int esp_event_dump_prepare()
{
    esp_event_loop_instance_t* loop_it;
    esp_event_base_instance_t* base_it;
    esp_event_id_instance_t* id_it;
    esp_event_handler_instance_t* handler_it;

    // Count the number of items to be printed. This is needed to compute how much memory to reserve.
    int loops = 0, events = 0, handlers = 0;

    portENTER_CRITICAL(&s_event_loops_spinlock);

    SLIST_FOREACH(loop_it, &s_event_loops, loop_entry) {
        SLIST_FOREACH(handler_it, &(loop_it->loop_handlers), handler_entry) {
            handlers++;
        }
        SLIST_FOREACH(base_it, &(loop_it->event_bases), event_base_entry) {
            SLIST_FOREACH(handler_it, &(base_it->base_handlers), handler_entry) {
                handlers++;
            }
            // Print event-level handlers
            SLIST_FOREACH(id_it, &(base_it->event_ids), event_id_entry) {
                SLIST_FOREACH(handler_it, &(id_it->handlers), handler_entry) {
                    handlers++;
                }
                events++;
            }
            events++;
        }
        events++;
        loops++;
    }

    portEXIT_CRITICAL(&s_event_loops_spinlock);

    // Reserve slightly more memory than computed
    int allowance = 3;
    int size = (((loops + allowance) * (sizeof(LOOP_DUMP_FORMAT) + 10 + 20 + 3 * 11  + 20 )) +
                        ((events + allowance) * (sizeof(EVENT_DUMP_FORMAT) + 10 + 20 + 11 + 20)) +
                        ((handlers + allowance) * (sizeof(HANDLER_DUMP_FORMAT) + 10 + 11 + 20)));

    return size;
}
#endif

static void esp_event_loop_run_task(void* args)
{
    esp_err_t err;
    esp_event_loop_handle_t event_loop = (esp_event_loop_handle_t) args;

    ESP_LOGD(TAG, "running task for loop %p", event_loop);

    while(1) {
        err = esp_event_loop_run(event_loop, portMAX_DELAY);
        if (err != ESP_OK) {
            break;
        }
    }

    ESP_LOGE(TAG, "suspended task for loop %p", event_loop);
    vTaskSuspend(NULL);
}

// Functions that operate on handler instance
static esp_event_handler_instance_t* handler_instance_create(esp_event_handler_t event_handler, void* event_handler_arg)
{
    esp_event_handler_instance_t* handler_instance = calloc(1, sizeof(*handler_instance));

    if (handler_instance != NULL) {
        handler_instance->handler = event_handler;
        handler_instance->arg = event_handler_arg;
    }

    return handler_instance;
}

static void handler_instance_delete(esp_event_handler_instance_t* handler_instance)
{
    free(handler_instance);
}

// Functions that operate on handler instance list
static esp_event_handler_instance_t* handler_instances_find(esp_event_handler_instances_t* handlers, esp_event_handler_t handler)
{
    esp_event_handler_instance_t* it;

    SLIST_FOREACH(it, handlers, handler_entry) {
        if (it->handler == handler) {
            break;
        }
    }

    return it;
}

static void handler_instances_add(esp_event_handler_instances_t* handlers, esp_event_handler_instance_t* handler_instance)
{
    SLIST_INSERT_HEAD(handlers, handler_instance, handler_entry);
}

static void handler_instances_remove(esp_event_handler_instances_t* handlers, esp_event_handler_instance_t* handler_instance)
{
    SLIST_REMOVE(handlers, handler_instance, esp_event_handler_instance, handler_entry);
    handler_instance_delete(handler_instance);
}

static void handler_instances_remove_all(esp_event_handler_instances_t* handlers)
{
    esp_event_handler_instance_t* it;
    esp_event_handler_instance_t* temp;

    SLIST_FOREACH_SAFE(it, handlers, handler_entry, temp) {
        handler_instances_remove(handlers, it);
    }
}

// Functions that operate on event id instance
static void* event_id_instance_create(int32_t event_id)
{
    esp_event_id_instance_t* event_id_instance = calloc(1, sizeof(*event_id_instance));

    if (event_id_instance != NULL) {
        event_id_instance->id = event_id;
        SLIST_INIT(&(event_id_instance->handlers));
    }

    return event_id_instance;
}

static void event_id_instance_delete(esp_event_id_instance_t* event_id_instance)
{
    handler_instances_remove_all(&(event_id_instance->handlers));
    free(event_id_instance);
}

// Functions that operate on event id instance list
static void event_id_instances_remove(esp_event_id_instances_t* head, esp_event_id_instance_t* event_id_instance)
{
    SLIST_REMOVE(head, event_id_instance, esp_event_id_instance, event_id_entry);
    event_id_instance_delete(event_id_instance);
}

// Functions that operate on event base instance
static esp_event_base_instance_t* event_base_instance_create(esp_event_base_t event_base)
{
    esp_event_base_instance_t* event_base_instance = calloc(1, sizeof(*event_base_instance));

    if (event_base_instance != NULL) {
        event_base_instance->base = event_base;
        SLIST_INIT(&(event_base_instance->base_handlers));
        SLIST_INIT(&(event_base_instance->event_ids));
    }

    return event_base_instance;
}

static void event_base_instance_delete(esp_event_base_instance_t* event_base_instance)
{
    esp_event_id_instance_t* it;
    esp_event_id_instance_t* temp;

    handler_instances_remove_all(&(event_base_instance->base_handlers));

    SLIST_FOREACH_SAFE(it, &(event_base_instance->event_ids), event_id_entry, temp) {
        event_id_instances_remove(&(event_base_instance->event_ids), it);
    }

    free(event_base_instance);
}

static void event_base_instance_add_event_id_instance(esp_event_base_instance_t* event_base_instance, esp_event_id_instance_t* event_id_instance)
{
    SLIST_INSERT_HEAD(&(event_base_instance->event_ids), event_id_instance, event_id_entry);
}

static esp_event_id_instance_t* event_base_instance_find_event_id_instance(esp_event_base_instance_t* event_base_instance, int32_t event_id)
{
    esp_event_id_instance_t* it;

    SLIST_FOREACH(it, &(event_base_instance->event_ids), event_id_entry) {
        if (it->id == event_id) {
            break;
        }
    }

    return it;
}

// Functions that operate on event base instances list
static void event_base_instances_remove(esp_event_base_instances_t* head, esp_event_base_instance_t* event_base_instance)
{
    SLIST_REMOVE(head, event_base_instance, esp_event_base_instance, event_base_entry);
    event_base_instance_delete(event_base_instance);
}

// Functions that operate on loop instances
static void loop_add_event_base_instance(esp_event_loop_instance_t* loop, esp_event_base_instance_t* event_base_instance) {
    SLIST_INSERT_HEAD(&(loop->event_bases), event_base_instance, event_base_entry);
}

static void loop_remove_all_event_base_instance(esp_event_loop_instance_t* loop)
{
    esp_event_base_instance_t* it;
    esp_event_base_instance_t* temp;

    SLIST_FOREACH_SAFE(it, &(loop->event_bases), event_base_entry, temp) {
        event_base_instances_remove(&(loop->event_bases), it);
    }
}

static esp_event_base_instance_t* loop_find_event_base_instance(esp_event_loop_instance_t* loop, esp_event_base_t event_base)
{
    esp_event_base_instance_t* it;

    SLIST_FOREACH(it, &(loop->event_bases), event_base_entry) {
        if (it->base == event_base) {
            break;
        }
    }

    return it;
}

// Functions that operate on post instance
static esp_err_t post_instance_create(esp_event_base_t event_base, int32_t event_id, void* event_data, int32_t event_data_size, esp_event_post_instance_t* post)
{
    void** event_data_copy = NULL;

    // Make persistent copy of event data on heap.
    if (event_data != NULL && event_data_size != 0) {
        event_data_copy = calloc(1, event_data_size);

        if (event_data_copy == NULL) {
            ESP_LOGE(TAG, "alloc for post data to event %s:%d failed", event_base, event_id);
            return ESP_ERR_NO_MEM;
        }

        memcpy(event_data_copy, event_data, event_data_size);
    }

    post->base = event_base;
    post->id = event_id;
    post->data = event_data_copy;

    ESP_LOGD(TAG, "created post for event %s:%d", event_base, event_id);

    return ESP_OK;
}

static void post_instance_delete(esp_event_post_instance_t* post)
{
    free(post->data);
}

static esp_event_handler_instances_t* find_handlers_list(esp_event_loop_instance_t* loop, esp_event_base_t event_base,
                                        int32_t event_id)
{
    esp_event_handler_instances_t* handlers = NULL;
    
    esp_event_base_instance_t* base = NULL;
    esp_event_id_instance_t* event = NULL;

    if (event_base == esp_event_any_base && event_id == ESP_EVENT_ANY_ID) {
        handlers = &(loop->loop_handlers);
    } else {
        base = loop_find_event_base_instance(loop, event_base);
        if (base != NULL) {
            if (event_id == ESP_EVENT_ANY_ID) {
                handlers = &(base->base_handlers);
            } else {
                event = event_base_instance_find_event_id_instance(base, event_id);
                if (event != NULL) {
                    handlers = &(event->handlers);
                }
            }
        }
    }

    return handlers;
}

/* ---------------------------- Public API --------------------------------- */

esp_err_t esp_event_loop_create(const esp_event_loop_args_t* event_loop_args, esp_event_loop_handle_t* event_loop)
{
    assert(event_loop_args);

    esp_event_loop_instance_t* loop;
    esp_err_t err = ESP_ERR_NO_MEM; // most likely error

    loop = calloc(1, sizeof(*loop));
    if (loop == NULL) {
        ESP_LOGE(TAG, "alloc for event loop failed");
        goto on_err;
    }

    loop->queue = xQueueCreate(event_loop_args->queue_size , sizeof(esp_event_post_instance_t));
    if (loop->queue == NULL) {
        ESP_LOGE(TAG, "create event loop queue failed");
        goto on_err;
    }

    loop->mutex = xSemaphoreCreateRecursiveMutex();
    if (loop->mutex == NULL) {
        ESP_LOGE(TAG, "create event loop mutex failed");
        goto on_err;
    }

#ifdef CONFIG_EVENT_LOOP_PROFILING
    loop->profiling_mutex = xSemaphoreCreateMutex();
    if (loop->profiling_mutex == NULL) {
        ESP_LOGE(TAG, "create event loop profiling mutex failed");
        goto on_err;
    }
#endif

    SLIST_INIT(&(loop->loop_handlers));
    SLIST_INIT(&(loop->event_bases));

    // Create the loop task if requested
    if (event_loop_args->task_name != NULL) {
        BaseType_t task_created = xTaskCreatePinnedToCore(esp_event_loop_run_task, event_loop_args->task_name,
                    event_loop_args->task_stack_size, (void*) loop,
                    event_loop_args->task_priority, &(loop->task), event_loop_args->task_core_id);

        if (task_created != pdPASS) {
            ESP_LOGE(TAG, "create task for loop failed");
            err = ESP_FAIL;
            goto on_err;
        }

        loop->name = event_loop_args->task_name;

        ESP_LOGD(TAG, "created task for loop %p", loop);
    } else {
        loop->name = "";
        loop->task = NULL;
    }

    loop->running_task = NULL;

#ifdef CONFIG_EVENT_LOOP_PROFILING
    portENTER_CRITICAL(&s_event_loops_spinlock);
    SLIST_INSERT_HEAD(&s_event_loops, loop, loop_entry);
    portEXIT_CRITICAL(&s_event_loops_spinlock);
#endif

    *event_loop = (esp_event_loop_handle_t) loop;

    ESP_LOGD(TAG, "created event loop %p", loop);

    return ESP_OK;

on_err:
    if(loop->queue != NULL) {
        vQueueDelete(loop->queue);
    }

    if(loop->mutex != NULL) {
        vSemaphoreDelete(loop->mutex);
    }

#ifdef CONFIG_EVENT_LOOP_PROFILING
    if(loop->profiling_mutex != NULL) {
        vSemaphoreDelete(loop->profiling_mutex);
    }
#endif

    free(loop);

    return err;
}

// On event lookup performance: The library implements the event list as a linked list, which results to O(n)
// lookup time. The test comparing this implementation to the O(lg n) performance of rbtrees
// (https://github.com/freebsd/freebsd/blob/master/sys/sys/tree.h)
// indicate that the difference is not that substantial, especially considering the additional
// pointers per node of rbtrees. Code for the rbtree implementation of the event loop library is archived
// in feature/esp_event_loop_library_rbtrees if needed.
esp_err_t esp_event_loop_run(esp_event_loop_handle_t event_loop, TickType_t ticks_to_run)
{
    assert(event_loop);

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;
    esp_event_post_instance_t post;
    TickType_t marker = xTaskGetTickCount();
    TickType_t end = 0;
    esp_event_handler_instance_t* temp;

#if( configUSE_16_BIT_TICKS == 1 )
    int32_t remaining_ticks = ticks_to_run;
#else
    int64_t remaining_ticks = ticks_to_run;
#endif

    while(xQueueReceive(loop->queue, &post, ticks_to_run) == pdTRUE) {
        esp_event_base_instance_t* base = NULL;
        esp_event_id_instance_t* event = NULL;

        // Reserve space for three possible matches: (1) the entry for handlers registered to all events in the loop, the
        // (2) entry matching events with a specified base and (3) the entry matching both base and id.
        #define LOOP_LEVEL_HANDLER 0
        #define BASE_LEVEL_HANDLER 1
        #define EVENT_LEVEL_HANDLER 2

        esp_event_handler_instances_t* handlers_list[EVENT_LEVEL_HANDLER + 1] = {0};

        // The event has already been unqueued, so ensure it gets executed.
        xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);
        
        loop->running_task = xTaskGetCurrentTaskHandle();
        
        handlers_list[LOOP_LEVEL_HANDLER] = &(loop->loop_handlers);

        base = loop_find_event_base_instance(loop, post.base);
        if (base) {
            event = event_base_instance_find_event_id_instance(base, post.id);
            handlers_list[BASE_LEVEL_HANDLER] = &(base->base_handlers);
            if (event) {
                handlers_list[EVENT_LEVEL_HANDLER] = &(event->handlers);
            }
        }

        bool exec = false;

        for (int i = LOOP_LEVEL_HANDLER; i <= EVENT_LEVEL_HANDLER; i++) {
            if (handlers_list[i] != NULL) {
                esp_event_handler_instance_t* it;
                
                SLIST_FOREACH_SAFE(it, handlers_list[i], handler_entry, temp) {
                    ESP_LOGD(TAG, "running post %s:%d with handler %p on loop %p", post.base, post.id, it->handler, event_loop);

#ifdef CONFIG_EVENT_LOOP_PROFILING
                    int64_t start, diff;
                    start = esp_timer_get_time();
#endif
                    // Execute the handler
                    (*(it->handler))(it->arg, post.base, post.id, post.data);

#ifdef CONFIG_EVENT_LOOP_PROFILING
                    diff = esp_timer_get_time() - start;

                    xSemaphoreTake(loop->profiling_mutex, portMAX_DELAY);

                    it->total_times_invoked++;
                    it->total_runtime += diff;

                    if (i == LOOP_LEVEL_HANDLER) {
                        loop->loop_handlers_invoked++;
                        loop->loop_handlers_runtime += diff;
                    } else if (i == BASE_LEVEL_HANDLER) {
                        base->base_handlers_invoked++;
                        base->base_handlers_runtime += diff;
                    } else {
                        event->handlers_invoked++;
                        event->handlers_runtime += diff;
                    }

                    loop->total_handlers_invoked++;
                    loop->total_handlers_runtime += diff;

                    xSemaphoreGive(loop->profiling_mutex);
#endif
                }
            }
            exec |= true;
        }

        post_instance_delete(&post);

        if (ticks_to_run != portMAX_DELAY) {
            end = xTaskGetTickCount();
            remaining_ticks -= end - marker;
            // If the ticks to run expired, return to the caller
            if (remaining_ticks <= 0) {
                xSemaphoreGiveRecursive(loop->mutex);
                break;
            } else {
                marker = end;
            }
        }

        loop->running_task = NULL;

        xSemaphoreGiveRecursive(loop->mutex);

        if (!exec) {
            // No handlers were registered, not even loop/base level handlers
            ESP_LOGW(TAG, "no handlers have been registered for event %s:%d posted to loop %p", post.base, post.id, event_loop);
        }
    }

    return ESP_OK;
}

esp_err_t esp_event_loop_delete(esp_event_loop_handle_t event_loop)
{
    assert(event_loop);

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;
    SemaphoreHandle_t loop_mutex = loop->mutex;
#ifdef CONFIG_EVENT_LOOP_PROFILING
    SemaphoreHandle_t loop_profiling_mutex = loop->profiling_mutex;
#endif

    xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

#ifdef CONFIG_EVENT_LOOP_PROFILING
    xSemaphoreTakeRecursive(loop->profiling_mutex, portMAX_DELAY);
    portENTER_CRITICAL(&s_event_loops_spinlock);
    SLIST_REMOVE(&s_event_loops, loop, esp_event_loop_instance, loop_entry);
    portEXIT_CRITICAL(&s_event_loops_spinlock);
#endif

    // Delete the task if it was created
    if (loop->task != NULL) {
        vTaskDelete(loop->task);
    }

    // Remove all registered events in the loop
    handler_instances_remove_all(&(loop->loop_handlers));
    loop_remove_all_event_base_instance(loop);

    // Drop existing posts on the queue
    esp_event_post_instance_t post;
    while(xQueueReceive(loop->queue, &post, 0) == pdTRUE) {
        free(post.data);
    }

    // Cleanup loop
    vQueueDelete(loop->queue);
    free(loop);
    // Free loop mutex before deleting
    xSemaphoreGiveRecursive(loop_mutex);
#ifdef CONFIG_EVENT_LOOP_PROFILING
    xSemaphoreGiveRecursive(loop_profiling_mutex);
    vSemaphoreDelete(loop_profiling_mutex);
#endif
    vSemaphoreDelete(loop_mutex);

    ESP_LOGD(TAG, "deleted loop %p", (void*) event_loop);

    return ESP_OK;
}

esp_err_t esp_event_handler_register_with(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                        int32_t event_id, esp_event_handler_t event_handler, void* event_handler_arg)
{
    assert(event_loop);
    assert(event_handler);

    if (event_base == ESP_EVENT_ANY_BASE && event_id != ESP_EVENT_ANY_ID) {
        ESP_LOGE(TAG, "registering to any event base with specific id unsupported");
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    esp_event_base_instance_t* base = NULL;
    esp_event_id_instance_t* event = NULL;
    esp_event_handler_instance_t* handler = NULL;
    esp_event_handler_instances_t* handlers = NULL;

    bool base_created = false;
    bool event_created = false;

    if (event_base == ESP_EVENT_ANY_BASE) {
        event_base = esp_event_any_base;
    }

    xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

    if (event_base == esp_event_any_base && event_id == ESP_EVENT_ANY_ID) {
        // Add to the loop-level handlers
        handlers = &(loop->loop_handlers);
    } else {
        // If base instance does not exist, create one
        if ((base = loop_find_event_base_instance(loop, event_base)) == NULL) {
            base = event_base_instance_create(event_base);
            if (base == NULL) {
                xSemaphoreGiveRecursive(loop->mutex);
                return ESP_ERR_NO_MEM;
            }
            base_created = true;
        }
        // Add to the event base instance level handlers
        if (event_id == ESP_EVENT_ANY_ID) {
            handlers = &(base->base_handlers);
        } else {
            if (base_created ||
                (event = event_base_instance_find_event_id_instance(base, event_id)) == NULL) {
                event = event_id_instance_create(event_id);
                // If it does not exist, create one
                if (event == NULL) {
                    if (base_created) {
                        event_base_instance_delete(base);
                    }
                    xSemaphoreGiveRecursive(loop->mutex);
                    return ESP_ERR_NO_MEM;
                }
                event_created = true;
            }
            // Add to the event id instance level handlers
            handlers = &(event->handlers);
        }
    }

    // Add handler to the list
    if (base_created || event_created ||
        (handler = handler_instances_find(handlers, event_handler)) == NULL) {
        handler = handler_instance_create(event_handler, event_handler_arg);
        if (handler == NULL) {
            if (event_created) {
                event_id_instance_delete(event);
            }
            if (base_created) {
                event_base_instance_delete(base);
            }
            xSemaphoreGiveRecursive(loop->mutex);
            return ESP_ERR_NO_MEM;
        }
        handler_instances_add(handlers, handler);
        // If a new event base/ event id instance was created, add them to the appropriate list
        if (event_created) {
            event_base_instance_add_event_id_instance(base, event);
        }
        if (base_created) {
            loop_add_event_base_instance(loop, base);
        }
        ESP_LOGD(TAG, "registered handler %p for event %s:%d", event_handler, event_base, event_id);
    } else {
        handler->arg = event_handler_arg;
        ESP_LOGW(TAG, "handler %p for event %s:%d already registered, overwriting", event_handler, event_base, event_id);
    }

    xSemaphoreGiveRecursive(loop->mutex);

    return ESP_OK;
}


esp_err_t esp_event_handler_unregister_with(esp_event_loop_handle_t event_loop, esp_event_base_t event_base,
                                            int32_t event_id, esp_event_handler_t event_handler)
{
    assert(event_loop);
    assert(event_handler);

    if (event_base == ESP_EVENT_ANY_BASE && event_id != ESP_EVENT_ANY_ID) {
        ESP_LOGE(TAG, "unregistering to any event base with specific id unsupported");
        return ESP_FAIL;
    }

    if (event_base == ESP_EVENT_ANY_BASE) {
        event_base = esp_event_any_base;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    esp_event_handler_instance_t* handler = NULL;
    esp_event_handler_instances_t* handlers = find_handlers_list(loop, event_base, event_id);

    xSemaphoreTakeRecursive(loop->mutex, portMAX_DELAY);

    if (handlers != NULL &&
        (handler = handler_instances_find(handlers, event_handler)) != NULL) {
        handler_instances_remove(handlers, handler);
        ESP_LOGD(TAG, "unregistered handler %p from event %s:%d", event_handler, event_base, event_id);
    } else {
        ESP_LOGW(TAG, "handler %p for event %s:%d not registered, ignoring", event_handler, event_base, event_id);
    }

    xSemaphoreGiveRecursive(loop->mutex);

    return ESP_OK;
}


esp_err_t esp_event_post_to(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id,
                            void* event_data, size_t event_data_size, TickType_t ticks_to_wait)
{
    assert(event_loop);

    if (event_base == ESP_EVENT_ANY_BASE || event_id == ESP_EVENT_ANY_ID) {
        ESP_LOGE(TAG, "posting nonspecific event base or id unsupported");
        return ESP_ERR_INVALID_ARG;
    }

    esp_event_loop_instance_t* loop = (esp_event_loop_instance_t*) event_loop;

    esp_event_post_instance_t post;
    esp_err_t err = post_instance_create(event_base, event_id, event_data, event_data_size, &post);

    if (err != ESP_OK) {
        return err;
    }

    BaseType_t result = pdFALSE;

    // Find the task that currently executes the loop. It is safe to query loop->task since it is
    // not mutated since loop creation. ENSURE THIS REMAINS TRUE.
    if (loop->task == NULL) {
        // The loop has no dedicated task. Find out what task is currently running it.
        result = xSemaphoreTakeRecursive(loop->mutex, ticks_to_wait);

        if (result == pdTRUE) {
            if (loop->running_task != xTaskGetCurrentTaskHandle()) {
                xSemaphoreGiveRecursive(loop->mutex);
                result = xQueueSendToBack(loop->queue, &post, ticks_to_wait);
            } else {
                xSemaphoreGiveRecursive(loop->mutex);
                result = xQueueSendToBack(loop->queue, &post, 0);
            }
        }
    } else {
        // The loop has a dedicated task.
        if (loop->task != xTaskGetCurrentTaskHandle()) {
            result = xQueueSendToBack(loop->queue, &post, ticks_to_wait);
        } else {
            result = xQueueSendToBack(loop->queue, &post, 0);
        }
    }

    if (result != pdTRUE) {
        post_instance_delete(&post);

#ifdef CONFIG_EVENT_LOOP_PROFILING
        xSemaphoreTake(loop->profiling_mutex, portMAX_DELAY);
        loop->events_dropped++;
        xSemaphoreGive(loop->profiling_mutex);
#endif
        return ESP_ERR_TIMEOUT;
    }

#ifdef CONFIG_EVENT_LOOP_PROFILING
    xSemaphoreTake(loop->profiling_mutex, portMAX_DELAY);
    loop->events_recieved++;
    xSemaphoreGive(loop->profiling_mutex);
#endif

    ESP_LOGD(TAG, "posted %s:%d to loop %p", post.base, post.id, event_loop);

    return ESP_OK;
}

esp_err_t esp_event_dump(FILE* file)
{
#ifdef CONFIG_EVENT_LOOP_PROFILING
    assert(file);

    esp_event_loop_instance_t* loop_it;
    esp_event_base_instance_t* base_it;
    esp_event_id_instance_t* id_it;
    esp_event_handler_instance_t* handler_it;

    // Allocate memory for printing
    int sz = esp_event_dump_prepare();
    char* buf = calloc(sz, sizeof(char));
    char* dst = buf;

    // Print info to buffer
    portENTER_CRITICAL(&s_event_loops_spinlock);
    SLIST_FOREACH(loop_it, &s_event_loops, loop_entry) {
        PRINT_DUMP_INFO(dst, sz, LOOP_DUMP_FORMAT, loop_it, loop_it->name, loop_it->events_recieved,
                        loop_it->events_dropped, loop_it->total_handlers_invoked, loop_it->total_handlers_runtime);

        // Print loop-level handler
        PRINT_DUMP_INFO(dst, sz, esp_event_any_base, ESP_EVENT_ANY_ID, loop_it->loop_handlers_invoked,
                        loop_it->loop_handlers_runtime);
        SLIST_FOREACH(handler_it, &(loop_it->loop_handlers), handler_entry) {
            PRINT_DUMP_INFO(dst, sz, HANDLER_DUMP_FORMAT, handler_it->handler, handler_it->total_times_invoked,
                            handler_it->total_runtime);
        }

        SLIST_FOREACH(base_it, &(loop_it->event_bases), event_base_entry) {
            // Print base-level handler
            PRINT_DUMP_INFO(dst, sz, EVENT_DUMP_FORMAT, base_it->base, ESP_EVENT_ANY_ID,
                            base_it->base_handlers_invoked, base_it->base_handlers_runtime);
            SLIST_FOREACH(handler_it, &(base_it->base_handlers), handler_entry) {
                PRINT_DUMP_INFO(dst, sz, HANDLER_DUMP_FORMAT, handler_it->handler,
                                handler_it->total_times_invoked, handler_it->total_runtime);
            }

            // Print event-level handlers
            SLIST_FOREACH(id_it, &(base_it->event_ids), event_id_entry) {
                PRINT_DUMP_INFO(dst, sz, EVENT_DUMP_FORMAT, base_it->base, id_it->id,
                                id_it->handlers_invoked, id_it->handlers_runtime);

                SLIST_FOREACH(handler_it, &(id_it->handlers), handler_entry) {
                    PRINT_DUMP_INFO(dst, sz, HANDLER_DUMP_FORMAT, handler_it->handler,
                                    handler_it->total_times_invoked, handler_it->total_runtime);
                }
            }
        }
    }
    portEXIT_CRITICAL(&s_event_loops_spinlock);
    
    // Print the contents of the buffer to the file
    fprintf(file, buf);
    
    // Free the allocated buffer
    free(buf);
#endif
    return ESP_OK;
}
