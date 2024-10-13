/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include "esp_log_level.h"
#include "esp_attr.h"
#include "sdkconfig.h"

#if CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
esp_log_level_t esp_log_default_level = CONFIG_LOG_DEFAULT_LEVEL;
#endif

#ifdef CONFIG_LOG_MASTER_LEVEL
static esp_log_level_t s_master_log_level = CONFIG_LOG_DEFAULT_LEVEL;
#endif

void esp_log_set_default_level(esp_log_level_t level)
{
#if CONFIG_LOG_DYNAMIC_LEVEL_CONTROL
    esp_log_default_level = level;
#else
    (void)level;
#endif
}

#ifdef CONFIG_LOG_MASTER_LEVEL
esp_log_level_t esp_log_get_level_master(void)
{
    return s_master_log_level;
}

void esp_log_set_level_master(esp_log_level_t level)
{
    s_master_log_level = level;
}
#endif // CONFIG_LOG_MASTER_LEVEL
