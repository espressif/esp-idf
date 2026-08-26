/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "esp_err.h"
#include "unity.h"

/**
 * Tests for esp_err_to_name() and esp_err_to_name_r().
 *
 * These verify that the link-time error code table is populated correctly
 * and that lookups work for well-known codes defined in esp_err.h.
 */

TEST_CASE("esp_err_to_name returns correct string for ESP_OK", "[esp_err]")
{
    const char *name = esp_err_to_name(ESP_OK);
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("ESP_OK", name);
}

TEST_CASE("esp_err_to_name returns correct string for ESP_FAIL", "[esp_err]")
{
    const char *name = esp_err_to_name(ESP_FAIL);
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("ESP_FAIL", name);
}

TEST_CASE("esp_err_to_name returns correct strings for common error codes", "[esp_err]")
{
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_NO_MEM", esp_err_to_name(ESP_ERR_NO_MEM));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_ARG", esp_err_to_name(ESP_ERR_INVALID_ARG));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_STATE", esp_err_to_name(ESP_ERR_INVALID_STATE));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_SIZE", esp_err_to_name(ESP_ERR_INVALID_SIZE));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_NOT_FOUND", esp_err_to_name(ESP_ERR_NOT_FOUND));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_NOT_SUPPORTED", esp_err_to_name(ESP_ERR_NOT_SUPPORTED));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_TIMEOUT", esp_err_to_name(ESP_ERR_TIMEOUT));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_RESPONSE", esp_err_to_name(ESP_ERR_INVALID_RESPONSE));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_CRC", esp_err_to_name(ESP_ERR_INVALID_CRC));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_VERSION", esp_err_to_name(ESP_ERR_INVALID_VERSION));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_INVALID_MAC", esp_err_to_name(ESP_ERR_INVALID_MAC));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_NOT_FINISHED", esp_err_to_name(ESP_ERR_NOT_FINISHED));
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_NOT_ALLOWED", esp_err_to_name(ESP_ERR_NOT_ALLOWED));
}

TEST_CASE("esp_err_to_name returns fallback for unknown code", "[esp_err]")
{
    /* Use a code that should never be defined */
    const char *name = esp_err_to_name(0xFFFF);
    TEST_ASSERT_NOT_NULL(name);
    /* Should return "ERROR" when lookup is enabled, "UNKNOWN ERROR" otherwise */
    TEST_ASSERT_TRUE(strcmp(name, "ERROR") == 0 || strcmp(name, "UNKNOWN ERROR") == 0);
}

TEST_CASE("esp_err_to_name_r writes correct string to buffer", "[esp_err]")
{
    char buf[64];
    const char *result = esp_err_to_name_r(ESP_ERR_NO_MEM, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, result);
    TEST_ASSERT_EQUAL_STRING("ESP_ERR_NO_MEM", buf);
}

TEST_CASE("esp_err_to_name_r truncates to buffer size", "[esp_err]")
{
    char buf[8]; /* Too small for "ESP_ERR_NO_MEM" (14 chars + null) */
    const char *result = esp_err_to_name_r(ESP_ERR_NO_MEM, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, result);
    /* strlcpy should truncate and null-terminate */
    TEST_ASSERT_EQUAL_STRING("ESP_ERR", buf);
}

TEST_CASE("esp_err_to_name_r formats unknown code with hex value", "[esp_err]")
{
    char buf[64];
    const char *result = esp_err_to_name_r(0xFFFF, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, result);
    /* Should contain the hex value */
    TEST_ASSERT_NOT_NULL(strstr(buf, "0xffff"));
}
