/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"

#if CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_TICKLESS_IDLE_WAITI

#include "unity.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_sleep_internal.h"

#define MHZ (1000 * 1000)

#define TICKLESS_WAITI_TICK_TOLERANCE  2
/* 2 tick + duration * 5% (same as test_pm.c). */
#define TICKLESS_IDLE_TICK_TOLERANCE(expected_ticks) \
    (TICKLESS_WAITI_TICK_TOLERANCE + (expected_ticks) / 20)

static void tickless_waiti_configure_pm(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;
    int xtal_freq = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = xtal_freq,
        .light_sleep_enable = false,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
}

static void tickless_waiti_configure_pm_light_sleep(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;
    int xtal_freq = esp_clk_xtal_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = xtal_freq,
        .light_sleep_enable = true,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
}

static void tickless_waiti_restore_pm(void)
{
    int cur_freq_mhz = esp_clk_cpu_freq() / MHZ;

    esp_pm_config_t pm_config = {
        .max_freq_mhz = cur_freq_mhz,
        .min_freq_mhz = cur_freq_mhz,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
}

static void tickless_waiti_assert_vtaskdelay_tol(int delay_ms, int tick_tol)
{
    vTaskDelay(1);

    int64_t start_us = esp_timer_get_time();
    TickType_t start_tick = xTaskGetTickCount();
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
    TickType_t end_tick = xTaskGetTickCount();
    int64_t elapsed_us = esp_timer_get_time() - start_us;

    int expected_ticks = pdMS_TO_TICKS(delay_ms);
    int actual_ticks = (int)(end_tick - start_tick);
    int time_tol_us = tick_tol * portTICK_PERIOD_MS * 1000;

    printf("delay=%dms elapsed=%dms ticks=%d(expected %d) tol=%d\n",
           delay_ms, (int)(elapsed_us / 1000), actual_ticks, expected_ticks, tick_tol);

    TEST_ASSERT_INT_WITHIN(tick_tol, expected_ticks, actual_ticks);
    TEST_ASSERT_INT32_WITHIN(time_tol_us, delay_ms * 1000, (int32_t)elapsed_us);
}

static void tickless_waiti_assert_vtaskdelay(int delay_ms)
{
    int expected_ticks = pdMS_TO_TICKS(delay_ms);
    tickless_waiti_assert_vtaskdelay_tol(delay_ms, TICKLESS_IDLE_TICK_TOLERANCE(expected_ticks));
}

#if CONFIG_ESP_SLEEP_DEBUG
static void tickless_waiti_assert_light_sleep_ok(esp_sleep_context_t *sleep_ctx, uint32_t ls_cnt_before)
{
    printf("lightsleep_cnt=%lu (was %lu) sleep_request_result=%d\n",
           (unsigned long)sleep_ctx->lightsleep_cnt, (unsigned long)ls_cnt_before,
           (int)sleep_ctx->sleep_request_result);
    TEST_ASSERT_GREATER_THAN(ls_cnt_before, sleep_ctx->lightsleep_cnt);
    TEST_ASSERT_EQUAL(ESP_OK, sleep_ctx->sleep_request_result);
}

static void tickless_waiti_assert_no_light_sleep(esp_sleep_context_t *sleep_ctx, uint32_t ls_cnt_before)
{
    TEST_ASSERT_EQUAL(ls_cnt_before, sleep_ctx->lightsleep_cnt);
}
#endif

typedef struct {
    int delay_ms;
    int loop_count;
    int tick_tol;
    int tick_errors;
    int early_wake_errors;
    SemaphoreHandle_t done;
} tickless_waiti_core_delay_arg_t;

static void tickless_waiti_core_delay_task(void *arg)
{
    tickless_waiti_core_delay_arg_t *params = (tickless_waiti_core_delay_arg_t *)arg;
    const int expected_ticks = pdMS_TO_TICKS(params->delay_ms);
    const int core_id = xPortGetCoreID();

    vTaskDelay((esp_random() % 5) + 1);

    for (int loop = 0; loop < params->loop_count; loop++) {
        if (loop > 0) {
            vTaskDelay(esp_random() % 3);
        }

        vTaskDelay(1);

        TickType_t start_tick = xTaskGetTickCount();
        vTaskDelay(pdMS_TO_TICKS(params->delay_ms));
        int actual_ticks = (int)(xTaskGetTickCount() - start_tick);

        if (actual_ticks < expected_ticks - params->tick_tol) {
            params->early_wake_errors++;
            printf("core%d loop=%d early wake delay=%dms ticks=%d(expected %d)\n",
                   core_id, loop, params->delay_ms, actual_ticks, expected_ticks);
        } else if (actual_ticks > expected_ticks + params->tick_tol) {
            params->tick_errors++;
            printf("core%d loop=%d late delay=%dms ticks=%d(expected %d)\n",
                   core_id, loop, params->delay_ms, actual_ticks, expected_ticks);
        }

        xSemaphoreGive(params->done);
    }

    vTaskDelete(NULL);
}

/* Pin idle work to core0; on dual-core also create a core1 task. */
static void tickless_waiti_run_core_delays(int delay0_ms, int loops0, int delay1_ms, int loops1, int tick_tol)
{
    SemaphoreHandle_t done0 = xSemaphoreCreateCounting(loops0, 0);
    TEST_ASSERT_NOT_NULL(done0);

    tickless_waiti_core_delay_arg_t arg0 = {
        .delay_ms = delay0_ms,
        .loop_count = loops0,
        .tick_tol = tick_tol,
        .done = done0,
    };

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(
                          tickless_waiti_core_delay_task, "waiti0", 4096, &arg0,
                          tskIDLE_PRIORITY + 2, NULL, 0));

#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
    SemaphoreHandle_t done1 = xSemaphoreCreateCounting(loops1, 0);
    TEST_ASSERT_NOT_NULL(done1);

    tickless_waiti_core_delay_arg_t arg1 = {
        .delay_ms = delay1_ms,
        .loop_count = loops1,
        .tick_tol = tick_tol,
        .done = done1,
    };

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreatePinnedToCore(
                          tickless_waiti_core_delay_task, "waiti1", 4096, &arg1,
                          tskIDLE_PRIORITY + 2, NULL, 1));
