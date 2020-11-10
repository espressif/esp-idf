// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"
#include "esp32s2/ulp.h"
#include "esp32s2/ulp_riscv.h"
#include "ulp_test_app.h"
#include "unity.h"

typedef enum{
    RISCV_READ_WRITE_TEST = 1,
    RISCV_DEEP_SLEEP_WAKEUP_TEST,
    RISCV_LIGHT_SLEEP_WAKEUP_TEST,
    RISCV_NO_COMMAND,
} riscv_test_commands_t;

typedef enum {
    RISCV_COMMAND_OK = 1,
    RISCV_COMMAND_NOK,
} riscv_test_command_reply_t;

#define XOR_MASK 0xDEADBEEF

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_test_app_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_test_app_bin_end");
static bool  firmware_loaded = false;

static void load_and_start_ulp_firmware(void)
{
    if (!firmware_loaded) {
        TEST_ASSERT(ulp_riscv_load_binary(ulp_main_bin_start,
                            (ulp_main_bin_end - ulp_main_bin_start)) == ESP_OK);

        TEST_ASSERT(ulp_set_wakeup_period(0, 1000000) == ESP_OK);
        TEST_ASSERT(ulp_riscv_run() == ESP_OK);

        firmware_loaded = true;
    }
}

TEST_CASE("ULP-RISC-V and main CPU are able to exchange data", "[ulp][ignore]")
{
    const uint32_t test_data = 0x12345678;

    load_and_start_ulp_firmware();
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);

    ulp_riscv_test_data_in = test_data ^ XOR_MASK;
    ulp_main_cpu_command = RISCV_READ_WRITE_TEST;

    TEST_ASSERT(esp_light_sleep_start() == ESP_OK);
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);

    printf("data out: 0x%X, expected: 0x%X \n", ulp_riscv_test_data_out, test_data);
    TEST_ASSERT(test_data == ulp_riscv_test_data_out);

}

TEST_CASE("ULP-RISC-V is able to wakeup main CPU from light sleep", "[ulp][ignore]")
{
    load_and_start_ulp_firmware();
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    ulp_main_cpu_command = RISCV_LIGHT_SLEEP_WAKEUP_TEST;
    TEST_ASSERT(esp_light_sleep_start() == ESP_OK);
    TEST_ASSERT(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_ULP);
    TEST_ASSERT(ulp_main_cpu_reply == RISCV_COMMAND_OK);
}

TEST_CASE("ULP-RISC-V is able to wakeup main CPU from deep sleep", "[ulp][reset=SW_CPU_RESET][ignore]")
{
    load_and_start_ulp_firmware();
    TEST_ASSERT(esp_sleep_enable_ulp_wakeup() == ESP_OK);
    ulp_main_cpu_command = RISCV_DEEP_SLEEP_WAKEUP_TEST;
    esp_deep_sleep_start();
    UNITY_TEST_FAIL(__LINE__, "Should not get here!");
}
