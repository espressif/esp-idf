/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Intrusive slist. Nodes stay with the caller. remove/free use the callback
 * stored at slist_new.
 */

#include <stddef.h>

#include "unity.h"

#include "osi/slist.h"

static int s_stored;

static void stored_free(slist_node_t *node)
{
    (void)node;
    s_stored++;
}

typedef struct {
    slist_t *list;
    slist_node_t *drop;
    int seen;
} walk_ctx_t;

typedef struct {
    slist_node_t *stop;
    slist_node_t *first;
    int seen;
} stop_ctx_t;

static bool drop_current(slist_node_t *node, void *context)
{
    walk_ctx_t *ctx = context;

    ctx->seen++;
    if (node == ctx->drop) {
        TEST_ASSERT_TRUE(slist_delete(ctx->list, node));
    }
    return true;
}

static bool stop_at(slist_node_t *node, void *context)
{
    stop_ctx_t *ctx = context;

    if (ctx->seen == 0) {
        ctx->first = node;
    }
    ctx->seen++;
    return node != ctx->stop;
}

TEST_CASE("slist links nodes in insert order", "[slist]")
{
    slist_node_t a;
    slist_node_t b;
    slist_node_t c;
    slist_node_t d;
    slist_node_t missing;
    slist_t *list;
    const slist_node_t *node;
    int count;

    list = slist_new(NULL);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(slist_is_empty(list));
    TEST_ASSERT_EQUAL(0, slist_length(list));
    TEST_ASSERT_EQUAL_PTR(slist_end(list), slist_begin(list));
    TEST_ASSERT_NULL(slist_begin(list));

    TEST_ASSERT_TRUE(slist_append(list, &a));
    TEST_ASSERT_TRUE(slist_prepend(list, &c));
    TEST_ASSERT_TRUE(slist_prepend(list, &b));
    TEST_ASSERT_TRUE(slist_append(list, &d));

    TEST_ASSERT_EQUAL_PTR(&b, slist_front(list));
    TEST_ASSERT_EQUAL_PTR(&c, slist_next(slist_begin(list)));
    TEST_ASSERT_EQUAL_PTR(&a, slist_next(&c));
    TEST_ASSERT_EQUAL_PTR(&d, slist_next(&a));
    TEST_ASSERT_NULL(slist_next(&d));
    TEST_ASSERT_TRUE(slist_contains(list, &a));
    TEST_ASSERT_FALSE(slist_contains(list, &missing));
    TEST_ASSERT_EQUAL(4, slist_length(list));

    count = 0;
    for (node = slist_begin(list); node != slist_end(list); node = slist_next(node)) {
        count++;
    }
    TEST_ASSERT_EQUAL(4, count);

    slist_free(list);
}

TEST_CASE("slist remove runs the stored callback and delete does not", "[slist]")
{
    slist_node_t a;
    slist_node_t b;
    slist_node_t c;
    slist_node_t missing;
    slist_t *list;

    list = slist_new(stored_free);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(slist_append(list, &a));
    TEST_ASSERT_TRUE(slist_append(list, &b));
    TEST_ASSERT_TRUE(slist_append(list, &c));

    s_stored = 0;
    TEST_ASSERT_TRUE(slist_delete(list, &c));
    TEST_ASSERT_EQUAL(0, s_stored);
    TEST_ASSERT_FALSE(slist_contains(list, &c));
    TEST_ASSERT_NULL(slist_next(&b));

    TEST_ASSERT_TRUE(slist_remove(list, &a));
    TEST_ASSERT_EQUAL(1, s_stored);
    TEST_ASSERT_EQUAL_PTR(&b, slist_front(list));
    TEST_ASSERT_EQUAL(1, slist_length(list));

    TEST_ASSERT_FALSE(slist_remove(list, &missing));
    TEST_ASSERT_FALSE(slist_delete(list, &a));
    TEST_ASSERT_EQUAL(1, s_stored);
    TEST_ASSERT_EQUAL(1, slist_length(list));

    slist_free(list);
    TEST_ASSERT_EQUAL(2, s_stored);
}

