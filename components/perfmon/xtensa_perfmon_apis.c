/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "xtensa_perfmon_apis.h"
#include "xtensa_perfmon_masks.h"

static const char *TAG = "perfmon";

esp_err_t xtensa_perfmon_exec(const xtensa_perfmon_config_t *config)
{
    esp_err_t result = ESP_OK;
    if (config->call_function == NULL) {
        ESP_LOGE(TAG, "Parameter call_function must be defined.");
        return ESP_ERR_INVALID_ARG;
    }
    if (config->callback == NULL) {
        ESP_LOGE(TAG, "Parameter callback must be defined.");
        return ESP_ERR_INVALID_ARG;
    }
    for (size_t n = 0; n < config->counters_size; n++) {
        uint32_t call_min = UINT32_MAX;
        uint32_t call_max = 0;
        float result_value = 0;

        for (size_t i = 0; i < config->repeat_count; i++) {
            // Set up cycle counter
            xtensa_perfmon_stop();
            int kernelcnt = 0;
            // if tracelevel used, then kernelcnt will be enabled
            if (config->tracelevel >=0) kernelcnt = 1;
            xtensa_perfmon_init(0, 0, 0xffff, kernelcnt, config->tracelevel);
            xtensa_perfmon_init(1, config->select_mask[n * 2 + 0], config->select_mask[n * 2 + 1], kernelcnt, config->tracelevel);
            xtensa_perfmon_start();
            config->call_function(config->call_params);
            xtensa_perfmon_stop();
            uint32_t p0 = xtensa_perfmon_value(0);
            uint32_t p1 = xtensa_perfmon_value(1);
            result_value += (float)p1 / config->repeat_count;
            if (p0 < call_min) {
                call_min = p0;
            }
            if (p0 > call_max) {
                call_max = p0;
            }
            ESP_LOGV(TAG, "p0 = %i, p1 = %i", p0, p1);
        }
        uint32_t call_diff = (call_max - call_min);

        if (call_diff > call_max * config->max_deviation) {
            return ESP_FAIL;
        } else {
            config->callback(config->callback_params, config->select_mask[n * 2 + 0], config->select_mask[n * 2 + 1], (uint32_t)result_value);
        }
    }
    return result;
}

void xtensa_perfmon_view_cb(void *params, uint32_t select, uint32_t mask, uint32_t value)
{
    FILE *handle;
    if (params != NULL) {
        handle = (FILE *)params;
    } else {
        handle = stdout;
    }
    for (int i = 0 ; xtensa_perfmon_select_table[i].select != -1; i++) {
        if (xtensa_perfmon_select_table[i].select == select) {
            fprintf(handle, "Value = %9i, select = %2i, mask = %04x.  %s.\n", value, select, mask, xtensa_perfmon_select_table[i].description);
        }
    }
    for (int i = 0 ; xtensa_perfmon_masks_table[i].select != -1; i++) {
        if ((xtensa_perfmon_masks_table[i].select == select) && (xtensa_perfmon_masks_table[i].mask & mask)) {
            fprintf(handle, "                  %s\n", xtensa_perfmon_masks_table[i].description);
        }
    }
}
