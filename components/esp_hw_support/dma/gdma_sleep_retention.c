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
#if CONFIG_GDMA_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "esp_check.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/esp_regdma.h"

static const char *TAG = "gdma";

typedef struct {
    int group_id;
    int pair_id;
} gdma_channel_retention_arg_t;

static esp_err_t sleep_gdma_channel_retention_init(void *arg)
{
    gdma_channel_retention_arg_t *parg = (gdma_channel_retention_arg_t *)arg;
    int group_id = parg->group_id;
    int pair_id = parg->pair_id;

    sleep_retention_module_t module = gdma_chx_regs_retention[group_id][pair_id].module_id;
    esp_err_t err = sleep_retention_entries_create(gdma_chx_regs_retention[group_id][pair_id].link_list, gdma_chx_regs_retention[group_id][pair_id].link_num, REGDMA_LINK_PRI_GDMA, module);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "GDMA pair (%d, %d) retention initialization", group_id, pair_id);
    }

    ESP_RETURN_ON_ERROR(err, TAG, "Failed to create sleep retention linked list for GDMA pair (%d, %d) retention", group_id, pair_id);
    return err;
}

esp_err_t gdma_sleep_retention_init(int group_id, int pair_id)
{
    gdma_channel_retention_arg_t arg = { .group_id = group_id, .pair_id = pair_id };
    sleep_retention_module_init_param_t init_param = {
        .cbs = { .create = { .handle = sleep_gdma_channel_retention_init, .arg = &arg } },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    sleep_retention_module_t module = gdma_chx_regs_retention[group_id][pair_id].module_id;
    esp_err_t err = sleep_retention_module_init(module, &init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(module);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Failed to allocate sleep retention linked list for GDMA retention");
        }
    }
    return err;
}

esp_err_t gdma_sleep_retention_deinit(int group_id, int pair_id)
{
    sleep_retention_module_t module = gdma_chx_regs_retention[group_id][pair_id].module_id;
    esp_err_t err = sleep_retention_module_free(module);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "GDMA pair (%d, %d) retention destroy failed", group_id, pair_id);
    }
    err = sleep_retention_module_deinit(module);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "GDMA pair (%d, %d) retention deinit failed", group_id, pair_id);
    }
    return err;
}
