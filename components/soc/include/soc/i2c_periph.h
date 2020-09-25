// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint8_t sda_out_sig;
    const uint8_t sda_in_sig;
    const uint8_t scl_out_sig;
    const uint8_t scl_in_sig;
    const uint8_t irq;
    const periph_module_t module;
} i2c_signal_conn_t;

extern const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM];

#ifdef __cplusplus
}
#endif