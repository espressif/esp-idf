// Copyright 2016 Espressif Systems (Shanghai) PTE LTD
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
#include <stddef.h>
#include <string.h>
#include "esp_attr.h"
#include "soc/wdev_reg.h"
#include "freertos/FreeRTOSConfig.h"
#include "xtensa/core-macros.h"

uint32_t IRAM_ATTR esp_random(void)
{
    static uint32_t last_ccount = 0;
    uint32_t ccount;
    do {
        ccount = XTHAL_GET_CCOUNT();
    } while (ccount - last_ccount < XT_CLOCK_FREQ / APB_CLK_FREQ * 16);
    last_ccount = ccount;
    return REG_READ(WDEV_RND_REG);
}
