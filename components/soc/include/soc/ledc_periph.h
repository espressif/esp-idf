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
#include "soc/ledc_reg.h"
#include "soc/ledc_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 Stores a bunch of per-ledc-peripheral data.
*/
typedef struct {
    const uint8_t sig_out0_idx;
} ledc_signal_conn_t;

#if SOC_LEDC_SUPPORT_HS_MODE
extern const ledc_signal_conn_t ledc_periph_signal[2];
#else
extern const ledc_signal_conn_t ledc_periph_signal[1];
#endif

#ifdef __cplusplus
}
#endif
