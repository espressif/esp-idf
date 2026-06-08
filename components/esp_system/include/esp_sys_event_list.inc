/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file esp_sys_event_list.inc
 * @brief Master table of all system events. Single source of truth.
 *
 * This file is included multiple times with different definitions of
 * ESP_SYS_EVENT_DEF() to generate the enum, linker symbol externs, and
 * the trigger dispatch table from one declaration each.
 *
 * To add a new event "FOO":
 *   1. Add `#define _ESP_SYS_EVENT_SECTION_FOO esysev_foo` below (within
 *      the include guard). Keep the section name short so it fits within
 *      the Mach-O 16-character section-name limit.
 *   2. Add `ESP_SYS_EVENT_DEF(FOO)` to the table at the bottom.
 *   3. Add the corresponding entries to linker.lf and ld/linux/sections.ld
 *      for `esysev_foo`.
 *   4. Optionally add a registration macro in the relevant private domain
 *      header using `_ESP_SYS_EVENT_REGISTER(FOO, prio, fn)`, or a
 *      public event-specific wrapper that does not expose the generic
 *      framework header.
 *
 * @note Enum values are assigned implicitly in the order below. Do not
 *       reorder unless you know nothing depends on a specific numeric
 *       value (no IDF code does today; values are not persisted).
 */

#ifndef ESP_SYS_EVENT_SECTIONS_DEFINED
#define ESP_SYS_EVENT_SECTIONS_DEFINED
#define _ESP_SYS_EVENT_SECTION_SHUTDOWN                     esysev_shdn
#define _ESP_SYS_EVENT_SECTION_SYSTEM_INIT_CORE             esysev_initc
#define _ESP_SYS_EVENT_SECTION_SYSTEM_INIT_SECONDARY        esysev_inits
#endif

/* ESP_SYS_EVENT_DEF(ENUM_SUFFIX) */
ESP_SYS_EVENT_DEF(SHUTDOWN)
ESP_SYS_EVENT_DEF(SYSTEM_INIT_CORE)
ESP_SYS_EVENT_DEF(SYSTEM_INIT_SECONDARY)
