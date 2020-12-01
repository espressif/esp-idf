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

#include "soc/i2c_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2C peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM] = {
    {
        .sda_out_sig = I2CEXT0_SDA_OUT_IDX,
        .sda_in_sig = I2CEXT0_SDA_IN_IDX,
        .scl_out_sig = I2CEXT0_SCL_OUT_IDX,
        .scl_in_sig = I2CEXT0_SCL_IN_IDX,
        .irq = ETS_I2C_EXT0_INTR_SOURCE,
        .module = PERIPH_I2C0_MODULE,
    },
};
