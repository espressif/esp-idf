/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdlib.h>

#include <zephyr/logging/log.h>

#include "esp_log.h"

#include "utils/assert.h"

#define TAG "ISO_ASSERT"

/* Always logged (no LOG_LEVEL gate) — this is the last message before abort, and
 * the user needs the context to diagnose. */
void bt_le_assert(const char *tag, size_t info,
                  const char *file, int line, const char *func)
{
    esp_log_write(ESP_LOG_ERROR, TAG,
                  BT_ISO_LOG_COLOR_E
                  "E (%lu) %s: Assert[%s][info=%u][%s:%d][%s]"
                  BT_ISO_LOG_RESET_COLOR "\n",
                  esp_log_timestamp(), TAG,
                  tag, (unsigned)info, file, line, func);
    abort();
}
