/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>
#include <stdio.h>
#include "esp_sleep.h"
#include "ulp.h"
#include "ulp_riscv.h"
#include "ulp_fsm_main.h"
#include "ulp_riscv_main.h"

extern const uint8_t ulp_fsm_main_bin_start[] asm("_binary_ulp_fsm_main_bin_start");
extern const uint8_t ulp_fsm_main_bin_end[] asm("_binary_ulp_fsm_main_bin_end");

extern const uint8_t ulp_riscv_main_bin_start[] asm("_binary_ulp_riscv_main_bin_start");
extern const uint8_t ulp_riscv_main_bin_end[] asm("_binary_ulp_riscv_main_bin_end");

static void init_ulp_fsm_program(void);
static void init_ulp_riscv_program(uint32_t initial_count);

void app_main(void)
{
    printf("ULP FSM and RISC-V full subproject example\n");

    ESP_ERROR_CHECK(esp_sleep_enable_ulp_wakeup());

    printf("Starting ULP FSM counter\n");
    init_ulp_fsm_program();
    ESP_ERROR_CHECK(esp_light_sleep_start());
    ulp_timer_stop();

    uint32_t fsm_count = ulp_fsm_counter & UINT16_MAX;
    printf("ULP FSM counter: %" PRIu32 "\n", fsm_count);

    printf("Starting ULP RISC-V counter\n");
    init_ulp_riscv_program(fsm_count);
    ESP_ERROR_CHECK(esp_light_sleep_start());
    ulp_riscv_timer_stop();

    uint32_t riscv_count = ulp_riscv_counter;
    printf("ULP RISC-V counter: %" PRIu32 "\n", riscv_count);
    printf("Example finished\n");
}

static void init_ulp_fsm_program(void)
{
    ESP_ERROR_CHECK(ulp_load_binary(0, ulp_fsm_main_bin_start,
                                    (ulp_fsm_main_bin_end - ulp_fsm_main_bin_start) / sizeof(uint32_t)));

    ulp_fsm_counter = 0;
    ulp_set_wakeup_period(0, 10000);
    ESP_ERROR_CHECK(ulp_run(&ulp_entry - RTC_SLOW_MEM));
}

static void init_ulp_riscv_program(uint32_t initial_count)
{
    ESP_ERROR_CHECK(ulp_riscv_load_binary(ulp_riscv_main_bin_start,
                                          ulp_riscv_main_bin_end - ulp_riscv_main_bin_start));

    ulp_riscv_counter = initial_count;
    ulp_set_wakeup_period(0, 10000);
    ESP_ERROR_CHECK(ulp_riscv_run());
}
