/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "unity.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "esp_wifi_driver.h"
#include "memory_checks.h"
#include "test_wpa_supplicant_common.h"

bool current_task_is_wifi_task(void);
static void eloop_test_reset_hooks(void)
{
}

static const uint32_t timeouts_usec[6] = { 300000, 100000, 300000, 200000, 400000, 100000 };
static const uint32_t timeouts_sec[6] = { 0, 0, 0, 0, 0, 0 };
static int executed_order[6];
static int t;
static struct os_reltime ts;

struct timeout_test_ctx {
    void *sem;
    int count;
    int last_id;
    bool ran_in_wifi_task;
    struct os_reltime armed_at;
    struct os_reltime fired_at;
};

struct chain_test_ctx {
    void *sem;
    struct timeout_test_ctx *followup_ctx;
    struct timeout_test_ctx *cancel_ctx;
    int followup_id;
    int cancel_id;
    int count;
    bool ran_in_wifi_task;
};

struct blocking_waiter_ctx {
    int idx;
    eloop_blocking_timeout_handler handler;
    void *done_sem;
};

static void *blocking_gate;
static void *blocking_done_sem;
static int blocking_results[2];
static bool blocking_in_wifi_task[2];
static int blocking_ids[2] = { 11, 22 };
static int blocking_task_index[2] = { 0, 1 };

static uint32_t reltime_diff_ms(const struct os_reltime *start,
                                const struct os_reltime *end)
{
    struct os_reltime diff;

    os_reltime_sub(end, start, &diff);
    return diff.sec * 1000U + diff.usec / 1000U;
}

static void wait_sem_or_fail(void *sem, uint32_t timeout_ms)
{
    TEST_ASSERT_EQUAL(pdTRUE,
                      os_semphr_take(sem, os_task_ms_to_tick(timeout_ms)));
}

static void expect_sem_timeout(void *sem, uint32_t timeout_ms)
{
    TEST_ASSERT_EQUAL(pdFALSE,
                      os_semphr_take(sem, os_task_ms_to_tick(timeout_ms)));
}

static void eloop_test_wifi_start(void)
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
}

static void eloop_test_wifi_stop(void)
{
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
}

static void reset_timeout_ctx(struct timeout_test_ctx *ctx, void *sem)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->sem = sem;
    ctx->last_id = -1;
    os_get_reltime(&ctx->armed_at);
}

/* there is only single instance of esp_timer so no need of protection */
static void ordered_timeout_callback(void *a, void *b)
{
    int *i = a;
    struct os_time age, now;
    int32_t ms_diff;

    os_get_reltime(&now);
    os_time_sub(&now, &ts, &age);

    ms_diff = (age.sec - timeouts_sec[*i]) * 1000 +
              (age.usec - timeouts_usec[*i]) / 1000;

    if (t > 5) {
        TEST_ASSERT(0);
    }

    /* Allow some jitter from scheduling into pptask context. */
    if (ms_diff > 50) {
        executed_order[t] = -1;
    } else {
        executed_order[t] = *i;
    }
    t++;

    ESP_LOGI("Eloop Test", "timer[%d] ran after %" PRId32 " msec of scheduled time",
             *i, ms_diff);
}

static void generic_timeout_callback(void *eloop_data, void *user_data)
{
    struct timeout_test_ctx *ctx = eloop_data;
    int *id = user_data;

    ctx->count++;
    ctx->last_id = id ? *id : -1;
    ctx->ran_in_wifi_task = current_task_is_wifi_task();
    os_get_reltime(&ctx->fired_at);

    if (ctx->sem) {
        os_semphr_give(ctx->sem);
    }
}

static void chain_timeout_callback(void *eloop_data, void *user_data)
{
    struct chain_test_ctx *ctx = eloop_data;
    (void) user_data;

    ctx->count++;
    ctx->ran_in_wifi_task = current_task_is_wifi_task();
    eloop_cancel_timeout(generic_timeout_callback, ctx->cancel_ctx,
                         &ctx->cancel_id);
    os_get_reltime(&ctx->followup_ctx->armed_at);
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 0, generic_timeout_callback,
                                                ctx->followup_ctx,
                                                &ctx->followup_id));

    if (ctx->sem) {
        os_semphr_give(ctx->sem);
    }
}

