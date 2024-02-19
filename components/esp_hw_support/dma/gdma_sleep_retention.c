/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/esp_regdma.h"

#include "sleep_gdma_retention_context.inc"

static const char *TAG = "gdma";

#define SLEEP_RETENTION_MODULE_GDMA_CH(group_id, pair_id) (SLEEP_RETENTION_MODULE_GDMA_CH0 << (SOC_GDMA_PAIRS_PER_GROUP_MAX * group_id) << pair_id)

static esp_err_t sleep_modem_gdma_channel_retention_init(sleep_retention_module_bitmap_t module)
{
    uint32_t id =  __builtin_ctz(module / SLEEP_RETENTION_MODULE_GDMA_CH0);
    esp_err_t err = sleep_retention_entries_create(gdma_chx_regs_retention[id].link_list, gdma_chx_regs_retention[id].link_num, REGDMA_LINK_PRI_7, module);
    if (err == ESP_OK) {
        int group_id = id / SOC_GDMA_PAIRS_PER_GROUP_MAX;
        int pair_id = id % SOC_GDMA_PAIRS_PER_GROUP_MAX;
        ESP_LOGI(TAG, "GDMA pair (%d, %d) retention initialization", group_id, pair_id);
    }
    return err;
}

esp_err_t gdma_sleep_retention_init(int group_id, int pair_id)
{
    esp_err_t err = ESP_OK;
    err = sleep_modem_gdma_channel_retention_init(SLEEP_RETENTION_MODULE_GDMA_CH(group_id, pair_id));
    ESP_RETURN_ON_ERROR(err, TAG, "Failed to create sleep retention linked list for GDMA pair (%d, %d) retention", group_id, pair_id);
    return err;
}

esp_err_t gdma_sleep_retention_deinit(int group_id, int pair_id)
{
    sleep_retention_entries_destroy(SLEEP_RETENTION_MODULE_GDMA_CH(group_id, pair_id));
    return ESP_OK;;
}
