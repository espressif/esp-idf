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

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/pcnt_reg.h"
#include "soc/pcnt_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        struct {
            const uint32_t pulse_sig;
            const uint32_t control_sig;
        } channels[SOC_PCNT_UNIT_CHANNEL_NUM];
    } units[SOC_PCNT_UNIT_NUM];
    const uint32_t irq;
    const periph_module_t module;
} pcnt_signal_conn_t;

extern const pcnt_signal_conn_t pcnt_periph_signals;

#ifdef __cplusplus
}
#endif
