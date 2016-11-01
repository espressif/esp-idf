// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


#if CONFIG_BROWNOUT_DET
/*
This file is included in esp-idf, but the menuconfig option for this is disabled because a silicon bug
prohibits the brownout detector from functioning correctly on the ESP32.
*/

void esp_brownout_init() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 
            RTC_CNTL_BROWN_OUT_ENA | (CONFIG_BROWNOUT_DET_LVL << RTC_CNTL_DBROWN_OUT_THRES_S) |
            RTC_CNTL_BROWN_OUT_RST_ENA | (((CONFIG_BROWNOUT_DET_RESETDELAY*150)/1000) << RTC_CNTL_BROWN_OUT_RST_WAIT_S) |
            RTC_CNTL_BROWN_OUT_PD_RF_ENA|RTC_CNTL_BROWN_OUT_CLOSE_FLASH_ENA);

}

#endif