#else
    (void)delay1_ms;
    (void)loops1;
#endif

    int pending0 = loops0;
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
    int pending1 = loops1;
#endif
    int wait_loops = 0;
    const int max_wait_loops = loops0 + loops1 + 100;

    while (pending0 > 0
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
           || pending1 > 0
#endif
          ) {
        TEST_ASSERT_LESS_THAN(max_wait_loops, wait_loops++);

        if (pending0 > 0 && xSemaphoreTake(done0, pdMS_TO_TICKS(50)) == pdTRUE) {
            pending0--;
            continue;
        }
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
        if (pending1 > 0 && xSemaphoreTake(done1, pdMS_TO_TICKS(50)) == pdTRUE) {
            pending1--;
            continue;
        }
#endif
    }

    TEST_ASSERT_EQUAL(0, arg0.tick_errors);
    TEST_ASSERT_EQUAL(0, arg0.early_wake_errors);
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
    TEST_ASSERT_EQUAL(0, arg1.tick_errors);
    TEST_ASSERT_EQUAL(0, arg1.early_wake_errors);
    vSemaphoreDelete(done1);
#endif
    vSemaphoreDelete(done0);
}

/*
 * Verify: tickless WAITI suppress + LP alarm + xTaskCatchUpTicks keep RTOS tick and
 *         esp_timer wall time aligned with the requested delay.
 *         light_sleep_enable=false forces the WAITI path; sample delays at the tickless
 *         threshold, a short multi-tick park, and a long park that needs multi-tick catch-up.
 */
TEST_CASE("Tickless WAITI tick compensation is accurate", "[pm]")
{
    tickless_waiti_configure_pm();

    const int idle_ms = CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP * portTICK_PERIOD_MS;
    const int delays_ms[] = {
        idle_ms,
        5 * idle_ms,
        50 * idle_ms,
        100 * idle_ms,
        300 * idle_ms,
    };

    for (int i = 0; i < sizeof(delays_ms) / sizeof(delays_ms[0]); i++) {
        tickless_waiti_assert_vtaskdelay(delays_ms[i]);
    }

    tickless_waiti_restore_pm();
}

/*
 * Verify: with light_sleep_enable=true, ESP_PM_NO_LIGHT_SLEEP still selects WAITI (not LS)
 *         and tick compensation remains correct.
 *         acquire NO_LIGHT_SLEEP; assert lightsleep_cnt does not advance across idle; assert
 *         RTOS tick / wall time within WAITI tolerance.
 */