static int blocking_callback(void *a, void *b)
{
    int idx = *(int *) a;
    int *id = b;

    blocking_in_wifi_task[idx] = current_task_is_wifi_task();
    os_semphr_take(blocking_gate, OS_BLOCK);
    return *id;
}

static int immediate_blocking_callback(void *a, void *b)
{
    int idx = *(int *) a;
    int *id = b;

    blocking_in_wifi_task[idx] = current_task_is_wifi_task();
    return *id;
}

static void blocking_waiter_task(void *arg)
{
    struct blocking_waiter_ctx *ctx = arg;
    int idx = ctx->idx;

    blocking_results[idx] = eloop_register_timeout_blocking(ctx->handler,
                                                            &blocking_task_index[idx],
                                                            &blocking_ids[idx]);
    os_semphr_give(ctx->done_sem ? ctx->done_sem : blocking_done_sem);
    vTaskDelete(NULL);
}

static void assert_eloop_inactive_api_behavior(void)
{
    struct timeout_test_ctx ctx;
    struct os_reltime remaining = { .sec = 1, .usec = 1 };
    int id = 91;

    memset(&ctx, 0, sizeof(ctx));
    TEST_ASSERT_EQUAL(-1, eloop_register_timeout(0, 1000,
                                                 generic_timeout_callback,
                                                 &ctx, &id));
    TEST_ASSERT_EQUAL(-1, eloop_register_timeout_blocking(immediate_blocking_callback,
                                                          &blocking_task_index[0],
                                                          &blocking_ids[0]));
    TEST_ASSERT_EQUAL(0, eloop_cancel_timeout(generic_timeout_callback,
                                              &ctx, &id));
    TEST_ASSERT_EQUAL(0, eloop_cancel_timeout_one(generic_timeout_callback,
                                                  &ctx, &id, &remaining));
    TEST_ASSERT_EQUAL(0, remaining.sec);
    TEST_ASSERT_EQUAL(0, remaining.usec);
    TEST_ASSERT_EQUAL(0, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx, &id));
    TEST_ASSERT_EQUAL(-1, eloop_deplete_timeout(0, 1000,
                                                generic_timeout_callback,
                                                &ctx, &id));
    TEST_ASSERT_EQUAL(-1, eloop_replenish_timeout(0, 1000,
                                                  generic_timeout_callback,
                                                  &ctx, &id));
}

extern const wifi_osi_funcs_t *wifi_funcs;

TEST_CASE("Test eloop timers run", "[eloop]")
{
    int execution_order[6] = { 1, 5, 3, 0, 2, 4 };
    int index[6] = { 0, 1, 2, 3, 4, 5 };
    int fired_before_stop;

    eloop_test_reset_hooks();
    set_leak_threshold(1200);
    eloop_test_wifi_start();
    eloop_test_wifi_stop();
    /* Reset memory stats since some is leaked during the first initialization */
    test_utils_record_free_mem();

    t = 0;
    eloop_test_wifi_start();
    os_get_reltime(&ts);
    for (int i = 0; i < 6; i++) {
        TEST_ASSERT_EQUAL(0, eloop_register_timeout(timeouts_sec[i], timeouts_usec[i],
                                                    ordered_timeout_callback,
                                                    &index[i], NULL));
    }

    os_sleep(1, 0);
    TEST_ASSERT(memcmp(execution_order, executed_order, 6 * sizeof(int)) == 0);

    t = 0;
    for (int i = 0; i < 6; i++) {
        TEST_ASSERT_EQUAL(0, eloop_register_timeout(timeouts_sec[i], timeouts_usec[i],
                                                    ordered_timeout_callback,
                                                    &index[i], NULL));
    }

    /* Pending async timers are dropped during eloop destroy; only callbacks
     * already dispatched before stop should run. */
    vTaskDelay(250 / portTICK_PERIOD_MS);
    fired_before_stop = t;
    eloop_test_wifi_stop();
    vTaskDelay(300 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(fired_before_stop, t);
    TEST_ASSERT_TRUE(t >= 1);
    TEST_ASSERT_TRUE(t < 6);
}

TEST_CASE("Test eloop rejects APIs before init and after destroy", "[eloop]")
{
    eloop_test_reset_hooks();
    assert_eloop_inactive_api_behavior();

    eloop_test_wifi_start();
    eloop_test_wifi_stop();

    assert_eloop_inactive_api_behavior();
}

TEST_CASE("Test eloop can reinitialize repeatedly", "[eloop]")
{
    for (int i = 0; i < 3; i++) {
        struct timeout_test_ctx ctx;
        void *sem = os_semphr_create(1, 0);
        int user_id = 100 + i;

        TEST_ASSERT_NOT_NULL(sem);
        eloop_test_reset_hooks();
        reset_timeout_ctx(&ctx, sem);
        eloop_test_wifi_start();

        TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 50000,
                                                    generic_timeout_callback,
                                                    &ctx, &user_id));
        wait_sem_or_fail(sem, 1000);
        TEST_ASSERT_EQUAL(1, ctx.count);
        TEST_ASSERT_TRUE(ctx.ran_in_wifi_task);

        eloop_test_wifi_stop();
        os_semphr_delete(sem);
    }
}

