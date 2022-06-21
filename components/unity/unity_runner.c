/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include "unity.h"
#include "esp_system.h"

/* similar to UNITY_PRINT_EOL */
#define UNITY_PRINT_TAB() UNITY_OUTPUT_CHAR('\t')

// Pointers to the head and tail of linked list of test description structs:
static test_desc_t *s_unity_tests_first = NULL;
static test_desc_t *s_unity_tests_last = NULL;

void unity_testcase_register(test_desc_t *desc)
{
    if (!s_unity_tests_first) {
        s_unity_tests_first = desc;
        s_unity_tests_last = desc;
    } else {
        test_desc_t *temp = s_unity_tests_first;
        s_unity_tests_first = desc;
        s_unity_tests_first->next = temp;
    }
}

/* print the multiple function case name and its sub-menu
 * e.g:
 * (1) spi master/slave case
 *       (1)master case
 *       (2)slave case
 * */
static void print_multiple_function_test_menu(const test_desc_t *test_ms)
{
    UnityPrint(test_ms->name);
    UNITY_PRINT_EOL();
    for (int i = 0; i < test_ms->test_fn_count; i++) {
        UNITY_PRINT_TAB();
        UnityPrint("(");
        UnityPrintNumberUnsigned(i + 1);
        UnityPrint(")");
        UNITY_PRINT_TAB();
        UnityPrint("\"");
        UnityPrint(test_ms->test_fn_name[i]);
        UnityPrint("\"");
        UNITY_PRINT_EOL();
    }
}

/*
 * This function looks like UnityDefaultTestRun function only without UNITY_CLR_DETAILS.
 * void UnityDefaultTestRun(UnityTestFunction Func, const char* FuncName, const int FuncLineNum)
 * was moved from `components/unity/unity/src/unity.c` to here.
*/
static void unity_default_test_run(UnityTestFunction Func, const char* FuncName, const int FuncLineNum)
{
    Unity.CurrentTestName = FuncName;
    Unity.CurrentTestLineNumber = (UNITY_LINE_TYPE)FuncLineNum;
    Unity.NumberOfTests++;
    if (TEST_PROTECT())
    {
        setUp();
        Func();
    }
    if (TEST_PROTECT())
    {
        tearDown();
    }
    UnityConcludeTest();
}

static int multiple_function_option(const test_desc_t *test_ms)
{
    int selection;
    char cmdline[256] = {0};

    print_multiple_function_test_menu(test_ms);
    while (strlen(cmdline) == 0) {
        unity_gets(cmdline, sizeof(cmdline));
        if (strlen(cmdline) == 0) {
            /* if input was newline, print a new menu */
            print_multiple_function_test_menu(test_ms);
        }
    }
    selection = atoi((const char *) cmdline) - 1;
    if (selection >= 0 && selection < test_ms->test_fn_count) {
        unity_default_test_run(test_ms->fn[selection], test_ms->name, test_ms->line);
    } else {
        UnityPrint("Invalid selection, your should input number 1-");
        UnityPrintNumber(test_ms->test_fn_count);
        UNITY_PRINT_EOL();
    }
    return selection;
}

static void unity_run_single_test(const test_desc_t *test)
{
    UnityPrint("Running ");
    UnityPrint(test->name);
    UnityPrint("...");
    UNITY_PRINT_EOL();
    // Unit test runner expects to see test name before the test starts
    UNITY_OUTPUT_FLUSH();

    Unity.TestFile = test->file;
    Unity.CurrentDetail1 = test->desc;
    bool reset_after_test = strstr(Unity.CurrentDetail1, "[leaks") != NULL;
    bool multi_device = strstr(Unity.CurrentDetail1, "[multi_device]") != NULL;
    if (test->test_fn_count == 1) {
        unity_default_test_run(test->fn[0], test->name, test->line);
    } else {
        int selection = multiple_function_option(test);
        if (reset_after_test && multi_device == false) {
            if (selection != (test->test_fn_count - 1)) {
                // to do a reset for all stages except the last stage.
                esp_restart();
            }
        }
    }

    if (reset_after_test) {
        // print a result of test before to do reset for the last stage.
        UNITY_END();
        UnityPrint("Enter next test, or 'enter' to see menu");
        UNITY_PRINT_EOL();
        UNITY_OUTPUT_FLUSH();
        esp_restart();
    }
}

void unity_run_test_by_index(int index)
{
    const test_desc_t *test;
    for (test = s_unity_tests_first; test != NULL && index != 0; test = test->next, --index) {
        ;
    }
    if (test != NULL) {
        unity_run_single_test(test);
    }
}

static void unity_run_single_test_by_index_parse(const char *filter, int index_max)
{
    int test_index = strtol(filter, NULL, 10);
    if (test_index >= 1 && test_index <= index_max) {
        UNITY_EXEC_TIME_START();
        unity_run_test_by_index(test_index - 1);
        UNITY_EXEC_TIME_STOP();
        UnityPrint("Test ran in ");
        UnityPrintNumberUnsigned(UNITY_EXEC_TIME_MS());
        UnityPrint("ms");
        UNITY_PRINT_EOL();
        UNITY_OUTPUT_FLUSH();
    }
}

