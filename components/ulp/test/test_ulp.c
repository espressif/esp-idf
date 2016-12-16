// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp32/ulp.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "sdkconfig.h"

static void hexdump(const uint32_t* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        printf("%08x ", *src);
        ++src;
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

TEST_CASE("ulp add test", "[ulp]")
{
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    const ulp_insn_t program[] = {
        I_MOVI(R3, 16),
        I_LD(R0, R3, 0),
        I_LD(R1, R3, 1),
        I_ADDR(R2, R0, R1),
        I_ST(R2, R3, 2),
        I_HALT()
    };
    RTC_SLOW_MEM[16] = 10;
    RTC_SLOW_MEM[17] = 11;
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ASSERT_EQUAL(ESP_OK, ulp_process_macros_and_load(0, program, &size));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_run(0));
    ets_delay_us(1000);
    hexdump(RTC_SLOW_MEM, CONFIG_ULP_COPROC_RESERVE_MEM / 4);
    TEST_ASSERT_EQUAL(10 + 11, RTC_SLOW_MEM[18] & 0xffff);
}

TEST_CASE("ulp branch test", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    const ulp_insn_t program[] = {
        I_MOVI(R0, 34),     // r0 = dst
        M_LABEL(1),
        I_MOVI(R1, 32),
        I_LD(R1, R1, 0),    // r1 = mem[33]
        I_MOVI(R2, 33),
        I_LD(R2, R2, 0),    // r2 = mem[34]
        I_SUBR(R3, R1, R2), // r3 = r1 - r2
        I_ST(R3, R0, 0),    // dst[0] = r3
        I_ADDI(R0, R0, 1),
        M_BL(1, 64),
        I_HALT(),
    };
    RTC_SLOW_MEM[32] = 42;
    RTC_SLOW_MEM[33] = 18;
    hexdump(RTC_SLOW_MEM, CONFIG_ULP_COPROC_RESERVE_MEM / 4);
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    ulp_process_macros_and_load(0, program, &size);
    ulp_run(0);
    printf("\n\n");
    hexdump(RTC_SLOW_MEM, CONFIG_ULP_COPROC_RESERVE_MEM / 4);
    for (int i = 34; i < 64; ++i) {
        TEST_ASSERT_EQUAL(42 - 18, RTC_SLOW_MEM[i] & 0xffff);
    }
    TEST_ASSERT_EQUAL(0, RTC_SLOW_MEM[64]);
}
