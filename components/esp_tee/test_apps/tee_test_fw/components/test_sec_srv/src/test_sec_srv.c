/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_cpu.h"
#include "esp_log.h"
#include "esp_tee.h"
#include "esp_tee_test.h"
#include "esp_attr.h"
static const char *TAG = "test_sec_srv";

/* Sample Trusted App */

uint32_t NOINLINE_ATTR _ss_esp_tee_test_service_add(uint32_t a, uint32_t b)
{
    ESP_LOGD(TAG, "SS: %s", __func__);
    return (a + b);
}

uint32_t NOINLINE_ATTR _ss_esp_tee_test_service_sub(uint32_t a, uint32_t b)
{
    ESP_LOGD(TAG, "SS: %s", __func__);
    return (a - b);
}

uint32_t NOINLINE_ATTR _ss_esp_tee_test_service_mul(uint32_t a, uint32_t b)
{
    ESP_LOGD(TAG, "SS: %s", __func__);
    return (a * b);
}

uint32_t NOINLINE_ATTR _ss_esp_tee_test_service_div(uint32_t a, uint32_t b)
{
    ESP_LOGD(TAG, "SS: %s", __func__);
    return (a / b);
}

uint32_t _ss_esp_tee_test_priv_mode_switch(uint32_t *a, uint32_t *b)
{
    ESP_LOGD(TAG, "SS: %s", __func__);

    *a = 1;
    *b = 1;

    uint32_t c = 0;
    while (c < ESP_TEE_TEST_INTR_ITER * 2) {
        c = *a + *b;
        esp_rom_delay_us(500 * 1000U);
        esp_rom_printf("[mode: %d] val: %d\n", esp_cpu_get_curr_privilege_level(), c);
        *a += 1;
        *b += 1;
    }

    return c;
}
