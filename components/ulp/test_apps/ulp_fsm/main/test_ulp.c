/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <unity.h>
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "ulp.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_io_reg.h"
#include "driver/rtc_io.h"

#include "sdkconfig.h"
#include "esp_rom_sys.h"

#include "ulp_test_app.h"

extern const uint8_t ulp_test_app_bin_start[] asm("_binary_ulp_test_app_bin_start");
extern const uint8_t ulp_test_app_bin_end[]   asm("_binary_ulp_test_app_bin_end");

#define HEX_DUMP_DEBUG 0

static void hexdump(const uint32_t* src, size_t count) {
#if HEX_DUMP_DEBUG
    for (size_t i = 0; i < count; ++i) {
        printf("%08x ", *src);
        ++src;
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
#else
    (void)src;
    (void)count;
#endif
}

TEST_CASE("ULP FSM addition test", "[ulp]")
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* ULP co-processor program to add data in 2 memory locations using ULP macros */
    const ulp_insn_t program[] = {
        I_MOVI(R3, 16),     // r3 = 16
        I_LD(R0, R3, 0),    // r0 = mem[r3 + 0]
        I_LD(R1, R3, 1),    // r1 = mem[r3 + 1]
        I_ADDR(R2, R0, R1), // r2 = r0 + r1
        I_ST(R2, R3, 2),    // mem[r3 + 2] = r2
        I_HALT()            // halt
    };

    /* Load the memory regions used by the ULP co-processor */
    RTC_SLOW_MEM[16] = 10;
    RTC_SLOW_MEM[17] = 11;

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ASSERT_EQUAL(ESP_OK, ulp_process_macros_and_load(0, program, &size));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_run(0));

    /* Wait for the ULP co-processor to finish up */
    esp_rom_delay_us(1000);
    hexdump(RTC_SLOW_MEM, 20);

    /* Verify the test results */
    TEST_ASSERT_EQUAL(10 + 11, RTC_SLOW_MEM[18] & 0xffff);
}

TEST_CASE("ULP FSM subtraction and branch test", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* ULP co-processor program to perform subtractions and branch to a label */
    const ulp_insn_t program[] = {
        I_MOVI(R0, 34),     // r0 = 34
        M_LABEL(1),         // define a label with label number as 1
        I_MOVI(R1, 32),     // r1 = 32
        I_LD(R1, R1, 0),    // r1 = mem[32 + 0]
        I_MOVI(R2, 33),     // r2 = 33
        I_LD(R2, R2, 0),    // r2 = mem[33 + 0]
        I_SUBR(R3, R1, R2), // r3 = r1 - r2
        I_ST(R3, R0, 0),    // mem[r0 + 0] = r3
        I_ADDI(R0, R0, 1),  // r0 = r0 + 1
        M_BL(1, 64),        // branch to label 1 if r0 < 64
        I_HALT(),           // halt
    };

    /* Load the memory regions used by the ULP co-processor */
    RTC_SLOW_MEM[32] = 42;
    RTC_SLOW_MEM[33] = 18;

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ASSERT_EQUAL(ESP_OK, ulp_process_macros_and_load(0, program, &size));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_run(0));
    printf("\n\n");

    /* Wait for the ULP co-processor to finish up */
    esp_rom_delay_us(1000);
    hexdump(RTC_SLOW_MEM, 50);

    /* Verify the test results */
    for (int i = 34; i < 64; ++i) {
        TEST_ASSERT_EQUAL(42 - 18, RTC_SLOW_MEM[i] & 0xffff);
    }
    TEST_ASSERT_EQUAL(0, RTC_SLOW_MEM[64]);
}

TEST_CASE("ULP FSM JUMPS instruction test", "[ulp]")
{
    /*
     * Load the ULP binary.
     *
     * This ULP program is written in assembly. Please refer associated .S file.
     */
    esp_err_t err = ulp_load_binary(0, ulp_test_app_bin_start,
            (ulp_test_app_bin_end - ulp_test_app_bin_start) / sizeof(uint32_t));
    TEST_ESP_OK(err);

    /* Clear ULP FSM raw interrupt */
    REG_CLR_BIT(RTC_CNTL_INT_RAW_REG, RTC_CNTL_ULP_CP_INT_RAW);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(&ulp_test_jumps - RTC_SLOW_MEM));

    /* Wait for the ULP co-processor to finish up */
    esp_rom_delay_us(1000);

    /* Verify that ULP FSM issued an interrupt to wake up the main CPU */
    TEST_ASSERT_NOT_EQUAL(0, REG_GET_BIT(RTC_CNTL_INT_RAW_REG, RTC_CNTL_ULP_CP_INT_RAW));

    /* Verify the test results */
    TEST_ASSERT_EQUAL(0, ulp_jumps_fail & UINT16_MAX);
    TEST_ASSERT_EQUAL(1, ulp_jumps_pass & UINT16_MAX);
}

