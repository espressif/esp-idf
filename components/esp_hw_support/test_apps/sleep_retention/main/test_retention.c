/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "unity.h"
#include "esp_private/sleep_sys_periph.h"
#include "esp_private/sleep_retention.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"

const char TAG[] = "retention";

TEST_CASE("retention: can go to retention", "[retention]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    sleep_cpu_configure(true);

    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Going to sleep...");
    esp_light_sleep_start();

    ESP_LOGI(TAG, "After wakeup");
    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);

    sleep_cpu_configure(false);
}
