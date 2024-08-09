/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * pthread port for Linux build
 */

#include "esp_pthread.h"
#include <string.h>

/**
 * @brief Creates a default pthread configuration based
 * on the values set via menuconfig.
 *
 * @return
 *      A default configuration structure.
 */
esp_pthread_cfg_t esp_pthread_get_default_config(void)
{
    esp_pthread_cfg_t cfg = {
        .stack_size = CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT,
        .prio = CONFIG_PTHREAD_TASK_PRIO_DEFAULT,
        .inherit_cfg = false,
        .thread_name = NULL,
        .pin_to_core = 0,
        .stack_alloc_caps = 0,
    };

    return cfg;
}

esp_err_t esp_pthread_set_cfg(const esp_pthread_cfg_t *cfg)
{
    return ESP_OK;
}

esp_err_t esp_pthread_get_cfg(esp_pthread_cfg_t *p)
{
    memset(p, 0, sizeof(*p));
    return ESP_ERR_NOT_FOUND;
}
