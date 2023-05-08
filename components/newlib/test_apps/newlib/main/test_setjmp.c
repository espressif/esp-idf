/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <setjmp.h>
#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "esp_system.h"


typedef struct {
    jmp_buf jmp_env;
    uint32_t retval;
    volatile bool inner_called;
} setjmp_test_ctx_t;

static __attribute__((noreturn)) void inner(setjmp_test_ctx_t *ctx)
{
    printf("inner, retval=0x%" PRIx32 "\n", ctx->retval);
    ctx->inner_called = true;
    longjmp(ctx->jmp_env, ctx->retval);
    TEST_FAIL_MESSAGE("Should not reach here");
}

TEST_CASE("setjmp and longjmp", "[newlib]")
{
    const uint32_t expected = 0x12345678;
    setjmp_test_ctx_t ctx = {
        .retval = expected
    };
    uint32_t ret = setjmp(ctx.jmp_env);
    if (!ctx.inner_called) {
        TEST_ASSERT_EQUAL(0, ret);
        inner(&ctx);
    } else {
        TEST_ASSERT_EQUAL(expected, ret);
    }
    TEST_ASSERT(ctx.inner_called);
}