TEST_CASE("slist delete_after unlinks with a known predecessor", "[slist]")
{
    slist_node_t a;
    slist_node_t b;
    slist_node_t c;
    slist_t *list;

    list = slist_new(NULL);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(slist_append(list, &a));
    TEST_ASSERT_TRUE(slist_append(list, &b));
    TEST_ASSERT_TRUE(slist_append(list, &c));

    slist_delete_after(list, &b, &c);
    TEST_ASSERT_FALSE(slist_contains(list, &c));
    TEST_ASSERT_NULL(slist_next(&b));
    TEST_ASSERT_EQUAL(2, slist_length(list));

    slist_delete_after(list, NULL, &a);
    TEST_ASSERT_EQUAL_PTR(&b, slist_front(list));
    TEST_ASSERT_EQUAL(1, slist_length(list));

    slist_delete_after(list, NULL, &b);
    TEST_ASSERT_TRUE(slist_is_empty(list));
    TEST_ASSERT_EQUAL(0, slist_length(list));

    slist_free(list);
}

TEST_CASE("slist foreach keeps walking after the current node is unlinked", "[slist]")
{
    slist_node_t b;
    slist_node_t c;
    slist_node_t a;
    slist_node_t d;
    slist_t *list;
    walk_ctx_t ctx;
    stop_ctx_t stop;

    list = slist_new(NULL);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(slist_append(list, &b));
    TEST_ASSERT_TRUE(slist_append(list, &c));
    TEST_ASSERT_TRUE(slist_append(list, &a));
    TEST_ASSERT_TRUE(slist_append(list, &d));

    ctx.list = list;
    ctx.drop = &c;
    ctx.seen = 0;
    TEST_ASSERT_NULL(slist_foreach(list, drop_current, &ctx));
    TEST_ASSERT_EQUAL(4, ctx.seen);
    TEST_ASSERT_FALSE(slist_contains(list, &c));
    TEST_ASSERT_EQUAL(3, slist_length(list));
    TEST_ASSERT_EQUAL_PTR(&a, slist_next(&b));
    TEST_ASSERT_EQUAL_PTR(&d, slist_next(&a));

    stop.stop = &a;
    stop.first = NULL;
    stop.seen = 0;
    TEST_ASSERT_EQUAL_PTR(&a, slist_foreach(list, stop_at, &stop));
    TEST_ASSERT_EQUAL_PTR(&b, stop.first);
    TEST_ASSERT_EQUAL(2, stop.seen);

    slist_free(list);
}

TEST_CASE("slist free runs the stored callback once per linked node", "[slist]")
{
    slist_node_t a;
    slist_node_t b;
    slist_node_t c;
    slist_t *list;
    const slist_node_t *node;
    size_t linked;

    list = slist_new(stored_free);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(slist_append(list, &a));
    TEST_ASSERT_TRUE(slist_append(list, &b));

    s_stored = 0;
    slist_free(list);
    TEST_ASSERT_EQUAL(2, s_stored);

    list = slist_new(stored_free);
    TEST_ASSERT_NOT_NULL(list);
    TEST_ASSERT_TRUE(slist_append(list, &a));
    TEST_ASSERT_TRUE(slist_append(list, &c));
    s_stored = 0;
    TEST_ASSERT_TRUE(slist_remove(list, &a));
    TEST_ASSERT_EQUAL(1, s_stored);
    linked = 0;
    for (node = slist_begin(list); node != slist_end(list); node = slist_next(node)) {
        linked++;
    }
    TEST_ASSERT_EQUAL(1, linked);
    TEST_ASSERT_EQUAL(linked, slist_length(list));
    slist_free(list);
    TEST_ASSERT_EQUAL(2, s_stored);
}

TEST_CASE("slist NULL list is a no-op for free", "[slist]")
{
    s_stored = 0;
    slist_free(NULL);
    TEST_ASSERT_EQUAL(0, s_stored);
}
