/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_vfs_eventfd.h"
#include <sys/eventfd.h>

esp_err_t esp_vfs_eventfd_register(const esp_vfs_eventfd_config_t *config)
{
    (void)config;
    return ESP_OK;
}

esp_err_t esp_vfs_eventfd_unregister(void)
{
    return ESP_OK;
}
