// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_log.h"
#include "esp_err.h"
#include "xtensa-debug-module.h"
#include "eri.h"
#include "trax.h"
#include "sdkconfig.h"

#if defined(CONFIG_ESP32_TRAX) || defined(CONFIG_ESP32S2_TRAX)
#define WITH_TRAX 1
#endif

static const char* TAG = "trax";


int trax_start_trace(trax_downcount_unit_t units_until_stop) 
{
#if !WITH_TRAX
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
#if !WITH_TRAX
    ESP_LOGE(TAG, "Trax_trigger_traceend_after_delay called, but trax is disabled in menuconfig!");
    return ESP_ERR_NO_MEM;
#endif
    eri_write(ERI_TRAX_DELAYCNT, delay);
    eri_write(ERI_TRAX_TRAXCTRL, eri_read(ERI_TRAX_TRAXCTRL)|TRAXCTRL_TRSTP);
    return ESP_OK;
}