TEST_CASE("Tickless WAITI fallback when light sleep blocked by PM lock", "[pm]")
{
    tickless_waiti_configure_pm_light_sleep();

    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    esp_pm_lock_handle_t lock;
    TEST_ESP_OK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "waiti_lock", &lock));
    TEST_ESP_OK(esp_pm_lock_acquire(lock));

    const int delay_ms = 20 * CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP * portTICK_PERIOD_MS;
    uint32_t ls_cnt = sleep_ctx.lightsleep_cnt;
    tickless_waiti_assert_vtaskdelay(delay_ms);
    tickless_waiti_assert_no_light_sleep(&sleep_ctx, ls_cnt);

    TEST_ESP_OK(esp_pm_lock_release(lock));
    TEST_ESP_OK(esp_pm_lock_delete(lock));
    esp_sleep_set_sleep_context(NULL);
    tickless_waiti_restore_pm();
}

/*
 * Verify: WAITI and esp_pm auto light sleep can alternate without breaking idle timing or
 *         leaving the RTC/LP comparator in a bad state for the next WAITI.
 *
 *  1) Auto LS idle — lightsleep_cnt increases and sleep_request_result == ESP_OK; tick within
 *     light-sleep tolerance (dual-core: both cores idle).
 *  2) Switch to WAITI-only (light_sleep_enable=false) immediately after LS — reclaim path; tick
 *     within WAITI tolerance and lightsleep_cnt unchanged.
 *  3) light_sleep_enable=true again; release/acquire NO_LIGHT_SLEEP to toggle LS <-> WAITI a few
 *     times (fallback accuracy alone is in the lock case above).
 */
TEST_CASE("Tickless WAITI coexists with esp_pm auto light sleep", "[pm]")
{
#if !CONFIG_ESP_SLEEP_DEBUG
    TEST_IGNORE_MESSAGE("requires CONFIG_ESP_SLEEP_DEBUG");
#else
    const int delay_ms = 10 * CONFIG_FREERTOS_IDLE_TIME_BEFORE_SLEEP * portTICK_PERIOD_MS;
    const int ls_tol = TICKLESS_IDLE_TICK_TOLERANCE(pdMS_TO_TICKS(delay_ms));

    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);

    /* 1) Auto light sleep succeeds */
    tickless_waiti_configure_pm_light_sleep();
    uint32_t ls_cnt = sleep_ctx.lightsleep_cnt;
    tickless_waiti_run_core_delays(delay_ms, 2, delay_ms + 40, 2, ls_tol);
    tickless_waiti_assert_light_sleep_ok(&sleep_ctx, ls_cnt);

    /* 2) WAITI reclaim right after LS armed the RTC/LP timer */
    tickless_waiti_configure_pm();
    ls_cnt = sleep_ctx.lightsleep_cnt;
    tickless_waiti_run_core_delays(delay_ms, 2, delay_ms + 40, 2, TICKLESS_WAITI_TICK_TOLERANCE);
    tickless_waiti_assert_no_light_sleep(&sleep_ctx, ls_cnt);

    /* 3) Toggle LS <-> WAITI under light_sleep_enable=true */
    tickless_waiti_configure_pm_light_sleep();
    esp_pm_lock_handle_t no_ls_lock;
    TEST_ESP_OK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "waiti_coexist", &no_ls_lock));

    for (int round = 0; round < 3; round++) {
        printf("coexist toggle round %d\n", round);

        ls_cnt = sleep_ctx.lightsleep_cnt;
        tickless_waiti_run_core_delays(delay_ms, 2, delay_ms + 30, 2, ls_tol);
        tickless_waiti_assert_light_sleep_ok(&sleep_ctx, ls_cnt);

        TEST_ESP_OK(esp_pm_lock_acquire(no_ls_lock));
        ls_cnt = sleep_ctx.lightsleep_cnt;
        tickless_waiti_run_core_delays(delay_ms, 2, delay_ms + 30, 2, TICKLESS_WAITI_TICK_TOLERANCE);
        tickless_waiti_assert_no_light_sleep(&sleep_ctx, ls_cnt);
        TEST_ESP_OK(esp_pm_lock_release(no_ls_lock));
    }

    TEST_ESP_OK(esp_pm_lock_delete(no_ls_lock));
    esp_sleep_set_sleep_context(NULL);
    tickless_waiti_restore_pm();
#endif /* CONFIG_ESP_SLEEP_DEBUG */
}

typedef struct {
    SemaphoreHandle_t fired;
    int64_t first_fire_us;
    int64_t start_time_us;
    int fire_count;
} tickless_waiti_timer_arg_t;

static void tickless_waiti_timer_cb(void *arg)
{
    tickless_waiti_timer_arg_t *ctx = (tickless_waiti_timer_arg_t *)arg;
    int64_t now = esp_timer_get_time();

    if (ctx->fire_count == 0) {
        ctx->first_fire_us = now;
    }
    ctx->fire_count++;
    xSemaphoreGive(ctx->fired);
}

