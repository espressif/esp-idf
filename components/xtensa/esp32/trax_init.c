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
#include "esp_err.h"
#include "esp_log.h"
#include "trax.h"
#include "soc/dport_reg.h"
#include "sdkconfig.h"

#define TRACEMEM_MUX_PROBLK0_APPBLK1    0
#define TRACEMEM_MUX_BLK0_ONLY          1
#define TRACEMEM_MUX_BLK1_ONLY          2
#define TRACEMEM_MUX_PROBLK1_APPBLK0    3

static const char* __attribute__((unused)) TAG = "trax";

int trax_enable(trax_ena_select_t which) 
{
#ifndef CONFIG_ESP32_TRAX
    ESP_LOGE(TAG, "Trax_enable called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
#ifndef CONFIG_ESP32_TRAX_TWOBANKS
    if (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP) return ESP_ERR_NO_MEM;
#endif
    if (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP) {
        DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, (which == TRAX_ENA_PRO_APP_SWAP)?TRACEMEM_MUX_PROBLK1_APPBLK0:TRACEMEM_MUX_PROBLK0_APPBLK1);
    } else {
        DPORT_WRITE_PERI_REG(DPORT_TRACEMEM_MUX_MODE_REG, TRACEMEM_MUX_BLK0_ONLY);
    }
    DPORT_WRITE_PERI_REG(DPORT_PRO_TRACEMEM_ENA_REG, (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP || which == TRAX_ENA_PRO));
    DPORT_WRITE_PERI_REG(DPORT_APP_TRACEMEM_ENA_REG, (which == TRAX_ENA_PRO_APP || which == TRAX_ENA_PRO_APP_SWAP || which == TRAX_ENA_APP));
    return ESP_OK;
}
