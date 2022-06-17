/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/param.h>
#include <stdlib.h>
#include "unity.h"
#include "unity_fixture.h"

// unity_fixture_malloc_overrides.h defines 'free' as 'unity_free',
// which can only handle pointers allocated with 'unity_malloc'.
// This test allocates memory via 'posix_memalign' so calling 'free'
// for these pointers causes the heap guards check to fail.
#undef free

TEST_GROUP(misc);

TEST_SETUP(misc)
{
}

TEST_TEAR_DOWN(misc)
{
}

TEST(misc, posix_memalign)
{
    void* outptr;
    int ret;

    ret = posix_memalign(&outptr, 4, 0);
    TEST_ASSERT_EQUAL_PTR(NULL, outptr);
    TEST_ASSERT_EQUAL_INT(ret, 0);

    void* magic = (void*) 0xEFEFEFEF;
    outptr = magic;
    ret = posix_memalign(&outptr, 0x10000000, 64);  // too big alignment - should fail on all targets
    TEST_ASSERT_EQUAL_INT(ret, ENOMEM);
    TEST_ASSERT_EQUAL_PTR(magic, outptr);  // was not modified

    outptr = magic;
    ret = posix_memalign(&outptr, 16, 0x10000000);  // too big size - should fail on all targets
    TEST_ASSERT_EQUAL_INT(ret, ENOMEM);
    TEST_ASSERT_EQUAL_PTR(magic, outptr);  // was not modified

    outptr = magic;
    ret = posix_memalign(&outptr, 16, 64);
    TEST_ASSERT_TRUE(outptr != magic);
    TEST_ASSERT_NOT_NULL(outptr);
    TEST_ASSERT_EQUAL_INT(ret, 0);
    free(outptr);
}

TEST(misc, sysconf)
{
    TEST_ASSERT_NOT_EQUAL(-1, sysconf(_SC_NPROCESSORS_ONLN));
}

TEST(misc, realpath)
{
    char out[PATH_MAX];

    TEST_ASSERT_EQUAL_STRING("/", realpath("/", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("//", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath(".", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("./", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("/.", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("./.", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("..", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("../..", out));
    TEST_ASSERT_EQUAL_STRING("/a", realpath("/a/", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("/a/..", out));
    TEST_ASSERT_EQUAL_STRING("/", realpath("/a/../..", out));
    TEST_ASSERT_EQUAL_STRING("/c", realpath("/a/../b/../c", out));
    TEST_ASSERT_EQUAL_STRING("/abc/def", realpath("/abc/./def/ghi/..", out));
    char* out_new = realpath("/abc/./def/ghi/..", NULL);
    TEST_ASSERT_NOT_NULL(out_new);
    TEST_ASSERT_EQUAL_STRING("/abc/def", out_new);
    free(out_new);
}

TEST_GROUP_RUNNER(misc)
{
    RUN_TEST_CASE(misc, posix_memalign)
    RUN_TEST_CASE(misc, sysconf)
    RUN_TEST_CASE(misc, realpath)
}
