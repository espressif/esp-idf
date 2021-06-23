// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#include "ulp_riscv/ulp_riscv.h"
#include "ulp_riscv/ulp_riscv_utils.h"

void ulp_riscv_rescue_from_monitor(void)
{
    /* Rescue RISCV from monitor state. */
    CLEAR_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE | RTC_CNTL_COCPU_SHUT_RESET_EN);
}

void ulp_riscv_wakeup_main_processor(void)
{
    SET_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_SW_CPU_INT);
}

void ulp_riscv_shutdown(void)
{
    /* Setting the delay time after RISCV recv `DONE` signal, Ensure that action `RESET` can be executed in time. */
    REG_SET_FIELD(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_2_CLK_DIS, 0x3F);

    /* suspends the ulp operation*/
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_DONE);

    /* Resets the processor */
    SET_PERI_REG_MASK(RTC_CNTL_COCPU_CTRL_REG, RTC_CNTL_COCPU_SHUT_RESET_EN);

    while(1);
}

void ulp_riscv_delay_cycles(uint32_t cycles)
{
    uint32_t start = ULP_RISCV_GET_CCOUNT();

    while ((ULP_RISCV_GET_CCOUNT() - start) < cycles) {
        /* Wait */
    }
}
