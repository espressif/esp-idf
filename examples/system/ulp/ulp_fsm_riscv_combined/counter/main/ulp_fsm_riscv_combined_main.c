/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "ulp.h"
#include "ulp_riscv.h"
#include "ulp_fsm_main.h"
#include "ulp_riscv_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t ulp_fsm_main_bin_start[] asm("_binary_ulp_fsm_main_bin_start");
extern const uint8_t ulp_fsm_main_bin_end[]   asm("_binary_ulp_fsm_main_bin_end");

extern const uint8_t ulp_riscv_main_bin_start[] asm("_binary_ulp_riscv_main_bin_start");
extern const uint8_t ulp_riscv_main_bin_end[]   asm("_binary_ulp_riscv_main_bin_end");

static void init_ulp_fsm_program(void);
static void init_ulp_riscv_program(void);

void app_main(void)
{
    /* Initialize shared counter */
    ulp_fsm_counter = 0;
    ulp_riscv_counter = 0;

    printf("ULP FSM and RISC-V Combined Example\n");
    printf("====================================\n\n");

    /* Enable ULP wakeup */
    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());

    /* Step 1: Start ULP FSM to count from 0 to 100 */
    printf("Step 1: Starting ULP FSM to count from 0 to 100...\n");
    printf("HP core going to sleep, will be woken by ULP FSM when counting completes...\n");
    init_ulp_fsm_program();

    /* Enter light sleep - ULP FSM will wake us when done */
    esp_light_sleep_start();

    /* Woken up by ULP FSM */
    uint32_t wakeup_causes = esp_sleep_get_wakeup_causes();
    printf("HP core woken up by: %s\n", (wakeup_causes & (1U << ESP_SLEEP_WAKEUP_ULP)) ? "ULP" : "other");

    /* Stop FSM ULP */
    ulp_timer_stop();
    uint32_t fsm_final_count = ulp_fsm_counter;  // Save FSM result before RISC-V overwrites it
    printf("ULP FSM completed. Counter value: %" PRIu32 "\n\n", fsm_final_count);

    /* Small delay to ensure FSM is fully stopped before starting RISC-V */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Step 2: Start ULP RISC-V to count from 100 to 500 */
    printf("Step 2: Starting ULP RISC-V to count from 100 to 500...\n");
    printf("HP core going to sleep, will be woken by ULP RISC-V when counting completes...\n");
    init_ulp_riscv_program();

    /* Transfer counter value to RISC-V ULP (after loading binary, otherwise it will be overwritten) */
    ulp_riscv_counter = fsm_final_count;

    /* Enter light sleep - ULP RISC-V will wake us when done */
    esp_light_sleep_start();

    /* Woken up by ULP RISC-V */
    wakeup_causes = esp_sleep_get_wakeup_causes();
    printf("HP core woken up by: %s\n", (wakeup_causes & (1U << ESP_SLEEP_WAKEUP_ULP)) ? "ULP" : "other");

    /* Stop RISC-V ULP */
    ulp_riscv_timer_stop();
    uint32_t riscv_final_count = ulp_riscv_counter;  // Save RISC-V result
    printf("ULP RISC-V completed. Counter value: %" PRIu32 "\n\n", riscv_final_count);

    /* Stage 3: Main CPU counts from 500 to 1500 */
    printf("Step 3: Main CPU counting from 500 to 1500...\n");
    uint32_t counter = riscv_final_count;
    while (counter < 1500) {
        counter++;
    }
    printf("Main CPU completed. Final counter value: %" PRIu32 "\n\n", counter);

    printf("====================================\n");
    printf("All stages completed successfully!\n");
    printf("FSM: 0 -> %" PRIu32 "\n", fsm_final_count);
    printf("RISC-V: %" PRIu32 " -> %" PRIu32 "\n", fsm_final_count, riscv_final_count);
    printf("Main CPU: %" PRIu32 " -> %" PRIu32 "\n", riscv_final_count, counter);
}

static void init_ulp_fsm_program(void)
{
    esp_err_t err = ulp_load_binary(0, ulp_fsm_main_bin_start,
            (ulp_fsm_main_bin_end - ulp_fsm_main_bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    /* Set ULP wake up period to 10ms */
    ulp_set_wakeup_period(0, 10000);

    /* Start the ULP FSM program */
    err = ulp_run(&ulp_entry - RTC_SLOW_MEM);
    ESP_ERROR_CHECK(err);
}

static void init_ulp_riscv_program(void)
{
    esp_err_t err = ulp_riscv_load_binary(ulp_riscv_main_bin_start,
            (ulp_riscv_main_bin_end - ulp_riscv_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* Set ULP wake up period to 1ms */
    ulp_set_wakeup_period(0, 1000);

    /* Start the ULP RISC-V program */
    err = ulp_riscv_run();
    ESP_ERROR_CHECK(err);
}
