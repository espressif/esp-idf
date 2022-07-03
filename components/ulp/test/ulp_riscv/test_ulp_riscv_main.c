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
#include "ulp_riscv.h"
#include "ulp_test_app.h"
#include "unity.h"
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum{
    RISCV_READ_WRITE_TEST = 1,
    RISCV_DEEP_SLEEP_WAKEUP_TEST,
    RISCV_LIGHT_SLEEP_WAKEUP_TEST,
    RISCV_STOP_TEST,
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

TEST_CASE("ULP-RISC-V and main CPU are able to exchange data", "[ulp]")
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
    printf("Response time %jd ms\n", ((intmax_t)end.tv_sec - (intmax_t)start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_READ_WRITE_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);
    printf("data out: 0x%X, expected: 0x%X \n", ulp_riscv_test_data_out, test_data);
    TEST_ASSERT(test_data == ulp_riscv_test_data_out);

    /* Clear test data */
    ulp_main_cpu_command = RISCV_NO_COMMAND;
}

TEST_CASE("ULP-RISC-V is able to wakeup main CPU from light sleep", "[ulp]")
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
    printf("Response time 1st: %jd ms\n", ((intmax_t)end.tv_sec - (intmax_t)start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_LIGHT_SLEEP_WAKEUP_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);

    /* Enter Light Sleep again */
    TEST_ASSERT(esp_light_sleep_start() == ESP_OK);

    /* Wait for wakeup from ULP RISC-V Coprocessor */
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP);

    /* Wait till we receive the correct command response */
    gettimeofday(&start, NULL);
    while (ulp_command_resp != RISCV_LIGHT_SLEEP_WAKEUP_TEST)
        ;
    gettimeofday(&end, NULL);
    printf("Response time 2nd: %jd ms\n", ((intmax_t)end.tv_sec - (intmax_t)start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);

    /* Verify test data */
    TEST_ASSERT(ulp_command_resp == RISCV_LIGHT_SLEEP_WAKEUP_TEST);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);

    /* Clear test data */
    ulp_main_cpu_command = RISCV_NO_COMMAND;
}

static bool ulp_riscv_is_running(void)
{
    uint32_t start_cnt = ulp_riscv_counter;

    /* Wait a few ULP wakeup cycles to ensure ULP has run */
    vTaskDelay((5 * ULP_WAKEUP_PERIOD / 1000) / portTICK_PERIOD_MS);

    uint32_t end_cnt = ulp_riscv_counter;
    printf("start run count: %d, end run count %d\n", start_cnt, end_cnt);

    /* If the ulp is running the counter should have been incremented */
    return (start_cnt != end_cnt);
}

TEST_CASE("ULP-RISC-V can be stopped and resumed from main CPU", "[ulp]")
{
    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware();

    TEST_ASSERT(ulp_riscv_is_running());

    printf("Stopping the ULP\n");
    ulp_riscv_timer_stop();
    ulp_riscv_halt();

    TEST_ASSERT(!ulp_riscv_is_running());

    printf("Resuming the ULP\n");
    ulp_riscv_timer_resume();

    TEST_ASSERT(ulp_riscv_is_running());
}

TEST_CASE("ULP-RISC-V can stop itself and be resumed from the main CPU", "[ulp]")
{
    volatile riscv_test_commands_t *command_resp = &ulp_command_resp;

    /* Load ULP RISC-V firmware and start the ULP RISC-V Coprocessor */
    load_and_start_ulp_firmware();

    TEST_ASSERT(ulp_riscv_is_running());

    printf("Stopping the ULP\n");
    /* Setup test data */
    ulp_main_cpu_command = RISCV_STOP_TEST;

    while (*command_resp != RISCV_STOP_TEST) {
    }

    /* Wait a bit to ensure ULP finished shutting down */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    TEST_ASSERT(!ulp_riscv_is_running());

    printf("Resuming the ULP\n");
    ulp_main_cpu_command = RISCV_NO_COMMAND;
    ulp_riscv_timer_resume();

    TEST_ASSERT(ulp_riscv_is_running());
}

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
//IDF-5131
/*
* Keep this test case as the last test case in this suite as a CPU reset occurs.
* Add new test cases above in order to ensure they run when all test cases are run together.
*/
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

#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
