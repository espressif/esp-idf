/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This module implements debug/trace stubs. The stub is a piece of special code which can invoked by OpenOCD
// Currently one stub is used for GCOV functionality
//

#include "eri.h"
#include "xtensa-debug-module.h"

#include "esp_log.h"
const static char *TAG = "esp_dbg_stubs";

#define ESP_DBG_STUBS_TRAX_REG              ERI_TRAX_TRIGGERPC

void esp_dbg_stubs_ll_init(void *stub_table)
{
    eri_write(ESP_DBG_STUBS_TRAX_REG, (uint32_t)stub_table);
    ESP_LOGV(TAG, "%s stubs %"PRIx32, __func__, eri_read(ESP_DBG_STUBS_TRAX_REG));
}
