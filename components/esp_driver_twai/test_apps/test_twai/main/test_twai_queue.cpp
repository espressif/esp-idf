/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdatomic.h>
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_twai_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_private/twai_frame_queue.h"

typedef struct {
    int value;
    uint32_t priority;
} twai_frame_queue_test_item_t;

static void test_twai_frame_queue_print_header(const char *title)
{
    printf("\n%s\n", title);
    printf("+-------+----------+\n");
    printf("| value | priority |\n");
    printf("+-------+----------+\n");
}

static void test_twai_frame_queue_print_row(const twai_frame_queue_test_item_t *item)
{
    printf("| %5d | %8" PRIu32 " |\n", item->value, item->priority);
}

static void test_twai_frame_queue_print_footer(void)
{
    printf("+-------+----------+\n");
}

static void test_twai_frame_queue_pop_in_order(twai_frame_queue_t queue, size_t expected_count, const char *title)
{
    const twai_frame_t *item = NULL;
    const twai_frame_queue_test_item_t *last_item = NULL;

    test_twai_frame_queue_print_header(title);
    for (size_t i = 0; i < expected_count; i++) {
        TEST_ESP_OK(twai_frame_queue_pop(queue, &item));
        const twai_frame_queue_test_item_t *cur_item = (const twai_frame_queue_test_item_t *) item;

        test_twai_frame_queue_print_row(cur_item);
        if (last_item) {
            TEST_ASSERT_LESS_OR_EQUAL(last_item->priority, cur_item->priority);
            if (cur_item->priority == last_item->priority) {
                TEST_ASSERT_GREATER_THAN(last_item->value, cur_item->value);
            }
        }

        TEST_ASSERT_EQUAL(expected_count - i - 1, twai_frame_queue_get_count(queue));
        last_item = cur_item;
    }
    test_twai_frame_queue_print_footer();
}

