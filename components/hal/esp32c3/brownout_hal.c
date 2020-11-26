
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

#include "hal/brownout_hal.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "i2c_rtc_clk.h"
#include "i2c_brownout.h"


void brownout_hal_config(const brownout_hal_config_t *cfg)
{
    // TODO ESP32-C3 IDF-2397
}

void brownout_hal_intr_enable(bool enable)
{
    // TODO ESP32-C3 IDF-2397
}

void brownout_hal_intr_clear(void)
{
    // TODO ESP32-C3 IDF-2397
}
