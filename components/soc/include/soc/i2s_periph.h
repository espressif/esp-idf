// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
#include "soc/soc.h"
#include "soc/periph_defs.h"
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#include "soc/i2s_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 Stores a bunch of per-I2S-peripheral data.
*/
typedef struct {
    const uint8_t o_bck_in_sig;
    const uint8_t o_ws_in_sig;
    const uint8_t o_bck_out_sig;
    const uint8_t o_ws_out_sig;
    const uint8_t o_data_out_sig;
    const uint8_t i_bck_in_sig;
    const uint8_t i_ws_in_sig;
    const uint8_t i_bck_out_sig;
    const uint8_t i_ws_out_sig;
    const uint8_t i_data_in_sig;
    const uint8_t irq;
    const periph_module_t module;
} i2s_signal_conn_t;

extern const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_PERIPH_NUM];

#ifdef __cplusplus
}
#endif
