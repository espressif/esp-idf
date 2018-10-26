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

#ifndef ESP_EVENT_INTERNAL_H_
#define ESP_EVENT_INTERNAL_H_

#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Event handler
typedef struct esp_event_handler_instance {
    esp_event_handler_t handler;                                    /**< event handler function*/
    void* arg;                                                      /**< event handler argument */
#ifdef CONFIG_EVENT_LOOP_PROFILING
    uint32_t total_times_invoked;                                   /**< number of times this handler has been invoked */
    int64_t total_runtime;                                          /**< total runtime of this handler across all calls */
#endif
    SLIST_ENTRY(esp_event_handler_instance) handler_entry;          /**< next event handler in the list */
} esp_event_handler_instance_t;

typedef SLIST_HEAD(esp_event_handler_instances, esp_event_handler_instance) esp_event_handler_instances_t;

typedef struct esp_event_id_instance {
    int32_t id;
    esp_event_handler_instances_t handlers;                         /**< list of handlers to be executed when 
                                                                            this event is raised */
    SLIST_ENTRY(esp_event_id_instance) event_id_entry;              /**< pointer to the next event node on the linked list */
#ifdef CONFIG_EVENT_LOOP_PROFILING
    uint32_t handlers_invoked;                                      /**< total number of times the event has been 
                                                                            raised and processed in the loop */
    int64_t handlers_runtime;                                       /**< total time spent in executing handlers */
#endif
} esp_event_id_instance_t;

typedef SLIST_HEAD(esp_event_id_instances, esp_event_id_instance) esp_event_id_instances_t;

/// Event
typedef struct esp_event_base_instance {
    esp_event_base_t base;                                          /**< base identifier of the event */
    esp_event_handler_instances_t base_handlers;                    /**< event base level handlers, handlers for 
                                                                            all events with this base */
    esp_event_id_instances_t event_ids;                             /**< list of event ids with this base */
    SLIST_ENTRY(esp_event_base_instance) event_base_entry;          /**< pointer to the next event node on the linked list */
#ifdef CONFIG_EVENT_LOOP_PROFILING
    uint32_t base_handlers_invoked;                                 /**< total number of base-level handlers invoked */
    int64_t base_handlers_runtime;                                  /**< amount of time processing base-level handlers */
#endif
} esp_event_base_instance_t;

typedef SLIST_HEAD(esp_event_base_instances, esp_event_base_instance) esp_event_base_instances_t;

/// Event loop
typedef struct esp_event_loop_instance {
    const char* name;                                               /**< name of this event loop */
    QueueHandle_t queue;                                            /**< event queue */
    TaskHandle_t task;                                              /**< task that consumes the event queue */
    TaskHandle_t running_task;                                      /**< for loops with no dedicated task, the 
                                                                            task that consumes the queue */
    SemaphoreHandle_t mutex;                                        /**< mutex for updating the events linked list */
    esp_event_handler_instances_t loop_handlers;                    /**< loop level handlers, handlers for all events 
                                                                            registered in the loop */
    esp_event_base_instances_t event_bases;                         /**< events linked list head pointer */
#ifdef CONFIG_EVENT_LOOP_PROFILING
    uint32_t events_recieved;                                       /**< number of events successfully posted to the loop */
    uint32_t events_dropped;                                        /**< number of events dropped due to queue being full */
    uint32_t loop_handlers_invoked;                                 /**< total number of loop-level handlers invoked */
    int64_t loop_handlers_runtime;                                  /**< amount of time processing loop-level handlers */
    uint32_t total_handlers_invoked;                                /**< total number of handlers invoked */
    int64_t total_handlers_runtime;                                 /**< total amount of time dedicated to processing this loop */
    SLIST_ENTRY(esp_event_loop_instance) loop_entry;                /**< next event loop in the list */
    SemaphoreHandle_t profiling_mutex;
#endif
} esp_event_loop_instance_t;

/// Event posted to the event queue
typedef struct esp_event_post_instance {
    esp_event_base_t base;                                           /**< the event base */
    int32_t id;                                                      /**< the event id */
    void** data;                                                     /**< data associated with the event */
} esp_event_post_instance_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef ESP_EVENT_INTERNAL_H_