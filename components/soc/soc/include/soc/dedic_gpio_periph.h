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

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DEDICATED_GPIO_SUPPORTED
typedef struct {
    const periph_module_t module; // Peripheral module
    const int irq;                // Interrupt resource (-1 means no interrupt supported)
    struct {
        const int in_sig_per_channel[SOC_DEDIC_GPIO_IN_CHANNELS_NUM];
        const int out_sig_per_channel[SOC_DEDIC_GPIO_OUT_CHANNELS_NUM];
    } cores[SOC_CPU_CORES_NUM]; // Signals routed to/from GPIO matrix
} dedic_gpio_signal_conn_t;

extern const dedic_gpio_signal_conn_t dedic_gpio_periph_signals;
#endif // SOC_DEDICATED_GPIO_SUPPORTED

#ifdef __cplusplus
}
#endif
