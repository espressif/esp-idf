// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ESP_EVENT_H_
#define ESP_EVENT_H_

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "esp_event_base.h"
#include "esp_event_legacy.h"

#ifdef __cplusplus
extern "C" {
#endif


/// Configuration for creating event loops
typedef struct {
    int32_t queue_size;                         /**< size of the event loop queue */
    const char* task_name;                      /**< name of the event loop task; if NULL, 
                                                        a dedicated task is not created for event loop*/
    UBaseType_t task_priority;                  /**< priority of the event loop task, ignored if task name is NULL */
    uint32_t task_stack_size;                   /**< stack size of the event loop task, ignored if task name is NULL */
    BaseType_t task_core_id;                    /**< core to which the event loop task is pinned to, 
                                                        ignored if task name is NULL */
} esp_event_loop_args_t;

/**
 * @brief Create a new event loop.
 *
 * @param[in] event_loop_args configuration structure for the event loop to create
 * @param[out] event_loop handle to the created event loop
 *
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
esp_err_t esp_event_loop_create(const esp_event_loop_args_t* event_loop_args, esp_event_loop_handle_t* event_loop);

/**
 * @brief Delete an existing event loop.
 *
 * @param[in] event_loop event loop to delete
 *
 * @return 
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_delete(esp_event_loop_handle_t event_loop);

/**
 * @brief Create default event loop
 * 
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - ESP_FAIL: Failed to create task loop
 *  - Others: Fail
 */
esp_err_t esp_event_loop_create_default();

/**
 * @brief Delete the default event loop
 * 
 * @return 
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_delete_default();

/**
 * @brief Dispatch events posted to an event loop.
 *
 * This function is used to dispatch events posted to a loop with no dedicated task, i.e task name was set to NULL 
 * in event_loop_args argument during loop creation. This function includes an argument to limit the amount of time 
 * it runs, returning control to the caller when that time expires (or some time afterwards). There is no guarantee 
 * that a call to this function will exit at exactly the time of expiry. There is also no guarantee that events have 
 * been dispatched during the call, as the function might have spent all of the alloted time waiting on the event queue.
 * Once an event has been unqueued, however, it is guaranteed to be dispatched. This guarantee contributes to not being 
 * able to exit exactly at time of expiry as (1) blocking on internal mutexes is necessary for dispatching the unqueued 
 * event, and (2) during  dispatch of the unqueued event there is no way to control the time occupied by handler code 
 * execution. The guaranteed time of exit is therefore the alloted time + amount of time required to dispatch
 * the last unqueued event.
 *
 * In cases where waiting on the queue times out, ESP_OK is returned and not ESP_ERR_TIMEOUT, since it is 
 * normal behavior.
 *
 * @param[in] event_loop event loop to dispatch posted events from
 * @param[in] ticks_to_run number of ticks to run the loop
 *
 * @note encountering an unknown event that has been posted to the loop will only generate a warning, not an error.
 *
 * @return 
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_run(esp_event_loop_handle_t event_loop, TickType_t ticks_to_run);

/**
 * @brief Register an event handler to the system event loop.
 *
 * This function can be used to register a handler for either: (1) specific events,
 * (2) all events of a certain event base, or (3) all events known by the system event loop.
 *
 *  - specific events: specify exact event_base and event_id
 *  - all events of a certain base: specify exact event_base and use ESP_EVENT_ANY_ID as the event_id
 *  - all events known by the loop: use ESP_EVENT_ANY_BASE for event_base and ESP_EVENT_ANY_ID as the event_id
 *
 * Registering multiple handlers to events is possible. Registering a single handler to multiple events is 
 * also possible. However, registering the same handler to the same event multiple times would cause the 
 * previous registrations to be overwritten.
 *
 * @param[in] event_base the base id of the event to register the handler for
 * @param[in] event_id the id of the event to register the handler for
 * @param[in] event_handler the handler function which gets called when the event is dispatched
 * @param[in] event_handler_arg data, aside from event data, that is passed to the handler when it is called
 *
 * @note the event loop library does not maintain a copy of event_handler_arg, therefore the user should 
 * ensure that event_handler_arg still points to a valid location by the time the handler gets called
 *
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_register(esp_event_base_t event_base, 
                                        int32_t event_id, 
                                        esp_event_handler_t event_handler, 
                                        void* event_handler_arg);

/**
 * @brief Register an event handler to a specific loop.
 *
 * This function behaves in the same manner as esp_event_handler_register, except the additional 
 * specification of the event loop to register the handler to.
 *
 * @param[in] event_loop the event loop to register this handler function to
 * @param[in] event_base the base id of the event to register the handler for
 * @param[in] event_id the id of the event to register the handler for
 * @param[in] event_handler the handler function which gets called when the event is dispatched
 * @param[in] event_handler_arg data, aside from event data, that is passed to the handler when it is called
 *
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for the handler
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_register_with(esp_event_loop_handle_t event_loop, 
                                            esp_event_base_t event_base, 
                                            int32_t event_id, 
                                            esp_event_handler_t event_handler, 
                                            void* event_handler_arg);

/**
 * @brief Unregister a handler with the system event loop.
 *
 * This function can be used to unregister a handler so that it no longer gets called during dispatch.
 * Handlers can be unregistered for either: (1) specific events, (2) all events of a certain event base, 
 * or (3) all events known by the system event loop
 *
 *  - specific events: specify exact event_base and event_id
 *  - all events of a certain base: specify exact event_base and use ESP_EVENT_ANY_ID as the event_id
 *  - all events known by the loop: use ESP_EVENT_ANY_BASE for event_base and ESP_EVENT_ANY_ID as the event_id
 *
 * This function ignores unregistration of handlers that has not been previously registered.
 *
 * @param[in] event_base the base of the event with which to unregister the handler
 * @param[in] event_id the id of the event with which to unregister the handler
 * @param[in] event_handler the handler to unregister
 *
 * @return ESP_OK success
 * @return ESP_ERR_INVALIG_ARG invalid combination of event base and event id
 * @return others fail
 */
esp_err_t esp_event_handler_unregister(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler);

/**
 * @brief Unregister a handler with the system event loop.
 *
 * This function behaves in the same manner as esp_event_handler_unregister, except the additional specification of 
 * the event loop to unregister the handler with.
 *
 * @param[in] event_loop the event loop with which to unregister this handler function
 * @param[in] event_base the base of the event with which to unregister the handler
 * @param[in] event_id the id of the event with which to unregister the handler
 * @param[in] event_handler the handler to unregister
 *
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_handler_unregister_with(esp_event_loop_handle_t event_loop, 
                                            esp_event_base_t event_base, 
                                            int32_t event_id, 
                                            esp_event_handler_t event_handler);

/**
 * @brief Posts an event to the system default event loop. The event loop library keeps a copy of event_data and manages 
 * the copy's lifetime automatically (allocation + deletion); this ensures that the data the 
 * handler recieves is always valid.
 * 
 * @param[in] event_base the event base that identifies the event
 * @param[in] event_id the the event id that identifies the event
 * @param[in] event_data the data, specific to the event occurence, that gets passed to the handler
 * @param[in] event_data_size the size of the event data
 * @param[in] ticks_to_wait number of ticks to block on a full event queue
 *
 * @note posting events from an ISR is not supported
 *
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_TIMEOUT: Time to wait for event queue to unblock expired
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_post(esp_event_base_t event_base, 
                            int32_t event_id, 
                            void* event_data, 
                            size_t event_data_size, 
                            TickType_t ticks_to_wait);

/**
 * @brief Posts an event to the specified event loop. The event loop library keeps a copy of event_data and manages 
 * the copy's lifetime automatically (allocation + deletion); this ensures that the data the 
 * handler recieves is always valid.
 *
 * This function behaves in the same manner as esp_event_post_to, except the additional specification of the event loop
 * to post the event to.
 *
 * @param[in] event_loop the event loop to post to
 * @param[in] event_base the event base that identifies the event
 * @param[in] event_id the the event id that identifies the event
 * @param[in] event_data the data, specific to the event occurence, that gets passed to the handler
 * @param[in] event_data_size the size of the event data
 * @param[in] ticks_to_wait number of ticks to block on a full event queue
 *
 * @note posting events from an ISR is not supported
 * 
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_TIMEOUT: Time to wait for event queue to unblock expired
 *  - ESP_ERR_INVALIG_ARG: Invalid combination of event base and event id
 *  - Others: Fail
 */
esp_err_t esp_event_post_to(esp_event_loop_handle_t event_loop, 
                            esp_event_base_t event_base, 
                            int32_t event_id, 
                            void* event_data, 
                            size_t event_data_size, 
                            TickType_t ticks_to_wait);

/**
 * @brief Dumps statistics of all event loops.
 *
 * Dumps event loop info in the format:
 * 
 @verbatim
       event loop
           event
               handler
               handler
           event
               handler
               handler
       event loop
           event
               handler
               ...
           ...
       ...
 
  where:
 
   event loop
       format: address,name rx:total_recieved dr:total_dropped inv:total_number_of_invocations run:total_runtime
       where:
           address - memory address of the event loop
           name - name of the event loop
           total_recieved - number of successfully posted events
           total_number_of_invocations - total number of handler invocations performed so far
           total_runtime - total runtime of all invocations so far
 
   event
       format: base:id proc:total_processed run:total_runtime
       where:
           base - event base
           id - event id
           total_processed - number of instances of this event that has been processed
           total_runtime - total amount of time in microseconds used for invoking handlers of this event
 
   handler
       format: address inv:total_invoked run:total_runtime
       where:
           address - address of the handler function
           total_invoked - number of times this handler has been invoked
           total_runtime - total amount of time used for invoking this handler
 
 @endverbatim
 *
 * @param[in] file the file stream to output to
 *
 * @note this function is a noop when CONFIG_EVENT_LOOP_PROFILING is disabled
 *
 * @return 
 *  - ESP_OK: Success
 *  - ESP_ERR_NO_MEM: Cannot allocate memory for event loops list
 *  - Others: Fail
 */
esp_err_t esp_event_dump(FILE* file);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef ESP_EVENT_H_