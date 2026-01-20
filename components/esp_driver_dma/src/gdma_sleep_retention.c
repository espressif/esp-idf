/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/lock.h>
#include "gdma_priv.h"

// Note!: the sleep retention APIs are using OS locks, so here we have to use a lock rather than a light-weight critical section
static _lock_t gdma_sleep_retention_lock;
static uint8_t pair_ref_counts[GDMA_LL_GET(INST_NUM)][GDMA_LL_GET(PAIRS_PER_INST)];

ESP_LOG_ATTR_TAG(TAG, "gdma");

static esp_err_t sleep_gdma_channel_retention_init(void *arg)
{
    gdma_pair_t *pair = (gdma_pair_t *)arg;
    int group_id = pair->group->group_id;
    int pair_id = pair->pair_id;

    sleep_retention_module_t module = gdma_chx_regs_retention[group_id][pair_id].module_id;
    esp_err_t err = sleep_retention_entries_create(gdma_chx_regs_retention[group_id][pair_id].link_list,
                                                   gdma_chx_regs_retention[group_id][pair_id].link_num,
                                                   REGDMA_LINK_PRI_GDMA, module);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "retention link created for pair (%d, %d)", group_id, pair_id);
    } else {
        ESP_LOGE(TAG, "failed to create retention link for pair (%d, %d)", group_id, pair_id);
    }
    return err;
}

void gdma_acquire_sleep_retention(gdma_pair_t* pair)
{
    int group_id = pair->group->group_id;
    int pair_id = pair->pair_id;
    sleep_retention_module_init_param_t init_param = {
        .cbs = { .create = { .handle = sleep_gdma_channel_retention_init, .arg = pair } },
        .depends = RETENTION_MODULE_BITMAP_INIT(CLOCK_SYSTEM)
    };
    sleep_retention_module_t module = gdma_chx_regs_retention[group_id][pair_id].module_id;

    _lock_acquire(&gdma_sleep_retention_lock);
    // First time acquiring this pair, initialize the module
    if (pair_ref_counts[group_id][pair_id] == 0) {
        esp_err_t err = sleep_retention_module_init(module, &init_param);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "init retention module failed for pair (%d, %d), power domain may be turned off during sleep", group_id, pair_id);
        } else {
            err = sleep_retention_module_allocate(module);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "fail to allocate retention link list for pair (%d, %d)", group_id, pair_id);
                // don't call sleep_retention_module_deinit here, otherwise GDMA peripheral may be powered off during sleep
            }
        }
    }
    pair_ref_counts[group_id][pair_id]++;
    _lock_release(&gdma_sleep_retention_lock);
}

void gdma_release_sleep_retention(gdma_pair_t* pair)
{
    int group_id = pair->group->group_id;
    int pair_id = pair->pair_id;
    sleep_retention_module_t module = gdma_chx_regs_retention[group_id][pair_id].module_id;

    _lock_acquire(&gdma_sleep_retention_lock);
    pair_ref_counts[group_id][pair_id]--;
    // Last reference, free the module
    if (pair_ref_counts[group_id][pair_id] == 0) {
        esp_err_t err = sleep_retention_module_free(module);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "fail to free the retention link list for pair (%d, %d)", group_id, pair_id);
        }
        err = sleep_retention_module_deinit(module);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "fail to deinit the retention module for pair (%d, %d)", group_id, pair_id);
        }
    }
    _lock_release(&gdma_sleep_retention_lock);
}
