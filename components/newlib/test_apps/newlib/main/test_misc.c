/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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


TEST_CASE("misc - posix_memalign", "[newlib_misc]")
{
    void* outptr = NULL;
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

TEST_CASE("misc - sysconf", "[newlib_misc]")
{
    TEST_ASSERT_NOT_EQUAL(-1, sysconf(_SC_NPROCESSORS_ONLN));
}

TEST_CASE("misc - realpath", "[newlib_misc]")
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
