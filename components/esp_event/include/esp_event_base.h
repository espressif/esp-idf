/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Defines for declaring and defining event base
#define ESP_EVENT_DECLARE_BASE(id) extern esp_event_base_t const id
#define ESP_EVENT_DEFINE_BASE(id) esp_event_base_t const id = #id

// Event loop library types
typedef const char*  esp_event_base_t; /**< unique pointer to a subsystem that exposes events */
typedef void*        esp_event_loop_handle_t; /**< a number that identifies an event with respect to a base */
typedef void         (*esp_event_handler_t)(void* event_handler_arg,
                                        esp_event_base_t event_base,
                                        int32_t event_id,
                                        void* event_data); /**< function called when an event is posted to the queue */
typedef void*        esp_event_handler_instance_t; /**< context identifying an instance of a registered event handler */

// Defines for registering/unregistering event handlers
#define ESP_EVENT_ANY_BASE     NULL             /**< register handler for any event base */
#define ESP_EVENT_ANY_ID       -1               /**< register handler for any event id */

#ifdef __cplusplus
}
#endif
