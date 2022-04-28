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

#include "soc/sigmadelta_periph.h"
#include "soc/gpio_sig_map.h"

const sigma_delta_signal_conn_t sigma_delta_periph_signals = {
    .channels = {
        [0] = {
            GPIO_SD0_OUT_IDX
        },
        [1] = {
            GPIO_SD1_OUT_IDX
        },
        [2] = {
            GPIO_SD2_OUT_IDX
        },
        [3] = {
            GPIO_SD3_OUT_IDX
        }
    }
};
