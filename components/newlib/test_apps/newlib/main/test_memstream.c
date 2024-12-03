/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "test_utils.h"

TEST_CASE("memstream_fseek", "[newlib]")
{
    FILE *stream;
    char *buf;
    size_t len;
    off_t eob;

    stream = open_memstream(&buf, &len);
    fprintf(stream, "Hello Espressif!");
    fflush(stream);
    TEST_ASSERT_EQUAL_STRING("Hello Espressif!", buf);
    TEST_ASSERT_EQUAL_UINT32(strlen("Hello Espressif!"), len);
    eob = ftell(stream);
    fseek(stream, 0, SEEK_SET);
    fprintf(stream, "HELLO");
    fseek(stream, eob, SEEK_SET);
    fclose(stream);
    TEST_ASSERT_EQUAL_STRING("HELLO Espressif!", buf);
    TEST_ASSERT_EQUAL_UINT32(strlen("HELLO Espressif!"), len);
    free(buf);
}

TEST_CASE("memstream_overwrite", "[newlib]")
{
    FILE *stream;
    char *buf;
    size_t len;

    stream = open_memstream(&buf, &len);
    fprintf(stream, "Hello Espressif!");
    fflush(stream);
    TEST_ASSERT_EQUAL_STRING("Hello Espressif!", buf);
    TEST_ASSERT_EQUAL_UINT32(strlen("Hello Espressif!"), len);
    fseek(stream, 0, SEEK_SET);
    fprintf(stream, "HELLO");
    fclose(stream);
    TEST_ASSERT_EQUAL_STRING("HELLO", buf);
    TEST_ASSERT_EQUAL_UINT32(strlen("HELLO"), len);
    free(buf);
}

TEST_CASE("memstream_realloc", "[newlib]")
{
    FILE *stream;
    char *buf;
    size_t len;

    stream = open_memstream(&buf, &len);
    // Initial buf size is BUFSIZ. Try to go out of bounds.
    for (int i = 0; i < BUFSIZ * 2; i++) {
        fputc('A' + (i % ('Z' - 'A')), stream);
    }
    fclose(stream);
    for (int i = 0; i < BUFSIZ * 2; i++) {
        TEST_ASSERT_EQUAL_UINT32('A' + (i % ('Z' - 'A')), buf[i]);
    }
    free(buf);
}
