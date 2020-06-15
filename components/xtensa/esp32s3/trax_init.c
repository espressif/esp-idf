// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "trax.h"
#include "soc/sensitive_reg.h"
#include "sdkconfig.h"

#define TRACEMEM_MUX_BLK0_NUM                   19
#define TRACEMEM_MUX_BLK1_NUM                   20

static const char *__attribute__((unused)) TAG = "trax";

// IDF-1785
int trax_enable(trax_ena_select_t which)
{
#ifndef CONFIG_ESP32S3_TRAX
    ESP_LOGE(TAG, "Trax_enable called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
    if (which != TRAX_ENA_PRO) {
        return ESP_ERR_INVALID_ARG;
    }
    // REG_WRITE(DPORT_PMS_OCCUPY_3_REG, BIT(TRACEMEM_MUX_BLK1_NUM-4));
    return ESP_OK;
}