/*
 * Verify: non-LP interrupts (esp_timer) can wake WAITI early / repeatedly, and tick catch-up
 *         still matches the full vTaskDelay.
 *
 *  A) One-shot timer before the delay ends — proves early exit from WFI + Claim/CatchUp still
 *     delivers the full delay length.
 *  B) Periodic timer across a long idle — proves repeated wakeups do not accumulate tick error.
 */
TEST_CASE("Tickless WAITI esp_timer wakeup during idle", "[pm]")
{
    tickless_waiti_configure_pm();

    tickless_waiti_timer_arg_t ctx = {0};
    ctx.fired = xSemaphoreCreateCounting(64, 0);
    TEST_ASSERT_NOT_NULL(ctx.fired);

    esp_timer_handle_t timer;
    esp_timer_create_args_t create_args = {
        .callback = tickless_waiti_timer_cb,
        .arg = &ctx,
        .name = "waiti_timer",
    };
    TEST_ESP_OK(esp_timer_create(&create_args, &timer));

    /* --- A) One-shot early wake --- */
    const int early_delay_ms = 200;
    const int oneshot_ms = 50;
    ctx.fire_count = 0;
    ctx.first_fire_us = 0;
    TEST_ESP_OK(esp_timer_start_once(timer, oneshot_ms * 1000ULL));
    vTaskDelay(1);
    ctx.start_time_us = esp_timer_get_time();
    tickless_waiti_assert_vtaskdelay(early_delay_ms);
    /* One-shot already finished; stop is optional and may return INVALID_STATE. */
    (void)esp_timer_stop(timer);

    int first_fire_ms = (int)((ctx.first_fire_us - ctx.start_time_us) / 1000);
    printf("oneshot: first_fire=%dms (expect ~%d) fires=%d\n",
           first_fire_ms, oneshot_ms, ctx.fire_count);
    TEST_ASSERT_EQUAL(1, ctx.fire_count);
    TEST_ASSERT_INT32_WITHIN(15, oneshot_ms, first_fire_ms);

    /* --- B) Periodic wakes during long idle --- */
    const int delay_ms = 500;
    const int period_ms = 25;
    const int min_fires = (delay_ms / period_ms) - 3;
    ctx.fire_count = 0;
    ctx.first_fire_us = 0;
    while (xSemaphoreTake(ctx.fired, 0) == pdTRUE) {
    }

    TEST_ESP_OK(esp_timer_start_periodic(timer, period_ms * 1000ULL));
    vTaskDelay(1);
    ctx.start_time_us = esp_timer_get_time();
    tickless_waiti_assert_vtaskdelay(delay_ms);
    TEST_ESP_OK(esp_timer_stop(timer));

    first_fire_ms = (int)((ctx.first_fire_us - ctx.start_time_us) / 1000);
    printf("periodic: first_fire=%dms period=%dms fires=%d during %dms\n",
           first_fire_ms, period_ms, ctx.fire_count, delay_ms);
    TEST_ASSERT_GREATER_OR_EQUAL(min_fires, ctx.fire_count);
    TEST_ASSERT_INT32_WITHIN(15, period_ms, first_fire_ms);

    TEST_ESP_OK(esp_timer_delete(timer));
    vSemaphoreDelete(ctx.fired);
    tickless_waiti_restore_pm();
}

#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1

/*
 * Verify: dual-core WAITI with asymmetric idle windows (shared LP comparator partial disarm,
 *         per-core tick suppress, INT_WDT CPU1-idle bypass) keeps each core's delays accurate.
 *         light_sleep_enable=false; core0 short/frequent parks, core1 long/rare parks with
 *         random phase so WAITI windows cross; assert no early/late tick errors on either core.
 *         (LS <-> WAITI dual-core switching is in the coexist case.)
 */
TEST_CASE("Tickless WAITI dual-core asymmetric idle (partial LP disarm)", "[pm]")
{
    tickless_waiti_configure_pm();

    const int delay0_ms = 40;
    const int loop_count0 = 12;
    const int delay1_ms = 180;
    const int loop_count1 = 4;

    printf("asymmetric idle: core0 %dms x %d loops, core1 %dms x %d loops\n",
           delay0_ms, loop_count0, delay1_ms, loop_count1);

    tickless_waiti_run_core_delays(delay0_ms, loop_count0, delay1_ms, loop_count1,
                                   TICKLESS_WAITI_TICK_TOLERANCE);

    vTaskDelay(1);
    tickless_waiti_restore_pm();
}

#endif /* CONFIG_FREERTOS_NUMBER_OF_CORES > 1 */

#endif /* CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_TICKLESS_IDLE_WAITI */