TEST_CASE("ULP FSM light-sleep wakeup test", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* ULP co-processor program to perform some activities and wakeup the main CPU from deep-sleep */
    const ulp_insn_t program[] = {
        I_MOVI(R1, 1024),   // r1 = 1024
        M_LABEL(1),         // define label 1
        I_DELAY(32000),     // add a delay (NOP for 32000 cycles)
        I_SUBI(R1, R1, 1),  // r1 = r1 - 1
        M_BXZ(3),           // branch to label 3 if ALU value is 0. (r1 = 0)
        I_RSHI(R3, R1, 5),  // r3 = r1 / 32
        I_ST(R1, R3, 16),   // mem[r3 + 16] = r1
        M_BX(1),            // loop to label 1
        M_LABEL(3),         // define label 3
        I_MOVI(R2, 42),     // r2 = 42
        I_MOVI(R3, 15),     // r3 = 15
        I_ST(R2, R3, 0),    // mem[r3 + 0] = r2
        I_WAKE(),           // wake the SoC from deep-sleep
        I_END(),            // stop ULP timer
        I_HALT()            // halt
    };

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ASSERT_EQUAL(ESP_OK, ulp_process_macros_and_load(0, program, &size));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Enter Light Sleep */
    TEST_ASSERT(esp_light_sleep_start() == ESP_OK);

    /* Wait for wakeup from ULP FSM Coprocessor */
    printf("cause %d\r\n", esp_sleep_get_wakeup_cause());
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP);
}

