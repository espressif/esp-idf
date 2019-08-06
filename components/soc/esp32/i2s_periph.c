// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/i2s_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2S peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_PERIPH_NUM] = {
    {
        .o_bck_in_sig = I2S0O_BCK_IN_IDX,
        .o_ws_in_sig = I2S0O_WS_IN_IDX,
        .o_bck_out_sig = I2S0O_BCK_OUT_IDX,
        .o_ws_out_sig = I2S0O_WS_OUT_IDX,
        .o_data_out_sig = I2S0O_DATA_OUT23_IDX,
        .i_bck_in_sig = I2S0I_BCK_OUT_IDX,
        .i_ws_in_sig = I2S0I_WS_OUT_IDX,
        .i_bck_out_sig = I2S0I_BCK_IN_IDX,
        .i_ws_out_sig = I2S0I_WS_IN_IDX,
        .i_data_in_sig = I2S0I_DATA_IN15_IDX,
        .irq = ETS_I2S0_INTR_SOURCE,
        .module = PERIPH_I2S0_MODULE,
    },
    {
        .o_bck_in_sig = I2S1O_BCK_IN_IDX,
        .o_ws_in_sig = I2S1O_WS_IN_IDX,
        .o_bck_out_sig = I2S1O_BCK_OUT_IDX,
        .o_ws_out_sig = I2S1O_WS_OUT_IDX,
        .o_data_out_sig = I2S1O_DATA_OUT23_IDX,
        .i_bck_in_sig = I2S1I_BCK_OUT_IDX,
        .i_ws_in_sig = I2S1I_WS_OUT_IDX,
        .i_bck_out_sig = I2S1I_BCK_IN_IDX,
        .i_ws_out_sig = I2S1I_WS_IN_IDX,
        .i_data_in_sig = I2S1I_DATA_IN15_IDX,
        .irq = ETS_I2S1_INTR_SOURCE,
        .module = PERIPH_I2S1_MODULE,
    }
};
