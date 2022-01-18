/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/ulp.h"
#include "esp32s2/ulp_riscv.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/ulp.h"
#include "esp32s3/ulp_riscv.h"
#endif
#include "ulp_test_app.h"
#include "unity.h"
#include <sys/time.h>

typedef enum{
    RISCV_READ_WRITE_TEST = 1,
    RISCV_DEEP_SLEEP_WAKEUP_TEST,
    RISCV_LIGHT_SLEEP_WAKEUP_TEST,
    RISCV_NO_COMMAND,
} riscv_test_commands_t;

typedef enum {
    RISCV_COMMAND_OK = 1,
    RISCV_COMMAND_NOK,
    RISCV_COMMAND_INVALID,
} riscv_test_command_reply_t;

#define XOR_MASK 0xDEADBEEF
#define ULP_WAKEUP_PERIOD 1000000 // 1 second

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_test_app_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_test_app_bin_end");
static bool  firmware_loaded = false;

static void load_and_start_ulp_firmware(void)
{
    if (!firmware_loaded) {
        TEST_ASSERT(ulp_riscv_load_binary(ulp_main_bin_start,
                            (ulp_main_bin_end - ulp_main_bin_start)) == ESP_OK);

        TEST_ASSERT(ulp_set_wakeup_period(0, ULP_WAKEUP_PERIOD) == ESP_OK);
        TEST_ASSERT(ulp_riscv_run() == ESP_OK);

        firmware_loaded = true;
    }
}

TEST_CASE("ULP-RISC-V and main CPU are able to exchange data", "[ulp][ignore]")
{
    const uint32_t test_data = 0x12345678;
    struct timeval start, end;

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware();

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_riscv_test_data_in = test_data ^ XOR_MASK;
    ulp_main_cpu_command = RISCV_READ_WRITE_TEST;

    /* Enter Light Sleep */
    TEST_ASSERT(esp_light_sleep_start() == ESP_OK);

    /* Wait for wakeup from ULP RISC-V Coprocessor */
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP);

    /* Wait till we receive the correct command response */
    gettimeofday(&start, NULL);
    while (ulp_command_resp != RISCV_READ_WRITE_TEST)
        ;
    gettimeofday(&end, NULL);
    printf("Response time %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_READ_WRITE_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);
    printf("data out: 0x%X, expected: 0x%X \n", ulp_riscv_test_data_out, test_data);
    TEST_ASSERT(test_data == ulp_riscv_test_data_out);

    /* Clear test data */
    ulp_main_cpu_command = RISCV_NO_COMMAND;
}

TEST_CASE("ULP-RISC-V is able to wakeup main CPU from light sleep", "[ulp][ignore]")
{
    struct timeval start, end;

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware();

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_main_cpu_command = RISCV_LIGHT_SLEEP_WAKEUP_TEST;

    /* Enter Light Sleep */
    TEST_ASSERT(esp_light_sleep_start() == ESP_OK);

    /* Wait for wakeup from ULP RISC-V Coprocessor */
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP);

    /* Wait till we receive the correct command response */
    gettimeofday(&start, NULL);
    while (ulp_command_resp != RISCV_LIGHT_SLEEP_WAKEUP_TEST)
        ;
    gettimeofday(&end, NULL);
    printf("Response time %ld ms\n", (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_LIGHT_SLEEP_WAKEUP_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);

    /* Clear test data */
    ulp_main_cpu_command = RISCV_NO_COMMAND;
}

TEST_CASE("ULP-RISC-V is able to wakeup main CPU from deep sleep", "[ulp][reset=SW_CPU_RESET][ignore]")
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware();

    /* Setup wakeup triggers */
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    /* Setup test data */
    ulp_main_cpu_command = RISCV_DEEP_SLEEP_WAKEUP_TEST;

    /* Enter Deep Sleep */
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}
