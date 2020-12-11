// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "soc/rtc.h"
#include "esp32c3/clk.h"
#include "esp32c3/rom/ets_sys.h"

#define MHZ (1000000)

int IRAM_ATTR esp_clk_cpu_freq(void)
{
    return ets_get_cpu_frequency() * MHZ;
}

int IRAM_ATTR esp_clk_apb_freq(void)
{
    return MIN(80, ets_get_cpu_frequency()) * MHZ;
}

int IRAM_ATTR esp_clk_xtal_freq(void)
{
    return rtc_clk_xtal_freq_get() * MHZ;
}
