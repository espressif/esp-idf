/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tmap_central.h"

int mcp_server_init(void)
{
    int err;

    err = esp_ble_audio_media_proxy_pl_init();
    if (err) {
        ESP_LOGE(TAG, "Failed to init media proxy pl (err %d)", err);
        return err;
    }

    ESP_LOGI(TAG, "MCP server initialized");

    return 0;
}
