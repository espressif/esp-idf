/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_timer.h"
#include "esp_core_dump_common.h"
#include "esp_private/esp_core_dump_extension.h"

#define CORE_DUMP_UPTIME_NOTE_NAME "ESP_UPTIME"
#define CORE_DUMP_UPTIME_NOTE_TYPE 681

static int64_t s_uptime_us;

void esp_core_dump_uptime_capture(void)
{
    s_uptime_us = esp_timer_get_time();
}

static void core_dump_uptime_write(core_dump_sink_t *sink)
{
    if (s_uptime_us > 0) {
        esp_core_dump_sink_add_note(sink, CORE_DUMP_UPTIME_NOTE_NAME, CORE_DUMP_UPTIME_NOTE_TYPE,
                                    &s_uptime_us, sizeof(s_uptime_us));
    }
}

ESP_COREDUMP_REGISTER_EXTRA(core_dump_uptime_write);
