/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/esp_sys_event_internal.h"
#include "freertos/FreeRTOS.h"
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>

// Linker symbols for static handler arrays.  On macOS these are resolved by
// the portable section helpers in esp_attr.h.  The extra _DECL_/_GET_*
// indirections force expansion of _ESP_SYS_EVENT_SECTION_<ID> before it is
// token-pasted by _SECTION_ATTR_SYMBOL_DECL_GENERIC / _SECTION_START / _SECTION_END.
#define _ESP_SYS_EVENT_DECL_EXTERN(SECTION) \
    _SECTION_ATTR_SYMBOL_DECL_GENERIC(esp_sys_event_handler_t, SECTION)
#define _ESP_SYS_EVENT_GET_START(SECTION) _SECTION_START(SECTION)
#define _ESP_SYS_EVENT_GET_END(SECTION)   _SECTION_END(SECTION)

#define ESP_SYS_EVENT_DEF(ID) \
    _ESP_SYS_EVENT_DECL_EXTERN(_ESP_SYS_EVENT_SECTION_##ID)
#include "esp_sys_event_list.inc"
#undef ESP_SYS_EVENT_DEF

IRAM_ATTR esp_sys_event_iter_t esp_sys_event_get_static_handlers(esp_sys_event_id_t id)
{
    switch (id) {
#define ESP_SYS_EVENT_DEF(ID) \
    case ESP_SYS_EVENT_##ID: \
        return (esp_sys_event_iter_t) { \
            .start = _ESP_SYS_EVENT_GET_START(_ESP_SYS_EVENT_SECTION_##ID), \
            .end   = _ESP_SYS_EVENT_GET_END(_ESP_SYS_EVENT_SECTION_##ID), \
        };
#include "esp_sys_event_list.inc"
#undef ESP_SYS_EVENT_DEF
    default:
        return (esp_sys_event_iter_t) {
            NULL, NULL
        };
    }
}

typedef STAILQ_HEAD(esp_sys_event_dynamic_list, esp_sys_event_dynamic_node) esp_sys_event_dynamic_list_t;

static esp_sys_event_dynamic_list_t s_dynamic_handlers[ESP_SYS_EVENT_MAX] = {
#define ESP_SYS_EVENT_DEF(ID) \
    [ESP_SYS_EVENT_##ID] = STAILQ_HEAD_INITIALIZER(s_dynamic_handlers[ESP_SYS_EVENT_##ID]),
#include "esp_sys_event_list.inc"
#undef ESP_SYS_EVENT_DEF
};

static portMUX_TYPE s_dynamic_lock = portMUX_INITIALIZER_UNLOCKED;

// Number of triggers currently iterating each dynamic list.  Bumped atomically
// at trigger entry/exit so register/unregister can know whether it is safe to
// physically free tombstoned nodes (no concurrent iteration).
static atomic_int s_triggers_active[ESP_SYS_EVENT_MAX];

// Physically remove every tombstoned (handler == NULL) node from the dynamic
// list and free it.  Caller must hold s_dynamic_lock and must have verified
// that s_triggers_active[id] == 0; otherwise a concurrent trigger could be
// dereferencing a node we are about to free.
static void free_dead_nodes_locked(esp_sys_event_dynamic_list_t *list)
{
    esp_sys_event_dynamic_node_t *node, *tmp;
    STAILQ_FOREACH_SAFE(node, list, next, tmp) {
        if (node->handler == NULL) {
            STAILQ_REMOVE(list, node, esp_sys_event_dynamic_node, next);
            free(node);
        }
    }
}

static esp_sys_event_dynamic_node_t *find_dynamic_node_locked(esp_sys_event_dynamic_list_t *list,
                                                              esp_sys_event_fn_t handler,
                                                              void *user_arg)
{
    esp_sys_event_dynamic_node_t *node;
    STAILQ_FOREACH(node, list, next) {
        if (node->handler && node->handler == handler && node->user_arg == user_arg) {
            return node;
        }
    }

    return NULL;
}

esp_err_t esp_sys_event_register(esp_sys_event_id_t id, esp_sys_event_fn_t handler, void *user_arg)
{
    if (id >= ESP_SYS_EVENT_MAX || handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_sys_event_dynamic_node_t *node = malloc(sizeof(*node));
    if (!node) {
        return ESP_ERR_NO_MEM;
    }

    node->handler = handler;
    node->user_arg = user_arg;

    esp_sys_event_dynamic_list_t *list = &s_dynamic_handlers[id];
    portENTER_CRITICAL(&s_dynamic_lock);
    if (atomic_load(&s_triggers_active[id]) == 0) {
        free_dead_nodes_locked(list);
    }
    if (find_dynamic_node_locked(list, handler, user_arg)) {
        portEXIT_CRITICAL(&s_dynamic_lock);
        free(node);
        return ESP_ERR_INVALID_STATE;
    }
    STAILQ_INSERT_TAIL(list, node, next);
    portEXIT_CRITICAL(&s_dynamic_lock);

    return ESP_OK;
}

esp_err_t esp_sys_event_unregister(esp_sys_event_id_t id, esp_sys_event_fn_t handler, void *user_arg)
{
    if (id >= ESP_SYS_EVENT_MAX || handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_sys_event_dynamic_list_t *list = &s_dynamic_handlers[id];
    portENTER_CRITICAL(&s_dynamic_lock);
    esp_sys_event_dynamic_node_t *node = find_dynamic_node_locked(list, handler, user_arg);
    if (node) {
        // Tombstone first so any in-flight trigger skips this node.
        node->handler = NULL;
        if (atomic_load(&s_triggers_active[id]) == 0) {
            free_dead_nodes_locked(list);
        }
        portEXIT_CRITICAL(&s_dynamic_lock);
        return ESP_OK;
    }
    portEXIT_CRITICAL(&s_dynamic_lock);

    return ESP_ERR_NOT_FOUND;
}

void esp_sys_event_trigger_dynamic(esp_sys_event_id_t id, void *ctx)
{
    if (id >= ESP_SYS_EVENT_MAX) {
        return;
    }

    esp_sys_event_dynamic_list_t *list = &s_dynamic_handlers[id];

    // Bracket the dispatch with an active-trigger counter so that
    // register/unregister know not to free nodes while we iterate. The counter
    // touch is atomic and ISR-safe; handler dispatch itself runs lock-free so
    // handlers are free to block (e.g. flushing UART at shutdown). Tombstoned
    // (handler == NULL) nodes are skipped.
    atomic_fetch_add(&s_triggers_active[id], 1);

    esp_sys_event_dynamic_node_t *node;
    STAILQ_FOREACH(node, list, next) {
        if (node->handler) {
            node->handler(node->user_arg, ctx);
        }
    }

    atomic_fetch_sub(&s_triggers_active[id], 1);
}

IRAM_ATTR void esp_sys_event_trigger_static(esp_sys_event_id_t id, void *ctx)
{
    if (id >= ESP_SYS_EVENT_MAX) {
        return;
    }

    const esp_sys_event_iter_t info = esp_sys_event_get_static_handlers(id);
    if (info.start == NULL || info.end == NULL) {
        return;
    }

    for (const esp_sys_event_handler_t *h = info.start; h < info.end; ++h) {
        h->handler(NULL, ctx);
    }
}

void esp_sys_event_trigger(esp_sys_event_id_t id, void *ctx)
{
    esp_sys_event_trigger_static(id, ctx);
    esp_sys_event_trigger_dynamic(id, ctx);
}

// Event names indexed by id, generated from esp_sys_event_list.inc.
static const char *const s_event_names[] = {
#define ESP_SYS_EVENT_DEF(ID) [ESP_SYS_EVENT_##ID] = #ID,
#include "esp_sys_event_list.inc"
#undef ESP_SYS_EVENT_DEF
};

esp_err_t esp_sys_event_handler_dump(esp_sys_event_id_t id, FILE *stream)
{
    if (id >= ESP_SYS_EVENT_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    if (stream == NULL) {
        return ESP_OK;
    }

    const esp_sys_event_iter_t info = esp_sys_event_get_static_handlers(id);

    fprintf(stream, "Event %s:\n", s_event_names[id]);

    if (info.start != NULL && info.end != NULL) {
        size_t i = 0;
        for (const esp_sys_event_handler_t *h = info.start; h < info.end; ++h, i++) {
            fprintf(stream, "  static[%u] %p\n", (unsigned)i, (void *)h->handler);
        }
    }

    esp_sys_event_dynamic_list_t *list = &s_dynamic_handlers[id];

    // Bump the active-trigger counter to keep register/unregister from
    // freeing nodes while we walk the dynamic list lock-free.
    atomic_fetch_add(&s_triggers_active[id], 1);

    size_t dyn_idx = 0;
    esp_sys_event_dynamic_node_t *node;
    STAILQ_FOREACH(node, list, next) {
        if (node->handler) {
            fprintf(stream, "  dynamic[%u] %p\n", (unsigned)dyn_idx++, (void *)node->handler);
        }
    }

    atomic_fetch_sub(&s_triggers_active[id], 1);

    return ESP_OK;
}
