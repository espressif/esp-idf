/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_private/esp_sys_event.h"
#include "sys/queue.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * Event IDs, dynamic register/unregister, trigger functions, iteration,
 * and the dump helper. Internal to IDF; per-event public surface (if any)
 * is provided by separate event-specific headers or wrappers.
 */

/**
 * @brief System event identifier
 *
 * Auto-generated from esp_sys_event_list.inc. To add or remove events,
 * edit the .inc file — the enum, linker symbol externs, and dispatch
 * table all stay in sync automatically.
 */
typedef enum {
#define ESP_SYS_EVENT_DEF(ID) ESP_SYS_EVENT_##ID,
#include "esp_sys_event_list.inc"
#undef ESP_SYS_EVENT_DEF
    ESP_SYS_EVENT_MAX
} esp_sys_event_id_t;

/**
 * @brief Node structure for dynamically registered event handlers
 */
typedef struct esp_sys_event_dynamic_node {
    esp_sys_event_fn_t handler; /*!< Handler function pointer */
    void *user_arg;             /*!< Per-registration context */
    STAILQ_ENTRY(esp_sys_event_dynamic_node) next; /*!< Link to next node */
} esp_sys_event_dynamic_node_t;

/**
 * @brief Register a dynamic event handler
 *
 * Registers a handler function to be called when the specified event is
 * triggered. Dynamic handlers are called after static handlers.
 * The handler receives (user_arg, ctx) where ctx is the event context
 * passed at trigger time.
 *
 * @param id Event ID
 * @param handler Handler function to register
 * @param user_arg Per-registration context passed as first arg to handler
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if id is invalid or handler is NULL
 *      - ESP_ERR_INVALID_STATE if handler is already registered with same
 * user_arg
 *      - ESP_ERR_NO_MEM if memory allocation failed
 */
esp_err_t esp_sys_event_register(esp_sys_event_id_t id,
                                 esp_sys_event_fn_t handler, void *user_arg);

/**
 * @brief Unregister a dynamic event handler
 *
 * Removes a previously registered handler for the specified event.
 * Matches by both handler function pointer and user_arg.
 *
 * @param id Event ID
 * @param handler Handler function to unregister
 * @param user_arg The same user_arg that was used during registration
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if id is invalid or handler is NULL
 *      - ESP_ERR_NOT_FOUND if the handler was not found
 */
esp_err_t esp_sys_event_unregister(esp_sys_event_id_t id,
                                   esp_sys_event_fn_t handler, void *user_arg);

/**
 * @brief Trigger all dynamic handlers for an event
 *
 * Iterates over dynamically registered handlers for the event, calling each
 * with (user_arg, ctx). All handlers are called regardless of individual
 * return values.
 *
 * @param id Event ID
 * @param ctx Event context pointer passed to each handler
 */
void esp_sys_event_trigger_dynamic(esp_sys_event_id_t id, void *ctx);

/**
 * @brief Trigger all static (link-time) handlers for an event
 *
 * The dispatcher is placed in IRAM, but cache-disabled use is only safe for
 * events whose handler descriptor sections and registered handlers are also
 * placed in RAM/IRAM. Future events which need cache-disabled dispatch must
 * define their linker placement and registration macro accordingly.
 *
 * Iterates over statically registered handlers for the event, calling each
 * with (NULL, ctx). All handlers are called regardless of individual return
 * values.
 *
 * @param id Event ID
 * @param ctx Context pointer to pass to handlers
 */
void esp_sys_event_trigger_static(esp_sys_event_id_t id, void *ctx);

/**
 * @brief Trigger all handlers (static + dynamic) for an event
 *
 * Convenience wrapper that calls esp_sys_event_trigger_static() followed by
 * esp_sys_event_trigger_dynamic(). Both receive the same ctx.
 *
 * @param id Event ID
 * @param ctx Context pointer to pass to all handlers
 */
void esp_sys_event_trigger(esp_sys_event_id_t id, void *ctx);

/**
 * @brief Iterator bounds for static handler iteration
 */
typedef struct {
    const esp_sys_event_handler_t *start;
    const esp_sys_event_handler_t *end;
} esp_sys_event_iter_t;

/**
 * @brief Get static handler array bounds for an event
 *
 * Returns the start and end of the static handler array for the specified
 * event. This allows custom iteration logic (e.g., feeding watchdog between
 * handlers in panic context). Returns {NULL, NULL} for an invalid id.
 *
 * @param id Event ID
 * @return esp_sys_event_iter_t with start/end pointers ({NULL, NULL} on invalid
 * id)
 */
esp_sys_event_iter_t esp_sys_event_get_static_handlers(esp_sys_event_id_t id);

/**
 * @brief Iterate over all static handlers for an event
 *
 * @param h Variable name for the handler pointer
 * @param id Event ID (esp_sys_event_id_t)
 *
 * Example:
 * @code
 * ESP_SYS_EVENT_FOREACH(h, ESP_SYS_EVENT_SHUTDOWN) {
 *     h->handler(NULL, ctx);
 * }
 * @endcode
 */
#define ESP_SYS_EVENT_FOREACH(h, id)                                           \
  for (esp_sys_event_iter_t _iter = esp_sys_event_get_static_handlers(id),     \
                            *_once = &_iter;                                   \
       _once; _once = NULL)                                                    \
    for (const esp_sys_event_handler_t *h = _iter.start; h < _iter.end; ++h)

/**
 * @brief Dump every handler registered to the given event to a stream
 *
 * Prints each static handler and each dynamic handler (function pointer) for
 * the event. Output format is suitable for debugging.
 *
 * @param id Event ID
 * @param stream Stream to print to (e.g. stdout), or NULL to skip output
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if id is invalid
 */
esp_err_t esp_sys_event_handler_dump(esp_sys_event_id_t id, FILE *stream);

#ifdef __cplusplus
}
#endif