TEST_CASE("test twai frame priority queue", "[twai]")
{
    twai_frame_queue_t test_q = NULL;
    const twai_frame_t *item = NULL;
    twai_frame_queue_test_item_t test_items[] = {
        { 0, 1 }, { 1, 4 }, { 2, 2 }, { 3, 5 },
        { 4, 3 }, { 5, 5 }, { 6, 1 }, { 7, 4 },
        { 8, 2 }, { 9, 3 }, { 10, 5 }, { 11, 0 },
        { 12, 4 }, { 13, 2 }, { 14, 3 }, { 15, 1 },
    };
    twai_frame_queue_test_item_t overflow = { 16, 6 };
    const size_t capacity = sizeof(test_items) / sizeof(test_items[0]);

    TEST_ESP_OK(twai_frame_queue_new(&test_q, capacity, MALLOC_CAP_DEFAULT));
    TEST_ASSERT_EQUAL(0, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(capacity, twai_frame_queue_get_free_space(test_q));

    test_twai_frame_queue_print_header("Push all test data");
    for (size_t i = 0; i < capacity; i++) {
        TEST_ESP_OK(twai_frame_queue_push(test_q, (const twai_frame_t *) &test_items[i], test_items[i].priority, 0));
        test_twai_frame_queue_print_row(&test_items[i]);
        TEST_ASSERT_EQUAL(i + 1, twai_frame_queue_get_count(test_q));
        TEST_ASSERT_EQUAL(capacity - i - 1, twai_frame_queue_get_free_space(test_q));
    }
    test_twai_frame_queue_print_footer();

    TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, twai_frame_queue_push(test_q, (const twai_frame_t *) &overflow, 6, 0));
    TEST_ASSERT_EQUAL(capacity, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(0, twai_frame_queue_get_free_space(test_q));

    test_twai_frame_queue_pop_in_order(test_q, capacity, "Pop all test data");
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, twai_frame_queue_pop(test_q, &item));

    TEST_ASSERT_EQUAL(0, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(capacity, twai_frame_queue_get_free_space(test_q));
    TEST_ESP_OK(twai_frame_queue_del(test_q));
}

TEST_CASE("test twai queue mixed pop and push", "[twai]")
{
    twai_frame_queue_t test_q = NULL;
    const twai_frame_t *item = NULL;
    twai_frame_queue_test_item_t test_items[] = {
        { 0, 1 },
        { 1, 5 },
        { 2, 3 },
        { 3, 5 },
        { 4, 2 },
        { 5, 4 },
        { 6, 6 },
        { 7, 3 },
        { 8, 4 },
    };
    const size_t first_push_count = 6;
    const size_t capacity = 8;
    const size_t remaining_count = sizeof(test_items) / sizeof(test_items[0]) - 2;

    TEST_ESP_OK(twai_frame_queue_new(&test_q, capacity, MALLOC_CAP_DEFAULT));

    test_twai_frame_queue_print_header("Initial push before refill");
    for (size_t i = 0; i < first_push_count; i++) {
        TEST_ESP_OK(twai_frame_queue_push(test_q, (const twai_frame_t *) &test_items[i], test_items[i].priority, 0));
        test_twai_frame_queue_print_row(&test_items[i]);
    }
    test_twai_frame_queue_print_footer();
    TEST_ASSERT_EQUAL(first_push_count, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(capacity - first_push_count, twai_frame_queue_get_free_space(test_q));

    test_twai_frame_queue_print_header("Pop before refill");
    TEST_ESP_OK(twai_frame_queue_pop(test_q, &item));
    test_twai_frame_queue_print_row((const twai_frame_queue_test_item_t *) item);
    TEST_ASSERT_EQUAL_PTR(&test_items[1], item);
    TEST_ESP_OK(twai_frame_queue_pop(test_q, &item));
    test_twai_frame_queue_print_row((const twai_frame_queue_test_item_t *) item);
    TEST_ASSERT_EQUAL_PTR(&test_items[3], item);
    test_twai_frame_queue_print_footer();
    TEST_ASSERT_EQUAL(4, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(4, twai_frame_queue_get_free_space(test_q));

    test_twai_frame_queue_print_header("Push after partial pop");
    for (size_t i = first_push_count; i < sizeof(test_items) / sizeof(test_items[0]); i++) {
        TEST_ESP_OK(twai_frame_queue_push(test_q, (const twai_frame_t *) &test_items[i], test_items[i].priority, 0));
        test_twai_frame_queue_print_row(&test_items[i]);
    }
    test_twai_frame_queue_print_footer();
    TEST_ASSERT_EQUAL(remaining_count, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(1, twai_frame_queue_get_free_space(test_q));

    test_twai_frame_queue_pop_in_order(test_q, remaining_count, "Pop after refill");
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, twai_frame_queue_pop(test_q, &item));
    TEST_ASSERT_EQUAL(0, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(capacity, twai_frame_queue_get_free_space(test_q));
    TEST_ESP_OK(twai_frame_queue_del(test_q));
}

typedef struct {
    twai_frame_queue_t queue;
    twai_frame_queue_test_item_t *items;
    bool *seen;
    size_t total;
    atomic_int push_tasks_done;
    atomic_int pop_count;
    atomic_bool failed;
} twai_frame_queue_concurrent_ctx_t;

typedef struct {
    twai_frame_queue_concurrent_ctx_t *ctx;
    size_t begin;
    size_t end;
} twai_frame_queue_push_args_t;

static void twai_frame_queue_push_task(void *arg)
{
    twai_frame_queue_push_args_t *args = (twai_frame_queue_push_args_t *) arg;
    twai_frame_queue_concurrent_ctx_t *ctx = args->ctx;

    printf("%s started\n", pcTaskGetName(NULL));
    for (size_t i = args->begin; i < args->end; i++) {
        if (atomic_load(&ctx->failed)) {
            vTaskDelete(NULL);
        }
        TEST_ESP_OK(twai_frame_queue_push(ctx->queue, (const twai_frame_t *) &ctx->items[i], ctx->items[i].priority, portMAX_DELAY));
        vTaskDelay(1);
    }
    atomic_fetch_add(&ctx->push_tasks_done, 1);
    vTaskDelete(NULL);
}

static void twai_frame_queue_pop_task(void *arg)
{
    twai_frame_queue_concurrent_ctx_t *ctx = (twai_frame_queue_concurrent_ctx_t *) arg;

    printf("%s started\n", pcTaskGetName(NULL));
    const twai_frame_t *item = NULL;

    while (atomic_load(&ctx->pop_count) < (int) ctx->total) {
        if (twai_frame_queue_pop(ctx->queue, &item) == ESP_OK) {
            const twai_frame_queue_test_item_t *cur = (const twai_frame_queue_test_item_t *) item;
            int value = cur->value;

            if (value < 0 || (size_t) value >= ctx->total || ctx->seen[value]) {
                atomic_store(&ctx->failed, true);
                vTaskDelete(NULL);
            }
            ctx->seen[value] = true;
            atomic_fetch_add(&ctx->pop_count, 1);
        } else if (atomic_load(&ctx->push_tasks_done) == 2 && twai_frame_queue_get_count(ctx->queue) == 0) {
            break;
        } else {
            vTaskDelay(1);
        }
    }
    vTaskDelete(NULL);
}

TEST_CASE("test twai queue concurrent push and pop", "[twai]")
{
    twai_frame_queue_t test_q = NULL;
    const size_t item_count = 500;
    const size_t capacity = 4;
    twai_frame_queue_test_item_t *test_items = (twai_frame_queue_test_item_t *) malloc(item_count * sizeof(twai_frame_queue_test_item_t));
    bool seen[item_count] = {};
    twai_frame_queue_concurrent_ctx_t ctx = {};
    twai_frame_queue_push_args_t push_args0 = {};
    twai_frame_queue_push_args_t push_args1 = {};

    for (size_t i = 0; i < item_count; i++) {
        test_items[i].value = (int) i;
        test_items[i].priority = (uint32_t)((i * 7 + 3) % 6);
    }

    ctx.items = test_items;
    ctx.seen = seen;
    ctx.total = item_count;

    push_args0.ctx = &ctx;
    push_args0.begin = 0;
    push_args0.end = item_count / 2;

    push_args1.ctx = &ctx;
    push_args1.begin = item_count / 2;
    push_args1.end = item_count;

    TEST_ESP_OK(twai_frame_queue_new(&test_q, capacity, MALLOC_CAP_DEFAULT));
    ctx.queue = test_q;

    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(twai_frame_queue_pop_task, "twai_q_pop", 4096, &ctx, 5, NULL));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(twai_frame_queue_push_task, "twai_q_push0", 4096, &push_args0, 4, NULL));
    TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(twai_frame_queue_push_task, "twai_q_push1", 4096, &push_args1, 4, NULL));

    TickType_t start = xTaskGetTickCount();
    while (atomic_load(&ctx.push_tasks_done) < 2 || atomic_load(&ctx.pop_count) < (int) item_count) {
        TEST_ASSERT_FALSE(atomic_load(&ctx.failed));
        if ((xTaskGetTickCount() - start) > pdMS_TO_TICKS(5000)) {
            TEST_FAIL_MESSAGE("concurrent push/pop timed out");
        }
        vTaskDelay(1);
    }

    TEST_ASSERT_FALSE(atomic_load(&ctx.failed));
    TEST_ASSERT_EQUAL(2, atomic_load(&ctx.push_tasks_done));
    printf("pop %d items\n", atomic_load(&ctx.pop_count));
    TEST_ASSERT_EQUAL((int) item_count, atomic_load(&ctx.pop_count));
    TEST_ASSERT_EQUAL(0, twai_frame_queue_get_count(test_q));
    TEST_ASSERT_EQUAL(capacity, twai_frame_queue_get_free_space(test_q));
    for (size_t i = 0; i < item_count; i++) {
        TEST_ASSERT_TRUE(seen[i]);
    }

    printf("test finished\n");
    free(test_items);
    TEST_ESP_OK(twai_frame_queue_del(test_q));
    vTaskDelay(10); // wait for tasks to be deleted
}
