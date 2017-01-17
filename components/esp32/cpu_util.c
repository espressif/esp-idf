// Copyright 2013-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_attr.h"
#include "soc/cpu.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "sdkconfig.h"

void IRAM_ATTR esp_cpu_stall(int cpu_id)
{
    if (cpu_id == 1) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_APPCPU_C1_M);
        SET_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, 0x21<<RTC_CNTL_SW_STALL_APPCPU_C1_S);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_APPCPU_C0_M);
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, 2<<RTC_CNTL_SW_STALL_APPCPU_C0_S);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_PROCPU_C1_M);
        SET_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, 0x21<<RTC_CNTL_SW_STALL_PROCPU_C1_S);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_PROCPU_C0_M);
        SET_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, 2<<RTC_CNTL_SW_STALL_PROCPU_C0_S);
    }
}

void IRAM_ATTR esp_cpu_unstall(int cpu_id)
{
    if (cpu_id == 1) {
        CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_APPCPU_C1_M);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_APPCPU_C0_M);
    } else {
        CLEAR_PERI_REG_MASK(RTC_CNTL_SW_CPU_STALL_REG, RTC_CNTL_SW_STALL_PROCPU_C1_M);
        CLEAR_PERI_REG_MASK(RTC_CNTL_OPTIONS0_REG, RTC_CNTL_SW_STALL_PROCPU_C0_M);
    }
}

bool IRAM_ATTR esp_cpu_in_ocd_debug_mode()
{
#if CONFIG_ESP32_DEBUG_OCDAWARE
    int dcr;
    int reg=0x10200C; //DSRSET register
    asm("rer %0,%1":"=r"(dcr):"r"(reg));
    return (dcr&0x1);
#else
    return false; // Always return false if "OCD aware" is disabled
#endif
}
