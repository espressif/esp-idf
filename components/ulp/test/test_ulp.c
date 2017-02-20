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
#include "esp_deep_sleep.h"

#include "esp32/ulp.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "driver/rtc_io.h"

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

TEST_CASE("ulp branch test", "[ulp][ignore]")
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

TEST_CASE("ulp wakeup test", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    const ulp_insn_t program[] = {
        I_MOVI(R1, 1024),
        M_LABEL(1),
        I_DELAY(32000),
        I_SUBI(R1, R1, 1),
        M_BXZ(3),
        I_RSHI(R3, R1, 5),  // R3 = R1 / 32
        I_ST(R1, R3, 16),
        M_BX(1),
        M_LABEL(3),
        I_MOVI(R2, 42),
        I_MOVI(R3, 15),
        I_ST(R2, R3, 0),
        I_END(1)
    };
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    ulp_process_macros_and_load(0, program, &size);
    ulp_run(0);
    esp_deep_sleep_enable_ulp_wakeup();
    esp_deep_sleep_start();
}

TEST_CASE("ulp can write and read peripheral registers", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");
    CLEAR_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    REG_WRITE(RTC_CNTL_STORE1_REG, 0x89abcdef);

    const ulp_insn_t program[] = {
            I_MOVI(R1, 64),
            I_RD_REG(RTC_CNTL_STORE1_REG, 0, 15),
            I_ST(R0, R1, 0),
            I_RD_REG(RTC_CNTL_STORE1_REG, 4, 11),
            I_ST(R0, R1, 1),
            I_RD_REG(RTC_CNTL_STORE1_REG, 16, 31),
            I_ST(R0, R1, 2),
            I_RD_REG(RTC_CNTL_STORE1_REG, 20, 27),
            I_ST(R0, R1, 3),
            I_WR_REG(RTC_CNTL_STORE0_REG, 0, 7, 0x89),
            I_WR_REG(RTC_CNTL_STORE0_REG, 8, 15, 0xab),
            I_WR_REG(RTC_CNTL_STORE0_REG, 16, 23, 0xcd),
            I_WR_REG(RTC_CNTL_STORE0_REG, 24, 31, 0xef),
            I_LD(R0, R1, 4),
            I_ADDI(R0, R0, 1),
            I_ST(R0, R1, 4),
            I_END(0)
    };
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    TEST_ESP_OK(ulp_run(0));
    vTaskDelay(100/portTICK_PERIOD_MS);

    TEST_ASSERT_EQUAL_HEX32(0xefcdab89, REG_READ(RTC_CNTL_STORE0_REG));
    TEST_ASSERT_EQUAL_HEX16(0xcdef, RTC_SLOW_MEM[64] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(0xde, RTC_SLOW_MEM[65] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(0x89ab, RTC_SLOW_MEM[66] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(0x9a, RTC_SLOW_MEM[67] & 0xffff);
    TEST_ASSERT_EQUAL_HEX32(1 | (15 << 21) | (1 << 16), RTC_SLOW_MEM[68]);
}

TEST_CASE("ulp controls RTC_IO", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
    const ulp_insn_t program[] = {
        I_MOVI(R0, 0),                  // R0 is LED state
        I_MOVI(R2, 16),                 // loop R2 from 16 down to 0
        M_LABEL(4),
            I_SUBI(R2, R2, 1),
            M_BXZ(6),
            I_ADDI(R0, R0, 1),          // R0 = (R0 + 1) % 2
            I_ANDI(R0, R0, 0x1),
            M_BL(0, 1),                 // if R0 < 1 goto 0
            M_LABEL(1),
                I_WR_REG(RTC_GPIO_OUT_REG, 26, 27, 1), // RTC_GPIO12 = 1
                M_BX(2),                // goto 2
            M_LABEL(0),                 // 0:
                I_WR_REG(RTC_GPIO_OUT_REG, 26, 27, 0), // RTC_GPIO12 = 0
            M_LABEL(2),                 // 2:
            I_MOVI(R1, 100),            // loop R1 from 100 down to 0
            M_LABEL(3),
                I_SUBI(R1, R1, 1),
                M_BXZ(5),
                I_DELAY(32000),         // delay for a while
                M_BX(3),
            M_LABEL(5),
            M_BX(4),
        M_LABEL(6),
        I_END(1)                        // wake up the SoC
    };
    const gpio_num_t led_gpios[] = {
        GPIO_NUM_2,
        GPIO_NUM_0,
        GPIO_NUM_4
    };
    for (size_t i = 0; i < sizeof(led_gpios)/sizeof(led_gpios[0]); ++i) {
        rtc_gpio_init(led_gpios[i]);
        rtc_gpio_set_direction(led_gpios[i], RTC_GPIO_MODE_OUTPUT_ONLY);
        rtc_gpio_set_level(led_gpios[i], 0);
    }
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    ulp_process_macros_and_load(0, program, &size);
    ulp_run(0);
    esp_deep_sleep_enable_ulp_wakeup();
    esp_deep_sleep_start();
}


TEST_CASE("ulp can use TSENS in deep sleep", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    hexdump(RTC_SLOW_MEM, CONFIG_ULP_COPROC_RESERVE_MEM / 4);
    printf("\n\n");
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);

    // Allow TSENS to be controlled by the ULP
    SET_PERI_REG_BITS(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_CLK_DIV, 10, SENS_TSENS_CLK_DIV_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 3, SENS_FORCE_XPD_SAR_S);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP_FORCE);

    // data start offset
    size_t offset = 20;
    // number of samples to collect
    RTC_SLOW_MEM[offset] = (CONFIG_ULP_COPROC_RESERVE_MEM) / 4 - offset - 8;
    // sample counter
    RTC_SLOW_MEM[offset + 1] = 0;

    const ulp_insn_t program[] = {
        I_MOVI(R1, offset),     // r1 <- offset
        I_LD(R2, R1, 1),    // r2 <- counter
        I_LD(R3, R1, 0),    // r3 <- length
        I_SUBI(R3, R3, 1),  // end = length - 1
        I_SUBR(R3, R3, R2), // r3 = length - counter
        M_BXF(1),           // if overflow goto 1:
            I_WR_REG(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR_S, SENS_FORCE_XPD_SAR_S + 1, 3),
            I_TSENS(R0, 16383), // r0 <- tsens
            I_WR_REG(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR_S, SENS_FORCE_XPD_SAR_S + 1, 0),
            I_ST(R0, R2, offset + 4),
            I_ADDI(R2, R2, 1),  // counter += 1
            I_ST(R2, R1, 1),    // save counter
            I_SLEEP(0),         // enter sleep
            I_HALT(),
        M_LABEL(1),             // done with measurements
            I_END(0),           // stop ULP timer
            I_HALT()
    };

    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    assert(offset >= size);

    TEST_ESP_OK(ulp_run(0));
    esp_deep_sleep_enable_timer_wakeup(4000000);
    esp_deep_sleep_start();
}

