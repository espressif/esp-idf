// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include "soc/soc.h"
#include "soc/lcd_periph.h"
#include "soc/gpio_sig_map.h"

const lcd_signal_conn_t lcd_periph_signals = {
    .buses = {
        [0] = {
            .module = PERIPH_I2S0_MODULE,
            .irq_id = ETS_I2S0_INTR_SOURCE,
            .data_sigs = {
                I2S0O_DATA_OUT0_IDX,
                I2S0O_DATA_OUT1_IDX,
                I2S0O_DATA_OUT2_IDX,
                I2S0O_DATA_OUT3_IDX,
                I2S0O_DATA_OUT4_IDX,
                I2S0O_DATA_OUT5_IDX,
                I2S0O_DATA_OUT6_IDX,
                I2S0O_DATA_OUT7_IDX,
                I2S0O_DATA_OUT8_IDX,
                I2S0O_DATA_OUT9_IDX,
                I2S0O_DATA_OUT10_IDX,
                I2S0O_DATA_OUT11_IDX,
                I2S0O_DATA_OUT12_IDX,
                I2S0O_DATA_OUT13_IDX,
                I2S0O_DATA_OUT14_IDX,
                I2S0O_DATA_OUT15_IDX,
                I2S0O_DATA_OUT16_IDX,
                I2S0O_DATA_OUT17_IDX,
                I2S0O_DATA_OUT18_IDX,
                I2S0O_DATA_OUT19_IDX,
                I2S0O_DATA_OUT20_IDX,
                I2S0O_DATA_OUT21_IDX,
                I2S0O_DATA_OUT22_IDX,
                I2S0O_DATA_OUT23_IDX,
            },
            .wr_sig = I2S0O_WS_OUT_IDX,
        }
    }
};
