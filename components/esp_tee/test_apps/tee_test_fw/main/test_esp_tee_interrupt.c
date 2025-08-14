/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "driver/gptimer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_tee.h"
#include "esp_tee_test.h"
#include "secure_service_num.h"

#include "unity.h"

#define TEST_TIMER_RESOLUTION_HZ  (1000000ULL) // 1MHz, 1 tick = 1us
#define TIMER_ALARM_PERIOD_S      (0.25f)      // 250ms @ resolution 1MHz

static const char __attribute__((unused)) *TAG = "test_esp_tee_intr";

/* ---------------------------------------------------- Utility functions ---------------------------------------------------- */

static gptimer_handle_t gptimer = NULL;

static bool IRAM_ATTR test_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    uint32_t *intr_count = (uint32_t *)user_data;
    *intr_count = *intr_count + 1;
    esp_rom_printf("[mode: %d] Interrupt triggered (%d)\n", esp_cpu_get_curr_privilege_level(), *intr_count);
    return true;
}

static void test_timer_init(volatile uint32_t *arg)
{
    /* Select and initialize basic parameters of the timer */
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TEST_TIMER_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_timer_on_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, (void *)arg));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config2 = {
        .reload_count = 0,
        .alarm_count = TIMER_ALARM_PERIOD_S * TEST_TIMER_RESOLUTION_HZ,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config2));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

static void test_timer_deinit(void)
{
    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    ESP_ERROR_CHECK(gptimer_disable(gptimer));
    ESP_ERROR_CHECK(gptimer_del_timer(gptimer));
}

/* ---------------------------------------------------- Test cases ---------------------------------------------------- */

TEST_CASE("Test TEE interrupt in TEE", "[basic]")
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    uint32_t val = esp_tee_service_call(1, SS_ESP_TEE_TEST_TEE_INTR_IN_TEE);
    TEST_ASSERT_EQUAL_UINT32(ESP_TEE_TEST_INTR_ITER, val);

    mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");
}

TEST_CASE("Test REE interrupt in REE", "[basic]")
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    volatile uint32_t ree_intr_count = 0;
    test_timer_init(&ree_intr_count);
    while (ree_intr_count < ESP_TEE_TEST_INTR_ITER) {
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    test_timer_deinit();
}

TEST_CASE("Test REE interrupt in TEE", "[basic]")
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    volatile uint32_t ree_intr_count = 0;
    volatile uint32_t *volatile ree_intr_count_ptr = &ree_intr_count;

    test_timer_init(ree_intr_count_ptr);

    uint32_t val = esp_tee_service_call(2, SS_ESP_TEE_TEST_REE_INTR_IN_TEE, ree_intr_count_ptr);
    TEST_ASSERT_EQUAL_UINT32(0, val);

    test_timer_deinit();

    mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");
}

TEST_CASE("Test TEE interrupt in REE", "[basic]")
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    volatile uint32_t tee_intr_count = 0;
    volatile uint32_t *volatile tee_intr_count_ptr = &tee_intr_count;
    esp_tee_service_call(3, SS_ESP_TEE_TEST_TEE_INTR_IN_REE, 0, tee_intr_count_ptr);

    uint32_t prev_count = 0;
    while (true) {
        uint32_t curr_count = *tee_intr_count_ptr;
        if (curr_count > prev_count) {
            prev_count = curr_count;
            esp_rom_printf("[mode: %d] Interrupt received (%d)\n", esp_cpu_get_curr_privilege_level(), curr_count);
        }
        if (curr_count >= ESP_TEE_TEST_INTR_ITER) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    esp_tee_service_call(3, SS_ESP_TEE_TEST_TEE_INTR_IN_REE, 1, NULL);

    TEST_ASSERT_EQUAL_UINT32(ESP_TEE_TEST_INTR_ITER, tee_intr_count);

    mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");
}
