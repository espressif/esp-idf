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

#include "soc/soc_caps.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        const periph_module_t module; // Peripheral module
        const int irq_id;
        struct {
            struct {
                const uint32_t pwm_sig;
            } generators[SOC_MCPWM_GENERATORS_PER_OPERATOR];
        } operators[SOC_MCPWM_OPERATORS_PER_GROUP];
        struct {
            const uint32_t fault_sig;
        } detectors[SOC_MCPWM_FAULT_DETECTORS_PER_GROUP];
        struct {
            const uint32_t cap_sig;
        } captures[SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER];
        struct {
            const uint32_t sync_sig;
        } ext_syncers[SOC_MCPWM_EXT_SYNCERS_PER_GROUP];
    } groups[SOC_MCPWM_GROUPS];
} mcpwm_signal_conn_t;

extern const mcpwm_signal_conn_t mcpwm_periph_signals;

#ifdef __cplusplus
}
#endif