TEST_CASE("Test eloop cancel and query APIs", "[eloop]")
{
    struct timeout_test_ctx ctx_a, ctx_b, ctx_c;
    void *sem_a = os_semphr_create(1, 0);
    void *sem_b = os_semphr_create(1, 0);
    void *sem_c = os_semphr_create(1, 0);
    int user_a = 1;
    int user_b = 2;

    TEST_ASSERT_NOT_NULL(sem_a);
    TEST_ASSERT_NOT_NULL(sem_b);
    TEST_ASSERT_NOT_NULL(sem_c);

    eloop_test_reset_hooks();
    reset_timeout_ctx(&ctx_a, sem_a);
    reset_timeout_ctx(&ctx_b, sem_b);
    reset_timeout_ctx(&ctx_c, sem_c);

    eloop_test_wifi_start();
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 400000, generic_timeout_callback,
                                                &ctx_a, &user_a));
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 400000, generic_timeout_callback,
                                                &ctx_b, &user_b));
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 400000, generic_timeout_callback,
                                                &ctx_c, &user_a));

    TEST_ASSERT_EQUAL(1, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_a, &user_a));
    TEST_ASSERT_EQUAL(1, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_b, &user_b));
    TEST_ASSERT_EQUAL(1, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_c, &user_a));

    TEST_ASSERT_EQUAL(2, eloop_cancel_timeout(generic_timeout_callback,
                                              ELOOP_ALL_CTX, &user_a));
    TEST_ASSERT_EQUAL(0, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_a, &user_a));
    TEST_ASSERT_EQUAL(0, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_c, &user_a));
    TEST_ASSERT_EQUAL(1, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_b, &user_b));

    wait_sem_or_fail(sem_b, 1000);
    expect_sem_timeout(sem_a, 50);
    expect_sem_timeout(sem_c, 50);

    TEST_ASSERT_EQUAL(0, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx_b, &user_b));
    TEST_ASSERT_EQUAL(0, ctx_a.count);
    TEST_ASSERT_EQUAL(1, ctx_b.count);
    TEST_ASSERT_EQUAL(0, ctx_c.count);
    TEST_ASSERT_TRUE(ctx_b.ran_in_wifi_task);

    eloop_test_wifi_stop();
    os_semphr_delete(sem_a);
    os_semphr_delete(sem_b);
    os_semphr_delete(sem_c);
}

TEST_CASE("Test eloop cancel_timeout_one returns remaining", "[eloop]")
{
    struct timeout_test_ctx ctx;
    struct os_reltime remaining;
    uint32_t remaining_ms;
    void *sem = os_semphr_create(1, 0);
    int user_id = 7;

    TEST_ASSERT_NOT_NULL(sem);
    eloop_test_reset_hooks();
    reset_timeout_ctx(&ctx, sem);

    eloop_test_wifi_start();
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 600000, generic_timeout_callback,
                                                &ctx, &user_id));
    TEST_ASSERT_EQUAL(1, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx, &user_id));

    vTaskDelay(150 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(1, eloop_cancel_timeout_one(generic_timeout_callback,
                                                  &ctx, &user_id, &remaining));
    remaining_ms = remaining.sec * 1000U + remaining.usec / 1000U;
    TEST_ASSERT_TRUE(remaining_ms >= 250 && remaining_ms <= 550);
    TEST_ASSERT_EQUAL(0, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &ctx, &user_id));
    TEST_ASSERT_EQUAL(0, eloop_cancel_timeout_one(generic_timeout_callback,
                                                  &ctx, &user_id, &remaining));

    expect_sem_timeout(sem, 700);
    TEST_ASSERT_EQUAL(0, ctx.count);

    eloop_test_wifi_stop();
    os_semphr_delete(sem);
}

