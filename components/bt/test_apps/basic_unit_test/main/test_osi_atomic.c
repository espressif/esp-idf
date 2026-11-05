/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "osi/atomic.h"

/* Delayed phase: a basic concurrency sanity check. The tight phase below is
 * what catches an unlocked fallback on single-core, so this can stay small. */
#define ATOMIC_TEST_ITERS  200
/* Guest time for the tight phase. A tick must be able to land between the
 * fallback compare and the store; the critical section closes that window. */
#define ATOMIC_TIGHT_US    (500 * 1000)

typedef struct {
    osi_atomic_u32_t *atomic;
    SemaphoreHandle_t done;
    uint32_t *accepted;
} atomic_task_ctx_t;

static void check_word(osi_atomic_u32_t *atomic, uint32_t expected)
{
    TEST_ASSERT_EQUAL_HEX32(expected, osi_atomic_load_n(atomic, __ATOMIC_ACQUIRE));
}

static void check_all_ops(osi_atomic_u32_t *atomic)
{
    uint32_t expected;

    osi_atomic_store_n(atomic, 7, __ATOMIC_RELEASE);
    check_word(atomic, 7);

    expected = 7;
    TEST_ASSERT_TRUE(osi_atomic_compare_exchange_n(atomic, &expected, 11, true,
                                                   __ATOMIC_RELEASE, __ATOMIC_RELAXED));
    TEST_ASSERT_EQUAL_HEX32(7, expected);
    check_word(atomic, 11);

    expected = 1;
    TEST_ASSERT_FALSE(osi_atomic_compare_exchange_n(atomic, &expected, 12, false,
                                                    __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));
    TEST_ASSERT_EQUAL_HEX32(11, expected);
    check_word(atomic, 11);

    TEST_ASSERT_EQUAL_HEX32(11, osi_atomic_fetch_sub(atomic, 5, __ATOMIC_RELEASE));
    check_word(atomic, 6);
}

static uint32_t atomic_increment_once(atomic_task_ctx_t *ctx, bool pause)
{
    uint32_t expected = osi_atomic_load_n(ctx->atomic, __ATOMIC_ACQUIRE);

    /* Let the other task sample the same word before this swap. */
    if (pause) {
        vTaskDelay(1);
    }
    while (!osi_atomic_compare_exchange_n(ctx->atomic, &expected, expected + 1, true,
                                           __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
    }
    return 1;
}

static void atomic_increment_task(void *arg)
{
    atomic_task_ctx_t *ctx = (atomic_task_ctx_t *)arg;
    int64_t tight_end;
    uint32_t expected_tight;
    uint32_t accepted = 0;

    for (int i = 0; i < ATOMIC_TEST_ITERS; i++) {
        accepted += atomic_increment_once(ctx, true);
    }
    /* Pause-free swaps, so the only gap between compare and store is inside
     * compare_exchange. An unlocked fallback loses increments a tick splits;
     * the critical section keeps accepted equal to the word. */
    tight_end = esp_timer_get_time() + ATOMIC_TIGHT_US;
    expected_tight = osi_atomic_load_n(ctx->atomic, __ATOMIC_ACQUIRE);
    while (esp_timer_get_time() < tight_end) {
        for (int n = 0; n < 32; n++) {
            while (!osi_atomic_compare_exchange_n(ctx->atomic, &expected_tight,
                                                   expected_tight + 1, true,
                                                   __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            }
            expected_tight++;
            accepted++;
        }
    }
    *ctx->accepted = accepted;
    xSemaphoreGive(ctx->done);
    vTaskDelete(NULL);
}

static void run_concurrent(bool force_fallback)
{
    osi_atomic_u32_t atomic;
    atomic_task_ctx_t ctx[2];
    uint32_t accepted[2] = {0, 0};
    SemaphoreHandle_t done;
    uint32_t sum;

    osi_atomic_u32_init(&atomic, 0);
    if (force_fallback) {
        atomic.native = false;
    }
    done = xSemaphoreCreateCounting(2, 0);
    TEST_ASSERT_NOT_NULL(done);
    for (int i = 0; i < 2; i++) {
        ctx[i].atomic = &atomic;
        ctx[i].done = done;
        ctx[i].accepted = &accepted[i];
    }

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(atomic_increment_task, "atomic_a", 2048,
                                         &ctx[0], uxTaskPriorityGet(NULL) + 1, NULL));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(atomic_increment_task, "atomic_b", 2048,
                                         &ctx[1], uxTaskPriorityGet(NULL) + 1, NULL));
    /* 200 delays of one tick, plus the tight window and a margin. */
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done, pdMS_TO_TICKS(30000)));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(done, pdMS_TO_TICKS(30000)));
    sum = accepted[0] + accepted[1];
    TEST_ASSERT_GREATER_OR_EQUAL(2 * ATOMIC_TEST_ITERS, sum);
    TEST_ASSERT_EQUAL_UINT32(sum, osi_atomic_load_n(&atomic, __ATOMIC_ACQUIRE));
    vSemaphoreDelete(done);
    /* Let the idle tasks reclaim both deleted task control blocks. */
    vTaskDelay(pdMS_TO_TICKS(20));
}

TEST_CASE("osi atomic builtins update the word", "[osi_atomic]")
{
    osi_atomic_u32_t atomic;

    osi_atomic_u32_init(&atomic, 0);
    check_all_ops(&atomic);
}

TEST_CASE("osi atomic per-object fallback matches the builtins", "[osi_atomic]")
{
    osi_atomic_u32_t atomic;

    osi_atomic_u32_init(&atomic, 0);
    atomic.native = false;
    check_all_ops(&atomic);
}

TEST_CASE("osi atomic CAS serializes concurrent tasks", "[osi_atomic]")
{
    run_concurrent(false);
}

TEST_CASE("osi atomic fallback CAS serializes concurrent tasks", "[osi_atomic]")
{
    run_concurrent(true);
}
