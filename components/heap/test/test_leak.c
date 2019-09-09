/*
 Tests for a leak tag
*/

#include <stdio.h>
#include "unity.h"
#include "esp_heap_caps_init.h"
#include "esp_system.h"
#include <stdlib.h>


static char* check_calloc(int size)
{
    char *arr = calloc(size, sizeof(char));
    TEST_ASSERT_NOT_NULL(arr);
    return arr;
}

TEST_CASE("Check for leaks (no leak)", "[heap]")
{
    char *arr = check_calloc(7000);
    free(arr);
}

TEST_CASE("Check for leaks (leak)", "[heap][ignore]")
{
    check_calloc(7000);
}

TEST_CASE("Not check for leaks", "[heap][leaks]")
{
    check_calloc(7000);
}

TEST_CASE("Set a leak level = 7016", "[heap][leaks=7016]")
{
    check_calloc(7000);
}

static void test_fn(void)
{
    check_calloc(7000);
}

TEST_CASE_MULTIPLE_STAGES("Not check for leaks in MULTIPLE_STAGES mode", "[heap][leaks]", test_fn, test_fn, test_fn);

TEST_CASE_MULTIPLE_STAGES("Check for leaks in MULTIPLE_STAGES mode (leak)", "[heap][ignore]", test_fn, test_fn, test_fn);

static void test_fn2(void)
{
    check_calloc(7000);
    esp_restart();
}

static void test_fn3(void)
{
    check_calloc(7000);
}

TEST_CASE_MULTIPLE_STAGES_ESP32("Check for leaks in MULTIPLE_STAGES mode (manual reset)", "[heap][leaks][reset=SW_CPU_RESET, SW_CPU_RESET]", test_fn2, test_fn2, test_fn3);
