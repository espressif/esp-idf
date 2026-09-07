/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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

static void test_timer_init_with_cb(gptimer_alarm_cb_t on_alarm, void *arg)
{
    /* Select and initialize basic parameters of the timer */
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TEST_TIMER_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_alarm,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, arg));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config2 = {
        .reload_count = 0,
        .alarm_count = TIMER_ALARM_PERIOD_S * TEST_TIMER_RESOLUTION_HZ,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config2));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

static void test_timer_init(volatile uint32_t *arg)
{
    test_timer_init_with_cb(test_timer_on_alarm_cb, (void *)arg);
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

typedef struct {
    volatile uint32_t intr_count;
    volatile uint32_t accepted_count;
} test_nested_svc_call_ctx_t;

static bool IRAM_ATTR test_nested_svc_call_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    test_nested_svc_call_ctx_t *ctx = (test_nested_svc_call_ctx_t *)user_data;

    /* Issued while the preempted service call sits parked inside the TEE */
    uint32_t ret = esp_tee_service_call(3, SS_ESP_TEE_TEST_SERVICE_ADD, 200, 100);
    if (ret != UINT32_MAX) {
        ctx->accepted_count = ctx->accepted_count + 1;
    }
    ctx->intr_count = ctx->intr_count + 1;

    esp_rom_printf("[mode: %d] Nested service call from ISR (%d) returned 0x%x\n",
                   esp_cpu_get_curr_privilege_level(), ctx->intr_count, ret);
    return true;
}

TEST_CASE("Test nested secure service call from an REE interrupt", "[basic]")
{
    TEST_ASSERT_EQUAL(ESP_CPU_NS_MODE, esp_cpu_get_curr_privilege_level());

    static test_nested_svc_call_ctx_t ctx;
    ctx.intr_count = 0;
    ctx.accepted_count = 0;

    test_timer_init_with_cb(test_nested_svc_call_cb, &ctx);

    /* Runs in the TEE until the ISR above has fired ESP_TEE_TEST_INTR_ITER times */
    uint32_t val = esp_tee_service_call(2, SS_ESP_TEE_TEST_REE_INTR_IN_TEE, &ctx.intr_count);
    TEST_ASSERT_EQUAL_UINT32(0, val);

    test_timer_deinit();

    /* Every call made from the ISR should have been rejected by the TEE */
    TEST_ASSERT_EQUAL_UINT32(0, ctx.accepted_count);

    /* The parked call resumed and completed, so the TEE takes calls again */
    val = esp_tee_service_call(3, SS_ESP_TEE_TEST_SERVICE_ADD, 200, 100);
    TEST_ASSERT_EQUAL_UINT32(300, val);

    TEST_ASSERT_EQUAL(ESP_CPU_NS_MODE, esp_cpu_get_curr_privilege_level());
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
