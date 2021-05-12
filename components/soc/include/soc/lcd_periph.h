// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCD_I80_BUS_WIDTH];
        const int cs_sig;
        const int dc_sig;
        const int wr_sig;
    } buses[SOC_LCD_I80_BUSES];
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCD_RGB_DATA_WIDTH];
        const int hsync_sig;
        const int vsync_sig;
        const int pclk_sig;
        const int de_sig;
    } panels[SOC_LCD_RGB_PANELS];
} lcd_signal_conn_t;

extern const lcd_signal_conn_t lcd_periph_signals;

#ifdef __cplusplus
}
#endif
