
/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"
#include "xt_trax.h"
#include "esp_private/trax.h"
#include "hal/trace_ll.h"
#include "soc/dport_reg.h"
#include "soc/tracemem_config.h"
#include "sdkconfig.h"

// Utility functions for enabling TRAX in early startup (hence the use
// of ESP_EARLY_LOGX) in Xtensa targets.

#if defined(CONFIG_ESP32_TRAX) || defined(CONFIG_ESP32S2_TRAX) || defined(CONFIG_ESP32S3_TRAX)
#define WITH_TRAX 1
#endif

static const char* __attribute__((unused)) TAG = "trax";

int trax_enable(trax_ena_select_t which)
{
#if !WITH_TRAX
    ESP_EARLY_LOGE(TAG, "trax_enable called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
#if CONFIG_IDF_TARGET_ESP32
#ifndef CONFIG_ESP32_TRAX_TWOBANKS
    if (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP) return ESP_ERR_NO_MEM;
#endif
    if (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP) {
        trace_ll_set_mode((which == TRAX_ENA_PRO_APP_SWAP)?TRACEMEM_MUX_PROBLK1_APPBLK0:TRACEMEM_MUX_PROBLK0_APPBLK1);
    } else {
        trace_ll_set_mode(TRACEMEM_MUX_BLK0_ONLY);
    }
    trace_ll_mem_enable(0, (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP || which == TRAX_ENA_PRO));
    trace_ll_mem_enable(1, (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP || which == TRAX_ENA_APP));
    return ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32S2
    if (which != TRAX_ENA_PRO) {
        return ESP_ERR_INVALID_ARG;
    }
    trace_ll_set_mem_block(TRACEMEM_MUX_BLK1_NUM);
    return ESP_OK;
#elif CONFIG_IDF_TARGET_ESP32S3
    if (which == TRAX_ENA_PRO) {
        trace_ll_set_mem_block(0, TRACEMEM_MUX_BLK0_NUM);
    }
    else if (which == TRAX_ENA_APP) {
        trace_ll_set_mem_block(1, TRACEMEM_MUX_BLK0_NUM);
    }
#ifdef CONFIG_ESP32S3_TRAX_TWOBANKS
    else if (which == TRAX_ENA_PRO_APP) {
        trace_ll_set_mem_block(0, TRACEMEM_MUX_BLK0_NUM);
        trace_ll_set_mem_block(1, TRACEMEM_MUX_BLK1_NUM);
    }
    else if (which == TRAX_ENA_PRO_APP_SWAP) {
        trace_ll_set_mem_block(1, TRACEMEM_MUX_BLK0_NUM);
        trace_ll_set_mem_block(0, TRACEMEM_MUX_BLK1_NUM);
    }
#endif
    else {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
#endif
}

int trax_start_trace(trax_downcount_unit_t units_until_stop)
{
#if !WITH_TRAX
    ESP_EARLY_LOGE(TAG, "trax_start_trace called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
    if (xt_trax_trace_is_active()) {
        ESP_EARLY_LOGI(TAG, "Stopping active trace first.");
        //Trace is active. Stop trace.
        xt_trax_trigger_traceend_after_delay(0);
    }
    if (units_until_stop == TRAX_DOWNCOUNT_INSTRUCTIONS) {
        xt_trax_start_trace_instructions();
    } else {
        xt_trax_start_trace_words();
    }
    return ESP_OK;
}

int trax_trigger_traceend_after_delay(int delay)
{
#if !WITH_TRAX
    ESP_EARLY_LOGE(TAG, "trax_trigger_traceend_after_delay called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
    xt_trax_trigger_traceend_after_delay(delay);
    return ESP_OK;
}