TEST_CASE("ULP FSM deep-sleep wakeup test", "[ulp][reset=SW_CPU_RESET][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clearout the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* ULP co-processor program to perform some activities and wakeup the main CPU from deep-sleep */
    const ulp_insn_t program[] = {
        I_MOVI(R1, 1024),   // r1 = 1024
        M_LABEL(1),         // define label 1
        I_DELAY(32000),     // add a delay (NOP for 32000 cycles)
        I_SUBI(R1, R1, 1),  // r1 = r1 - 1
        M_BXZ(3),           // branch to label 3 if ALU value is 0. (r1 = 0)
        I_RSHI(R3, R1, 5),  // r3 = r1 / 32
        I_ST(R1, R3, 16),   // mem[r3 + 16] = r1
        M_BX(1),            // loop to label 1
        M_LABEL(3),         // define label 3
        I_MOVI(R2, 42),     // r2 = 42
        I_MOVI(R3, 15),     // r3 = 15
        I_ST(R2, R3, 0),    // mem[r3 + 0] = r2
        I_WAKE(),           // wake the SoC from deep-sleep
        I_END(),            // stop ULP timer
        I_HALT()            // halt
    };

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ASSERT_EQUAL(ESP_OK, ulp_process_macros_and_load(0, program, &size));
    TEST_ASSERT_EQUAL(ESP_OK, ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}


TEST_CASE("ULP FSM can write and read peripheral registers", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    /* Clear ULP timer */
    CLEAR_PERI_REG_MASK(RTC_CNTL_STATE0_REG, RTC_CNTL_ULP_CP_SLP_TIMER_EN);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop
    uint32_t rtc_store0 = REG_READ(RTC_CNTL_STORE0_REG);
    uint32_t rtc_store1 = REG_READ(RTC_CNTL_STORE1_REG);

    /* ULP co-processor program to read from and write to peripheral registers */
    const ulp_insn_t program[] = {
            I_MOVI(R1, 64),                                 // r1 = 64
            I_RD_REG(RTC_CNTL_STORE1_REG, 0, 15),           // r0 = REG_READ(RTC_CNTL_STORE1_REG[15:0])
            I_ST(R0, R1, 0),                                // mem[r1 + 0] = r0
            I_RD_REG(RTC_CNTL_STORE1_REG, 4, 11),           // r0 = REG_READ(RTC_CNTL_STORE1_REG[11:4])
            I_ST(R0, R1, 1),                                // mem[r1 + 1] = r0
            I_RD_REG(RTC_CNTL_STORE1_REG, 16, 31),          // r0 = REG_READ(RTC_CNTL_STORE1_REG[31:16])
            I_ST(R0, R1, 2),                                // mem[r1 + 2] = r0
            I_RD_REG(RTC_CNTL_STORE1_REG, 20, 27),          // r0 = REG_READ(RTC_CNTL_STORE1_REG[27:20])
            I_ST(R0, R1, 3),                                // mem[r1 + 3] = r0
            I_WR_REG(RTC_CNTL_STORE0_REG, 0, 7, 0x89),      // REG_WRITE(RTC_CNTL_STORE0_REG[7:0], 0x89)
            I_WR_REG(RTC_CNTL_STORE0_REG, 8, 15, 0xab),     // REG_WRITE(RTC_CNTL_STORE0_REG[15:8], 0xab)
            I_WR_REG(RTC_CNTL_STORE0_REG, 16, 23, 0xcd),    // REG_WRITE(RTC_CNTL_STORE0_REG[23:16], 0xcd)
            I_WR_REG(RTC_CNTL_STORE0_REG, 24, 31, 0xef),    // REG_WRITE(RTC_CNTL_STORE0_REG[31:24], 0xef)
            I_LD(R0, R1, 4),                                // r0 = mem[r1 + 4]
            I_ADDI(R0, R0, 1),                              // r0 = r0 + 1
            I_ST(R0, R1, 4),                                // mem[r1 + 4] = r0
            I_END(),                                        // stop ULP timer
            I_HALT()                                        // halt
    };

    /* Set data in the peripheral register to be read by the ULP co-processor */
    REG_WRITE(RTC_CNTL_STORE1_REG, 0x89abcdef);

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    TEST_ESP_OK(ulp_run(0));

    /* Wait for the ULP co-processor to finish up */
    vTaskDelay(100/portTICK_PERIOD_MS);

    /* Verify the test results */
    TEST_ASSERT_EQUAL_HEX32(0xefcdab89, REG_READ(RTC_CNTL_STORE0_REG));
    TEST_ASSERT_EQUAL_HEX16(0xcdef, RTC_SLOW_MEM[64] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(0xde, RTC_SLOW_MEM[65] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(0x89ab, RTC_SLOW_MEM[66] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(0x9a, RTC_SLOW_MEM[67] & 0xffff);
    TEST_ASSERT_EQUAL_HEX16(1, RTC_SLOW_MEM[68] & 0xffff);

    /* Restore initial calibration values */
    REG_WRITE(RTC_CNTL_STORE0_REG, rtc_store0);
    REG_WRITE(RTC_CNTL_STORE1_REG, rtc_store1);
}

TEST_CASE("ULP FSM I_WR_REG instruction test", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* Define the test set */
    typedef struct {
        int low;
        int width;
    } wr_reg_test_item_t;

    const wr_reg_test_item_t test_items[] = {
        {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8},
        {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8},
        {15, 1}, {15, 2}, {15, 3}, {15, 4}, {15, 5}, {15, 6}, {15, 7}, {15, 8},
        {16, 1}, {16, 2}, {16, 3}, {16, 4}, {16, 5}, {16, 6}, {16, 7}, {16, 8},
        {18, 1}, {18, 2}, {18, 3}, {18, 4}, {18, 5}, {18, 6}, {18, 7}, {18, 8},
        {24, 1}, {24, 2}, {24, 3}, {24, 4}, {24, 5}, {24, 6}, {24, 7}, {24, 8},
    };

    const size_t test_items_count =
            sizeof(test_items)/sizeof(test_items[0]);
    for (size_t i = 0; i < test_items_count; ++i) {
        const uint32_t mask = (uint32_t) (((1ULL << test_items[i].width) - 1) << test_items[i].low);
        const uint32_t not_mask = ~mask;
        printf("#%2d: low: %2d width: %2d mask: %08" PRIx32 " expected: %08" PRIx32 " ", i,
                test_items[i].low, test_items[i].width,
                mask, not_mask);

        /* Set all bits in RTC_CNTL_STORE0_REG and reset all bits in RTC_CNTL_STORE1_REG */
        uint32_t rtc_store0 = REG_READ(RTC_CNTL_STORE0_REG);
        uint32_t rtc_store1 = REG_READ(RTC_CNTL_STORE1_REG);
        REG_WRITE(RTC_CNTL_STORE0_REG, 0xffffffff);
        REG_WRITE(RTC_CNTL_STORE1_REG, 0x00000000);

        /* ULP co-processor program to write to peripheral registers */
        const ulp_insn_t program[] = {
            I_WR_REG(RTC_CNTL_STORE0_REG,
                    test_items[i].low,
                    test_items[i].low + test_items[i].width - 1,
                    0),
            I_WR_REG(RTC_CNTL_STORE1_REG,
                    test_items[i].low,
                    test_items[i].low + test_items[i].width - 1,
                    0xff & ((1 << test_items[i].width) - 1)),
            I_END(),
            I_HALT()
        };

        /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
        size_t size = sizeof(program)/sizeof(ulp_insn_t);
        TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
        TEST_ESP_OK(ulp_run(0));

        /* Wait for the ULP co-processor to finish up */
        vTaskDelay(10/portTICK_PERIOD_MS);

        /* Verify the test results */
        uint32_t clear = REG_READ(RTC_CNTL_STORE0_REG);
        uint32_t set = REG_READ(RTC_CNTL_STORE1_REG);
        printf("clear: %08" PRIx32 " set: %08" PRIx32 "\n", clear, set);

        /* Restore initial calibration values */
        REG_WRITE(RTC_CNTL_STORE0_REG, rtc_store0);
        REG_WRITE(RTC_CNTL_STORE1_REG, rtc_store1);

        TEST_ASSERT_EQUAL_HEX32(not_mask, clear);
        TEST_ASSERT_EQUAL_HEX32(mask, set);
    }
}


TEST_CASE("ULP FSM controls RTC_IO", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* ULP co-processor program to toggle LED */
    const ulp_insn_t program[] = {
        I_MOVI(R0, 0),                                  // r0 is LED state
        I_MOVI(R2, 16),                                 // loop r2 from 16 down to 0
        M_LABEL(4),                                     // define label 4
            I_SUBI(R2, R2, 1),                          // r2 = r2 - 1
            M_BXZ(6),                                   // branch to label 6 if r2 = 0
            I_ADDI(R0, R0, 1),                          // r0 = (r0 + 1) % 2
            I_ANDI(R0, R0, 0x1),
            M_BL(0, 1),                                 // if r0 < 1 goto 0
            M_LABEL(1),                                 // define label 1
                I_WR_REG(RTC_GPIO_OUT_REG, 26, 27, 1),  // RTC_GPIO12 = 1
                M_BX(2),                                // goto 2
            M_LABEL(0),                                 // define label 0
                I_WR_REG(RTC_GPIO_OUT_REG, 26, 27, 0),  // RTC_GPIO12 = 0
            M_LABEL(2),                                 // define label 2
                I_MOVI(R1, 100),                        // loop R1 from 100 down to 0
            M_LABEL(3),                                 // define label 3
                I_SUBI(R1, R1, 1),                      // r1 = r1 - 1
                M_BXZ(5),                               // branch to label 5 if r1 = 0
                I_DELAY(32000),                         // delay for a while
                M_BX(3),                                // goto 3
            M_LABEL(5),                                 // define label 5
                M_BX(4),                                // loop back to label 4
        M_LABEL(6),                                     // define label 6
            I_WAKE(),                                   // wake up the SoC
            I_END(),                                    // stop ULP program timer
            I_HALT()
    };

    /* Configure LED GPIOs */
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

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

TEST_CASE("ULP FSM power consumption in deep sleep", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 4 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /* Put the ULP coprocessor in halt state */
    ulp_insn_t insn = I_HALT();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    memcpy(&RTC_SLOW_MEM[0], &insn, sizeof(insn));
#pragma GCC diagnostic pop

    /* Set ULP timer */
    ulp_set_wakeup_period(0, 0x8000);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    TEST_ASSERT(esp_sleep_enable_timer_wakeup(10 * 1000000) == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}

TEST_CASE("ULP FSM timer setting", "[ulp]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 32 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    /*
     * Run a simple ULP program which increments the counter, for one second.
     * Program calls I_HALT each time and gets restarted by the timer.
     * Compare the expected number of times the program runs with the actual.
     */
    const int offset = 6;
    const ulp_insn_t program[] = {
        I_MOVI(R1, offset),     // r1 <- offset
        I_LD(R2, R1, 0),        // load counter
        I_ADDI(R2, R2, 1),      // counter += 1
        I_ST(R2, R1, 0),        // save counter
        I_HALT(),
    };

    /* Calculate the size of the ULP co-processor binary, load it and run the ULP coprocessor */
    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    assert(offset >= size && "data offset needs to be greater or equal to program size");
    TEST_ESP_OK(ulp_run(0));

    /* Disable the ULP program timer — we will enable it later */
    ulp_timer_stop();

    /* Define the test data */
    const uint32_t cycles_to_test[] = { 10000,      // 10 ms
                                        20000,      // 20 ms
                                        50000,      // 50 ms
                                        100000,     // 100 ms
                                        200000,     // 200 ms
                                        500000,     // 500 ms
                                        1000000 };  // 1 sec
    const size_t tests_count = sizeof(cycles_to_test) / sizeof(cycles_to_test[0]);

    for (size_t i = 0; i < tests_count; ++i) {
        // zero out the counter
        RTC_SLOW_MEM[offset] = 0;
        // set the ulp timer period
        ulp_set_wakeup_period(0, cycles_to_test[i]);
        // enable the timer and wait for a second
        ulp_timer_resume();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // stop the timer and get the counter value
        ulp_timer_stop();
        uint32_t counter = RTC_SLOW_MEM[offset] & 0xffff;
        // calculate the expected counter value and allow a tolerance of 15%
        uint32_t expected_counter = 1000000 / cycles_to_test[i];
        uint32_t tolerance = (expected_counter * 15 / 100);
        tolerance = tolerance ? tolerance : 1;  // Keep a tolerance of at least 1 count
        printf("expected: %" PRIu32 "\t tolerance: +/- %" PRIu32 "\t actual: %" PRIu32 "\n", expected_counter, tolerance, counter);
        // Should be within 15%
        TEST_ASSERT_INT_WITHIN(tolerance, expected_counter, counter);
    }
}

#if !DISABLED_FOR_TARGETS(ESP32)
TEST_CASE("ULP FSM can use temperature sensor (TSENS) in deep sleep", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

    // Allow TSENS to be controlled by the ULP
    SET_PERI_REG_BITS(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_CLK_DIV, 10, SENS_TSENS_CLK_DIV_S);
#if CONFIG_IDF_TARGET_ESP32S2
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, SENS_FORCE_XPD_SAR_FSM, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL2_REG, SENS_TSENS_CLKGATE_EN);
#elif CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_MASK(SENS_SAR_PERI_CLK_GATE_CONF_REG, SENS_TSENS_CLK_EN);
#endif
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP_FORCE);

    // data start offset
    size_t offset = 20;
    // number of samples to collect
    RTC_SLOW_MEM[offset] = (CONFIG_ULP_COPROC_RESERVE_MEM) / 4 - offset - 8;
    // sample counter
    RTC_SLOW_MEM[offset + 1] = 0;

    /* ULP co-processor program to record temperature sensor readings */
    const ulp_insn_t program[] = {
        I_MOVI(R1, offset),             // r1 <- offset
        I_LD(R2, R1, 1),                // r2 <- counter
        I_LD(R3, R1, 0),                // r3 <- length
        I_SUBI(R3, R3, 1),              // end = length - 1
        I_SUBR(R3, R3, R2),             // r3 = length - counter
        M_BXF(1),                       // if overflow goto 1:
            I_TSENS(R0, 16383),         // r0 <- tsens
            I_ST(R0, R2, offset + 4),   // mem[r2 + offset +4] <- r0
            I_ADDI(R2, R2, 1),          // counter += 1
            I_ST(R2, R1, 1),            // save counter
            I_HALT(),                   // enter sleep
        M_LABEL(1),                     // done with measurements
            I_END(),                    // stop ULP timer
            I_WAKE(),                   // initiate wakeup
            I_HALT()
    };

    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    assert(offset >= size);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    TEST_ASSERT(esp_sleep_enable_timer_wakeup(10 * 1000000) == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}
#endif //#if !DISABLED_FOR_TARGETS(ESP32)

TEST_CASE("ULP FSM can use ADC in deep sleep", "[ulp][ignore]")
{
    assert(CONFIG_ULP_COPROC_RESERVE_MEM >= 260 && "this test needs ULP_COPROC_RESERVE_MEM option set in menuconfig");

    const int adc = 0;
    const int channel = 0;
    const int atten = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Warray-bounds"
    /* Clear the RTC_SLOW_MEM region for the ULP co-processor binary to be loaded */
    memset(RTC_SLOW_MEM, 0, CONFIG_ULP_COPROC_RESERVE_MEM);
#pragma GCC diagnostic pop

#if defined(CONFIG_IDF_TARGET_ESP32)
    // Configure SAR ADCn resolution
    SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR1_BIT_WIDTH, 3, SENS_SAR1_BIT_WIDTH_S);
    SET_PERI_REG_BITS(SENS_SAR_START_FORCE_REG, SENS_SAR2_BIT_WIDTH, 3, SENS_SAR2_BIT_WIDTH_S);
    SET_PERI_REG_BITS(SENS_SAR_READ_CTRL_REG, SENS_SAR1_SAMPLE_BIT, 0x3, SENS_SAR1_SAMPLE_BIT_S);
    SET_PERI_REG_BITS(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_SAMPLE_BIT, 0x3, SENS_SAR2_SAMPLE_BIT_S);

    // SAR ADCn is started by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_MEAS2_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_MEAS1_START_FORCE);

    // Use ULP FSM to power up SAR ADCn
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_AMP, 2, SENS_FORCE_XPD_AMP_S);

    // SAR ADCn invert result
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR1_DATA_INV);
    SET_PERI_REG_MASK(SENS_SAR_READ_CTRL_REG, SENS_SAR2_DATA_INV);

    // Set SAR ADCn pad enable bitmap to be controlled by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START1_REG, SENS_SAR1_EN_PAD_FORCE_M);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS_START2_REG, SENS_SAR2_EN_PAD_FORCE_M);
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // SAR ADCn is started by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS2_CTRL2_REG, SENS_MEAS2_START_FORCE);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS1_CTRL2_REG, SENS_MEAS1_START_FORCE);

    // Use ULP FSM to power up/down SAR ADCn
    SET_PERI_REG_BITS(SENS_SAR_POWER_XPD_SAR_REG, SENS_FORCE_XPD_SAR, 0, SENS_FORCE_XPD_SAR_S);
    SET_PERI_REG_BITS(SENS_SAR_MEAS1_CTRL1_REG, SENS_FORCE_XPD_AMP, 2, SENS_FORCE_XPD_AMP_S);

    // SAR1 invert result
    SET_PERI_REG_MASK(SENS_SAR_READER1_CTRL_REG, SENS_SAR1_DATA_INV);
    SET_PERI_REG_MASK(SENS_SAR_READER2_CTRL_REG, SENS_SAR2_DATA_INV);

    // Set SAR ADCn pad enable bitmap to be controlled by ULP FSM
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS1_CTRL2_REG, SENS_SAR1_EN_PAD_FORCE_M);
    CLEAR_PERI_REG_MASK(SENS_SAR_MEAS2_CTRL2_REG, SENS_SAR2_EN_PAD_FORCE_M);

    // Enable SAR ADCn clock gate on esp32s3
#if CONFIG_IDF_TARGET_ESP32S3
    SET_PERI_REG_MASK(SENS_SAR_PERI_CLK_GATE_CONF_REG, SENS_SARADC_CLK_EN);
#endif
#endif

    SET_PERI_REG_BITS(SENS_SAR_ATTEN1_REG, 3, atten, 2 * channel); //set SAR1 attenuation
    SET_PERI_REG_BITS(SENS_SAR_ATTEN2_REG, 3, atten, 2 * channel); //set SAR2 attenuation

    // data start offset
    size_t offset = 20;
    // number of samples to collect
    RTC_SLOW_MEM[offset] = (CONFIG_ULP_COPROC_RESERVE_MEM) / 4 - offset - 8;
    // sample counter
    RTC_SLOW_MEM[offset + 1] = 0;

    const ulp_insn_t program[] = {
        I_MOVI(R1, offset),             // r1 <- offset
        I_LD(R2, R1, 1),                // r2 <- counter
        I_LD(R3, R1, 0),                // r3 <- length
        I_SUBI(R3, R3, 1),              // end = length - 1
        I_SUBR(R3, R3, R2),             // r3 = length - counter
        M_BXF(1),                       // if overflow goto 1:
            I_ADC(R0, adc, channel),    // r0 <- ADC
            I_ST(R0, R2, offset + 4),   // mem[r2 + offset +4] = r0
            I_ADDI(R2, R2, 1),          // counter += 1
            I_ST(R2, R1, 1),            // save counter
            I_HALT(),                   // enter sleep
        M_LABEL(1),                     // done with measurements
            I_END(),                    // stop ULP program timer
            I_HALT()
    };

    size_t size = sizeof(program)/sizeof(ulp_insn_t);
    TEST_ESP_OK(ulp_process_macros_and_load(0, program, &size));
    assert(offset >= size);

    /* Run the ULP coprocessor */
    TEST_ESP_OK(ulp_run(0));

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    TEST_ASSERT(esp_sleep_enable_timer_wakeup(10 * 1000000) == ESP_OK);

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}
