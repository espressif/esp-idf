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

#include "hal/dac_hal.h"

void dac_hal_cw_generator_config(dac_cw_config_t *cw)
{
    dac_ll_cw_set_freq(cw->freq);
    dac_ll_cw_set_scale(cw->en_ch, cw->scale);
    dac_ll_cw_set_phase(cw->en_ch, cw->phase);
    dac_ll_cw_set_dc_offset(cw->en_ch, cw->offset);
    dac_ll_cw_set_channel(cw->en_ch, true);
}
