/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "lp_core_main.h"
#include "ulp_lp_core.h"
#include "freertos/FreeRTOS.h"
#include "ulp_lp_core_critical_section_shared.h"

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");

static void lp_core_init(void)
{
    /* Set LP core wakeup source as the HP CPU */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
        .lp_timer_sleep_duration_us = 10000,
    };

    /* Load LP core firmware */
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));

    /* Run LP core */
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    printf("LP core loaded with firmware and running successfully\n");
}


static void inc_num_spinlock_iter(void *arg)
{
    int core_id = esp_cpu_get_core_id();

    /* Delay 100ms in case of interleaved console output and ensure LP CPU starts first */
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("core %d started, cnt = %ld\n", core_id, ulp_shared_cnt);

    for (int i = 0; i < 100000; i++) {
        ulp_lp_core_enter_critical((ulp_lp_core_spinlock_t*)&ulp_lp_spinlock);
        ulp_shared_cnt++;
        ulp_lp_core_exit_critical((ulp_lp_core_spinlock_t*)&ulp_lp_spinlock);
    }

    printf("HP CPU's increment ends, shared counter = %ld\n", ulp_shared_cnt);

    vTaskDelete(NULL);
}

void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */

    /* Load LP Core binary and start the coprocessor */

    lp_core_init();

    // create tasks on each core which would be accessing a shared resource protected by spinlock
    for (int core_id = 0; core_id < CONFIG_FREERTOS_NUMBER_OF_CORES; core_id++) {
        xTaskCreatePinnedToCore(inc_num_spinlock_iter, NULL, 4096, NULL, 3, NULL, core_id);
    }
}