void unity_run_test_by_name(const char *name)
{
    for (const test_desc_t *test = s_unity_tests_first; test != NULL; test = test->next) {
        if (strcmp(test->name, name) == 0) {
            unity_run_single_test(test);
        }
    }
}

void unity_run_all_tests(void)
{
    for (const test_desc_t *test = s_unity_tests_first; test != NULL; test = test->next) {
        unity_run_single_test(test);
    }
}

void unity_run_tests_by_tag(const char *tag, bool invert)
{
    UnityPrint("Running tests ");
    if (invert) {
        UnityPrint("NOT ");
    }
    UnityPrint("matching '");
    UnityPrint(tag);
    UnityPrint("'...");
    UNITY_PRINT_EOL();

    for (const test_desc_t *test = s_unity_tests_first; test != NULL; test = test->next) {
        if ((strstr(test->desc, tag) != NULL) == !invert) {
            unity_run_single_test(test);
        }
    }
}

static void trim_trailing_space(char *str)
{
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((int) *end)) {
        *end = 0;
        --end;
    }
}

static int print_test_menu(void)
{
    int test_counter = 0;
    UNITY_PRINT_EOL();
    UNITY_PRINT_EOL();
    UnityPrint("Here's the test menu, pick your combo:");
    UNITY_PRINT_EOL();
    for (const test_desc_t *test = s_unity_tests_first;
            test != NULL;
            test = test->next, ++test_counter) {

        UnityPrint("(");
        UnityPrintNumber(test_counter + 1);
        UnityPrint(")");
        UNITY_PRINT_TAB();
        UnityPrint("\"");
        UnityPrint(test->name);
        UnityPrint("\" ");
        UnityPrint(test->desc);
        UNITY_PRINT_EOL();

        if (test->test_fn_count > 1) {
            for (int i = 0; i < test->test_fn_count; i++) {
                UNITY_PRINT_TAB();
                UnityPrint("(");
                UnityPrintNumber(i + 1);
                UnityPrint(")");
                UNITY_PRINT_TAB();
                UnityPrint("\"");
                UnityPrint(test->test_fn_name[i]);
                UnityPrint("\"");
                UNITY_PRINT_EOL();
            }
        }
    }
    UNITY_PRINT_EOL();
    UnityPrint("Enter test for running."); /* unit_test.py needs it for finding the end of test menu */
    UNITY_PRINT_EOL();
    UNITY_OUTPUT_FLUSH();
    return test_counter;
}

int unity_get_test_count(void)
{
    int test_counter = 0;
    for (const test_desc_t *test = s_unity_tests_first;
            test != NULL;
            test = test->next) {
        ++test_counter;
    }
    return test_counter;
}

void unity_run_menu(void)
{
    UNITY_PRINT_EOL();
    UNITY_PRINT_EOL();
    UnityPrint("Press ENTER to see the list of tests.");
    UNITY_PRINT_EOL();
    int test_count = unity_get_test_count();
    while (true) {
        char cmdline[256] = { 0 };
        while (strlen(cmdline) == 0) {
            unity_gets(cmdline, sizeof(cmdline));
            trim_trailing_space(cmdline);
            if (strlen(cmdline) == 0) {
                /* if input was newline, print a new menu */
                print_test_menu();
            }
        }
        /*use '-' to show test history. Need to do it before UNITY_BEGIN cleanup history */
        if (cmdline[0] == '-') {
            UNITY_END();
            continue;
        }

        UNITY_BEGIN();

        size_t idx = 0;
        bool invert = false;
        if (cmdline[idx] == '!') {
            invert = true;
            ++idx;
        }

        if (cmdline[idx] == '*') {
            unity_run_all_tests();
        } else if (cmdline[idx] == '[') {
            unity_run_tests_by_tag(cmdline + idx, invert);
        } else if (cmdline[idx] == '"') {
            char* end = strrchr(cmdline, '"');
            if (end > &cmdline[idx]) {
                *end = 0;
                unity_run_test_by_name(cmdline + idx + 1);
            }
        } else if (isdigit((unsigned char)cmdline[idx])) {
            unity_run_single_test_by_index_parse(cmdline + idx, test_count);
        }

        UNITY_END();

        UnityPrint("Enter next test, or 'enter' to see menu");
        UNITY_PRINT_EOL();
        UNITY_OUTPUT_FLUSH();
    }
}

bool unity_get_test_info(int test_index, test_desc_t* out_info)
{
    if (test_index < 0) {
        return false;
    }
    const test_desc_t *test;
    for (test = s_unity_tests_first; test != NULL && test_index != 0; test = test->next, --test_index) {
        ;
    }
    if (test == NULL) {
        return false;
    }
    *out_info = *test;
    return true;
}
