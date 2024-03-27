/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "soc/gdma_periph.h"
#include "soc/soc_caps.h"

#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/esp_regdma.h"

#include "hal/gdma_ll.h"

static const char *TAG = "gdma";

esp_err_t gdma_sleep_retention_init(int group_id, int pair_id)
{
    sleep_retention_module_bitmap_t module = GDMA_CH_RETENTION_GET_MODULE_ID(group_id, pair_id);
    esp_err_t err = sleep_retention_entries_create(gdma_chx_regs_retention[group_id][pair_id].link_list, gdma_chx_regs_retention[group_id][pair_id].link_num, REGDMA_LINK_PRI_GDMA, module);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "GDMA pair (%d, %d) retention initialization", group_id, pair_id);
    }

    ESP_RETURN_ON_ERROR(err, TAG, "Failed to create sleep retention linked list for GDMA pair (%d, %d) retention", group_id, pair_id);
    return err;
}

esp_err_t gdma_sleep_retention_deinit(int group_id, int pair_id)
{
    esp_err_t err = ESP_OK;
    sleep_retention_entries_destroy(GDMA_CH_RETENTION_GET_MODULE_ID(group_id, pair_id));
    return err;
}
