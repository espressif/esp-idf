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

#include "soc/dac_periph.h"

/*
 Bunch of constants for DAC peripheral: GPIO number
*/
const dac_signal_conn_t dac_periph_signal = {
    .dac_channel_io_num[0] = DAC_CHANNEL_1_GPIO_NUM,
    .dac_channel_io_num[1] = DAC_CHANNEL_2_GPIO_NUM,
};