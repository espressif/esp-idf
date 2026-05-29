/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Unit tests for http_header_generate_string() — cover the empty list,
 * all-headers-fit and pagination paths.
 */

#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "http_header.h"

TEST_CASE("http_header_generate_string: empty list returns 0 without touching buffer", "[http_header]")
{
    http_header_handle_t hdr = http_header_init();
    TEST_ASSERT_NOT_NULL(hdr);

    char buf[64] = "untouched";
    int buf_len = sizeof(buf);
    int ret = http_header_generate_string(hdr, 0, buf, &buf_len);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT((int)sizeof(buf), buf_len);
    TEST_ASSERT_EQUAL_STRING("untouched", buf);

    http_header_destroy(hdr);
}

TEST_CASE("http_header_generate_string: all headers fit in one call", "[http_header]")
{
    http_header_handle_t hdr = http_header_init();
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT_EQUAL(ESP_OK, http_header_set(hdr, "K1", "V1"));
    TEST_ASSERT_EQUAL(ESP_OK, http_header_set(hdr, "K2", "V2"));

    char buf[64] = {0};
    int buf_len = sizeof(buf);
    int ret = http_header_generate_string(hdr, 0, buf, &buf_len);

    const char *expected = "K1: V1\r\nK2: V2\r\n\r\n";
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_STRING(expected, buf);
    TEST_ASSERT_EQUAL_INT((int)strlen(expected), buf_len);

    http_header_destroy(hdr);
}

TEST_CASE("http_header_generate_string: pagination across two calls", "[http_header]")
{
    http_header_handle_t hdr = http_header_init();
    TEST_ASSERT_NOT_NULL(hdr);
    TEST_ASSERT_EQUAL(ESP_OK, http_header_set(hdr, "K1", "V1"));
    TEST_ASSERT_EQUAL(ESP_OK, http_header_set(hdr, "K2", "V2"));

    /* 12-byte buffer fits one 8-byte header, but not both headers + terminator. */
    char buf[12] = {0};
    int buf_len = sizeof(buf);
    int ret = http_header_generate_string(hdr, 0, buf, &buf_len);

    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_STRING("K1: V1\r\n", buf);
    TEST_ASSERT_EQUAL_INT((int)strlen("K1: V1\r\n"), buf_len);

    memset(buf, 0, sizeof(buf));
    buf_len = sizeof(buf);
    ret = http_header_generate_string(hdr, 1, buf, &buf_len);

    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_STRING("K2: V2\r\n\r\n", buf);
    TEST_ASSERT_EQUAL_INT((int)strlen("K2: V2\r\n\r\n"), buf_len);

    http_header_destroy(hdr);
}
