/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

static const char *TAG = "test_esp_tee_intr";

/* ---------------------------------------------------- Utility functions ---------------------------------------------------- */

typedef struct {
    uint64_t event_count;
} test_queue_element_t;

static QueueHandle_t s_timer_queue;

static gptimer_handle_t gptimer = NULL;

static volatile uint32_t ns_int_count;

static bool IRAM_ATTR test_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_EARLY_LOGI(TAG, "Timer ISR Handler from World %d!", esp_cpu_get_curr_privilege_level());

    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    // Retrieve count value and send to queue
    test_queue_element_t ele = {
        .event_count = edata->count_value
    };
    ns_int_count += 1;

    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

static void IRAM_ATTR timer_evt_task(void *arg)
{
    int record = 3;
    while (1) {
        test_queue_element_t ele;
        if (xQueueReceive(s_timer_queue, &ele, pdMS_TO_TICKS(2000))) {
            ESP_LOGI(TAG, "Timer reloaded, count=%llu", ele.event_count);
            record--;
        } else {
            ESP_LOGW(TAG, "Missed one count event");
        }
        if (!record) {
            break;
        }
    }
}

static void test_timer_init(bool for_ns_world)
{
    s_timer_queue = xQueueCreate(10, sizeof(test_queue_element_t));
    if (!s_timer_queue) {
        ESP_LOGE(TAG, "Creating queue failed");
        return;
    }

    ns_int_count = 0;

    /* Select and initialize basic parameters of the timer */
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = test_timer_on_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, s_timer_queue));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config2 = {
        .reload_count = 0,
        .alarm_count = 250000, // Alarm target = 250ms @ resolution 1MHz
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config2));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    if (for_ns_world) {
        timer_evt_task(NULL);
    }
}

static void test_timer_deinit(void)
{
    ESP_ERROR_CHECK(gptimer_stop(gptimer));
    ESP_ERROR_CHECK(gptimer_disable(gptimer));
    ESP_ERROR_CHECK(gptimer_del_timer(gptimer));

    if (s_timer_queue != NULL) {
        vQueueDelete(s_timer_queue);
        s_timer_queue = NULL;
    }
}

/* ---------------------------------------------------- Test cases ---------------------------------------------------- */

TEST_CASE("Test Secure interrupt in Non-Secure World", "[basic]")
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");

    volatile uint32_t lsecure_int_count = 0;

    /* Pass the variable to secure world to record the interrupt count. */
    esp_tee_service_call(2, SS_ESP_TEE_TEST_INT_COUNT, &lsecure_int_count);
    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "World switch failed");

    esp_tee_service_call(2, SS_ESP_TEE_TEST_TIMER_INIT, true);
    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "World switch failed");

    /* Secure timer initialized.
     * As secure timer interrupt will fire; CPU will switch to secure world.
     * Secure world ISR handler will be called, Secure ISR log can be observed on console.
     * After handling the secure interrupt, CPU will return to non-secure world
     * and resume this loop and wait for the next secure timer interrupt.
     * CPU will wait for TEE_TEST_INT_COUNT number of secure interrupts.
     */
    while (lsecure_int_count < TEE_TEST_INT_COUNT);

    /* After waiting for TEE_TEST_INT_COUNT secure interrupt,
     * disable the secure timer and assert the test status.
     */
    esp_tee_service_call(2, SS_ESP_TEE_TEST_TIMER_INIT, false);
    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "World switch failed");

    /* Assert the number of secure interrupt occurred again. */
    TEST_ASSERT_EQUAL_UINT32(TEE_TEST_INT_COUNT, lsecure_int_count);
}

TEST_CASE("Test Secure interrupt in Secure World", "[basic]")
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");

    uint32_t cnt = esp_tee_service_call(1, SS_ESP_TEE_SECURE_INT_TEST);
    TEST_ASSERT_EQUAL_UINT32(TEE_TEST_INT_COUNT, cnt);

    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "World switch failed");
}

static volatile uint32_t *get_ns_int_count(void)
{
    return &ns_int_count;
}

TEST_CASE("Test Non-secure interrupt in Secure World", "[basic]")
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not W1");

    /* Non-secure world timer initialization. */
    ESP_LOGI(TAG, "Enabling test timer from non-secure world");
    test_timer_init(false);

    volatile uint32_t *volatile lns_int_count;
    lns_int_count = get_ns_int_count();

    /* After non-secure timer initialization,
     * CPU will switch to secure world by using a service call to test API.
     * CPU will wait in finite loop in secure world.
     * And as non-secure timer interrupt fires, CPU will switch to non-secure world.
     * Non-secure world ISR handler will be called, non-secure ISR log can be obsereved on console.
     * After handling the interrupt in non-secure world, CPU will switch back to secure world
     * and wait for the next timer interrupt.
     * In secure world CPU will wait for TEE_TEST_INT_COUNT non-secure interrupts.
     */
    uint32_t val = esp_tee_service_call(2, SS_ESP_TEE_NON_SECURE_INT_TEST, lns_int_count);
    TEST_ASSERT_EQUAL_UINT32(0, val);

    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "World switch failed");

    ESP_LOGI(TAG, "Disabling test timer from non-secure world");
    test_timer_deinit();
}

TEST_CASE("Test Non-secure interrupt in Non-Secure World", "[basic]")
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");

    ESP_LOGI(TAG, "Enabling test timer from non-secure world");
    test_timer_init(true);

    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "Disabling test timer from non-secure world");
    test_timer_deinit();
}
