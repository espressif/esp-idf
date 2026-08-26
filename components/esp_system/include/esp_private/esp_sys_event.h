/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_attr.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file esp_private/esp_sys_event.h
 * @brief Static-registration primitive for the private esp_sys_event framework
 *
 * This private header exposes the generic framework building blocks used by
 * internal per-event registration macros.
 *
 * User code should use the per-event public wrappers (for example,
 * esp_register_shutdown_handler and ESP_SHUTDOWN_HANDLER_REGISTER) rather than
 * registering against the framework directly.
 */

/**
 * @brief System event handler function signature
 *
 * - user_arg: per-registration context (NULL for static handlers)
 * - ctx: per-trigger event context passed at dispatch time
 */
typedef esp_err_t (*esp_sys_event_fn_t)(void *user_arg, void *ctx);

/**
 * @brief System event handler descriptor for static registration
 *
 * One instance is emitted per static registration into the event's linker
 * section by _ESP_SYS_EVENT_REGISTER.
 */
typedef struct {
    esp_sys_event_fn_t handler; /*!< Handler function pointer */
} esp_sys_event_handler_t;

/* Pull in the _ESP_SYS_EVENT_SECTION_<EVENT> lookup macros from the master
 * event table. ESP_SYS_EVENT_DEF is a no-op here -- we only want the section
 * name #defines on this pass. */
#define ESP_SYS_EVENT_DEF(ID)
#include "esp_sys_event_list.inc"
#undef ESP_SYS_EVENT_DEF

#define _ESP_SYS_EVENT_STRINGIFY_INNER(x) #x
#define _ESP_SYS_EVENT_STRINGIFY(x) _ESP_SYS_EVENT_STRINGIFY_INNER(x)

/**
 * @brief Register a static (link-time) handler for an event.
 *
 * Building block used by internal per-event wrappers (for example,
 * ESP_SYSTEM_INIT_CORE).
 *
 * The handler will be sorted by priority at link time and called with
 * (NULL, ctx) at trigger time.
 *
 * @param EVENT  Event token -- must match an entry in esp_sys_event_list.inc.
 *               Used to look up the linker section via
 *               _ESP_SYS_EVENT_SECTION_<EVENT> and to disambiguate the
 *               descriptor symbol so the same `fn` can register against
 *               different events from the same translation unit.
 * @param prio   Priority (lower runs first).
 * @param fn     Handler function name. The macro emits a forward
 *               declaration and opens a function definition; the body
 *               must follow immediately at the call site.
 */
#define _ESP_SYS_EVENT_REGISTER(EVENT, prio, fn, ...)                          \
  static esp_err_t __VA_ARGS__ fn(void *user_arg, void *ctx);                  \
  static _SECTION_ATTR_IMPL_GENERIC(                                           \
      _ESP_SYS_EVENT_STRINGIFY(_ESP_SYS_EVENT_SECTION_##EVENT), prio)          \
      esp_sys_event_handler_t _esysev_##EVENT##_##fn = {.handler = fn};        \
  static esp_err_t __VA_ARGS__ fn(void *user_arg, void *ctx)

#ifdef __cplusplus
}
#endif
