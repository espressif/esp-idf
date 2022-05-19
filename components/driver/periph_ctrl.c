// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "hal/clk_gate_ll.h"
#include "esp_attr.h"
#include "driver/periph_ctrl.h"

static portMUX_TYPE periph_spinlock = portMUX_INITIALIZER_UNLOCKED;

static uint8_t ref_counts[PERIPH_MODULE_MAX] = {0};

void periph_module_enable(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    if (ref_counts[periph] == 0) {
        periph_ll_enable_clk_clear_rst(periph);
    }
    ref_counts[periph]++;
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
}

void periph_module_disable(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    ref_counts[periph]--;
    if (ref_counts[periph] == 0) {
        periph_ll_disable_clk_set_rst(periph);
    }
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
}

void periph_module_reset(periph_module_t periph)
{
    assert(periph < PERIPH_MODULE_MAX);
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    periph_ll_reset(periph);
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
}

IRAM_ATTR void wifi_bt_common_module_enable(void)
{
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_enable_clk_clear_rst();
    }
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]++;
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
}

IRAM_ATTR void wifi_bt_common_module_disable(void)
{
    portENTER_CRITICAL_SAFE(&periph_spinlock);
    ref_counts[PERIPH_WIFI_BT_COMMON_MODULE]--;
    if (ref_counts[PERIPH_WIFI_BT_COMMON_MODULE] == 0) {
        periph_ll_wifi_bt_module_disable_clk_set_rst();
    }
    portEXIT_CRITICAL_SAFE(&periph_spinlock);
} 