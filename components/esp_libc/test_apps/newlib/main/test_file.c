/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "esp_vfs.h"
#include "unity.h"

/* This test checks that st_blksize value set in struct stat correctly affects the
 * FILE structure:
 * - _blksize field should be equal to st_blksize
 * - buffer size should be equal to st_blksize if it is nonzero, and __BUFSIZ__ otherwise.
 * This is more of an integration test since some of the functions responsible for this are
 * in ROM, and have been built without HAVE_BLKSIZE feature for the ESP32 chip.
 */

typedef struct {
    unsigned blksize;
} blksize_test_ctx_t;

static int blksize_test_open(void* ctx, const char * path, int flags, int mode)
{
    return 1;
}

static int blksize_test_fstat(void* ctx, int fd, struct stat * st)
{
    blksize_test_ctx_t* test_ctx = (blksize_test_ctx_t*) ctx;
    memset(st, 0, sizeof(*st));
    st->st_mode = S_IFREG;
    st->st_blksize = test_ctx->blksize;
    return 0;
}

static ssize_t blksize_test_write(void* ctx, int fd, const void * data, size_t size)
{
    return size;
}

TEST_CASE("file - blksize", "[newlib_file]")
{
    FILE* f;
    blksize_test_ctx_t ctx = {};
    const char c = 42;
    const esp_vfs_t desc = {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .open_p = blksize_test_open,
        .fstat_p = blksize_test_fstat,
        .write_p = blksize_test_write,
    };

    TEST_ESP_OK(esp_vfs_register("/test", &desc, &ctx));

    /* test with zero st_blksize (=not set) */
    ctx.blksize = 0;
    f = fopen("/test/path", "w");
    TEST_ASSERT_NOT_NULL(f);
    fwrite(&c, 1, 1, f);
    TEST_ASSERT_EQUAL(0, f->_blksize);
    TEST_ASSERT_EQUAL(__BUFSIZ__, __fbufsize(f));
    fclose(f);

    /* test with non-zero st_blksize */
    ctx.blksize = 4096;
    f = fopen("/test/path", "w");
    TEST_ASSERT_NOT_NULL(f);
    fwrite(&c, 1, 1, f);
    TEST_ASSERT_EQUAL(ctx.blksize, f->_blksize);
    TEST_ASSERT_EQUAL(ctx.blksize, __fbufsize(f));
    fclose(f);

    TEST_ESP_OK(esp_vfs_unregister("/test"));
}
