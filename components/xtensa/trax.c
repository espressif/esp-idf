// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include "soc/dport_reg.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "eri.h"
#include "xtensa-debug-module.h"
#include "trax.h"
#include "esp_log.h"

#define TRACEMEM_MUX_PROBLK0_APPBLK1    0
#define TRACEMEM_MUX_BLK0_ONLY          1
#define TRACEMEM_MUX_BLK1_ONLY          2
#define TRACEMEM_MUX_PROBLK1_APPBLK0    3

static const char* TAG = "trax";

int trax_enable(trax_ena_select_t which) 
{
#if !CONFIG_ESP32_TRAX
    ESP_LOGE(TAG, "Trax_enable called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
#if !CONFIG_ESP32_TRAX_TWOBANKS
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


int trax_start_trace(trax_downcount_unit_t units_until_stop) 
{
#if !CONFIG_ESP32_TRAX
    ESP_LOGE(TAG, "Trax_start_trace called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
    uint32_t v;
    if (eri_read(ERI_TRAX_TRAXSTAT)&TRAXSTAT_TRACT) {
        ESP_LOGI(TAG, "Stopping active trace first.");
        //Trace is active. Stop trace.
        eri_write(ERI_TRAX_DELAYCNT, 0);
        eri_write(ERI_TRAX_TRAXCTRL, eri_read(ERI_TRAX_TRAXCTRL)|TRAXCTRL_TRSTP);
        //ToDo: This will probably trigger a trace done interrupt. ToDo: Fix, but how? -JD
        eri_write(ERI_TRAX_TRAXCTRL, 0);
    }
    eri_write(ERI_TRAX_PCMATCHCTRL, 31); //do not stop at any pc match
    v=TRAXCTRL_TREN | TRAXCTRL_TMEN | TRAXCTRL_PTOWS | (1<<TRAXCTRL_SMPER_SHIFT);
    if (units_until_stop == TRAX_DOWNCOUNT_INSTRUCTIONS) v|=TRAXCTRL_CNTU;
    //Enable trace. This trace has no stop condition and will just keep on running.
    eri_write(ERI_TRAX_TRAXCTRL, v);
    return ESP_OK;
}


int trax_trigger_traceend_after_delay(int delay) 
{
#if !CONFIG_ESP32_TRAX
    ESP_LOGE(TAG, "Trax_trigger_traceend_after_delay called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
    eri_write(ERI_TRAX_DELAYCNT, delay);
    eri_write(ERI_TRAX_TRAXCTRL, eri_read(ERI_TRAX_TRAXCTRL)|TRAXCTRL_TRSTP);
    return ESP_OK;
}