TEST_CASE("Test eloop deplete and replenish APIs", "[eloop]")
{
    struct timeout_test_ctx deplete_ctx, replenish_ctx, nochange_ctx;
    void *deplete_sem = os_semphr_create(1, 0);
    void *replenish_sem = os_semphr_create(1, 0);
    int deplete_id = 31;
    int replenish_id = 32;
    int nochange_id = 33;
    uint32_t elapsed_ms;

    TEST_ASSERT_NOT_NULL(deplete_sem);
    TEST_ASSERT_NOT_NULL(replenish_sem);

    eloop_test_reset_hooks();
    eloop_test_wifi_start();

    reset_timeout_ctx(&deplete_ctx, deplete_sem);
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 800000, generic_timeout_callback,
                                                &deplete_ctx, &deplete_id));
    TEST_ASSERT_EQUAL(1, eloop_deplete_timeout(0, 150000, generic_timeout_callback,
                                               &deplete_ctx, &deplete_id));
    wait_sem_or_fail(deplete_sem, 600);
    elapsed_ms = reltime_diff_ms(&deplete_ctx.armed_at, &deplete_ctx.fired_at);
    TEST_ASSERT_TRUE(elapsed_ms < 600);
    TEST_ASSERT_TRUE(deplete_ctx.ran_in_wifi_task);

    reset_timeout_ctx(&replenish_ctx, replenish_sem);
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 150000, generic_timeout_callback,
                                                &replenish_ctx, &replenish_id));
    TEST_ASSERT_EQUAL(1, eloop_replenish_timeout(0, 600000, generic_timeout_callback,
                                                 &replenish_ctx, &replenish_id));
    expect_sem_timeout(replenish_sem, 250);
    wait_sem_or_fail(replenish_sem, 1000);
    elapsed_ms = reltime_diff_ms(&replenish_ctx.armed_at, &replenish_ctx.fired_at);
    TEST_ASSERT_TRUE(elapsed_ms >= 450 && elapsed_ms <= 1100);
    TEST_ASSERT_TRUE(replenish_ctx.ran_in_wifi_task);

    reset_timeout_ctx(&nochange_ctx, NULL);
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 200000, generic_timeout_callback,
                                                &nochange_ctx, &nochange_id));
    TEST_ASSERT_EQUAL(0, eloop_deplete_timeout(0, 500000, generic_timeout_callback,
                                               &nochange_ctx, &nochange_id));
    TEST_ASSERT_EQUAL(1, eloop_cancel_timeout(generic_timeout_callback,
                                              &nochange_ctx, &nochange_id));

    reset_timeout_ctx(&nochange_ctx, NULL);
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 600000, generic_timeout_callback,
                                                &nochange_ctx, &nochange_id));
    TEST_ASSERT_EQUAL(0, eloop_replenish_timeout(0, 100000, generic_timeout_callback,
                                                 &nochange_ctx, &nochange_id));
    TEST_ASSERT_EQUAL(1, eloop_cancel_timeout(generic_timeout_callback,
                                              &nochange_ctx, &nochange_id));

    TEST_ASSERT_EQUAL(-1, eloop_deplete_timeout(0, 100000, generic_timeout_callback,
                                                &nochange_ctx, &nochange_id));
    TEST_ASSERT_EQUAL(-1, eloop_replenish_timeout(0, 100000, generic_timeout_callback,
                                                  &nochange_ctx, &nochange_id));

    eloop_test_wifi_stop();
    os_semphr_delete(deplete_sem);
    os_semphr_delete(replenish_sem);
}

TEST_CASE("Test eloop callbacks can schedule and cancel timers", "[eloop]")
{
    struct timeout_test_ctx followup_ctx, cancel_ctx;
    struct chain_test_ctx chain_ctx;
    void *chain_sem = os_semphr_create(1, 0);
    void *followup_sem = os_semphr_create(1, 0);
    void *cancel_sem = os_semphr_create(1, 0);

    TEST_ASSERT_NOT_NULL(chain_sem);
    TEST_ASSERT_NOT_NULL(followup_sem);
    TEST_ASSERT_NOT_NULL(cancel_sem);

    eloop_test_reset_hooks();
    reset_timeout_ctx(&followup_ctx, followup_sem);
    reset_timeout_ctx(&cancel_ctx, cancel_sem);
    memset(&chain_ctx, 0, sizeof(chain_ctx));
    chain_ctx.sem = chain_sem;
    chain_ctx.followup_ctx = &followup_ctx;
    chain_ctx.cancel_ctx = &cancel_ctx;
    chain_ctx.followup_id = 41;
    chain_ctx.cancel_id = 42;

    eloop_test_wifi_start();
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 400000, generic_timeout_callback,
                                                &cancel_ctx, &chain_ctx.cancel_id));
    TEST_ASSERT_EQUAL(0, eloop_register_timeout(0, 100000, chain_timeout_callback,
                                                &chain_ctx, NULL));

    wait_sem_or_fail(chain_sem, 500);
    wait_sem_or_fail(followup_sem, 500);
    expect_sem_timeout(cancel_sem, 500);

    TEST_ASSERT_EQUAL(1, chain_ctx.count);
    TEST_ASSERT_TRUE(chain_ctx.ran_in_wifi_task);
    TEST_ASSERT_EQUAL(1, followup_ctx.count);
    TEST_ASSERT_TRUE(followup_ctx.ran_in_wifi_task);
    TEST_ASSERT_EQUAL(0, cancel_ctx.count);
    TEST_ASSERT_EQUAL(0, eloop_is_timeout_registered(generic_timeout_callback,
                                                     &cancel_ctx, &chain_ctx.cancel_id));

    eloop_test_wifi_stop();
    os_semphr_delete(chain_sem);
    os_semphr_delete(followup_sem);
    os_semphr_delete(cancel_sem);
}

TEST_CASE("Test eloop blocking waiters stay paired", "[eloop]")
{
    struct blocking_waiter_ctx waiter_ctx_a = {
        .idx = 0,
        .handler = blocking_callback,
    };
    struct blocking_waiter_ctx waiter_ctx_b = {
        .idx = 1,
        .handler = blocking_callback,
    };

    blocking_results[0] = 0;
    blocking_results[1] = 0;
    blocking_in_wifi_task[0] = false;
    blocking_in_wifi_task[1] = false;

    eloop_test_reset_hooks();
    eloop_test_wifi_start();

    blocking_gate = os_semphr_create(2, 0);
    blocking_done_sem = os_semphr_create(2, 0);
    TEST_ASSERT_NOT_NULL(blocking_gate);
    TEST_ASSERT_NOT_NULL(blocking_done_sem);
    waiter_ctx_a.done_sem = blocking_done_sem;
    waiter_ctx_b.done_sem = blocking_done_sem;

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(blocking_waiter_task, "eloop_blk_a",
                                          4096, &waiter_ctx_a, 3, NULL));
    vTaskDelay(50 / portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(blocking_waiter_task, "eloop_blk_b",
                                          4096, &waiter_ctx_b, 4, NULL));
    vTaskDelay(50 / portTICK_PERIOD_MS);

    os_semphr_give(blocking_gate);
    os_semphr_give(blocking_gate);

    wait_sem_or_fail(blocking_done_sem, 2000);
    wait_sem_or_fail(blocking_done_sem, 2000);
    TEST_ASSERT_EQUAL(11, blocking_results[0]);
    TEST_ASSERT_EQUAL(22, blocking_results[1]);
    TEST_ASSERT_TRUE(blocking_in_wifi_task[0]);
    TEST_ASSERT_TRUE(blocking_in_wifi_task[1]);

    os_semphr_delete(blocking_done_sem);
    os_semphr_delete(blocking_gate);
    blocking_done_sem = NULL;
    blocking_gate = NULL;

    eloop_test_wifi_stop();
}
