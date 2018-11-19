// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "esp_clk.h"
#include "soc/cpu.h"
#include "unity.h"


// Pointers to the head and tail of linked list of test description structs:
static struct test_desc_t* s_unity_tests_first = NULL;
static struct test_desc_t* s_unity_tests_last = NULL;

// Inverse of the filter
static bool s_invert = false;

void unity_testcase_register(struct test_desc_t* desc)
{
    if (!s_unity_tests_first)
    {
        s_unity_tests_first = desc;
        s_unity_tests_last = desc;
    }
    else
    {
        struct test_desc_t* temp = s_unity_tests_first;
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
static void print_multiple_function_test_menu(const struct test_desc_t* test_ms)
 {
    printf("%s\n", test_ms->name);
    for (int i = 0; i < test_ms->test_fn_count; i++)
    {
        printf("\t(%d)\t\"%s\"\n", i+1, test_ms->test_fn_name[i]);
    }
 }

void multiple_function_option(const struct test_desc_t* test_ms)
{
    int selection;
    char cmdline[256] = {0};

    print_multiple_function_test_menu(test_ms);
    while(strlen(cmdline) == 0)
    {
        unity_gets(cmdline, sizeof(cmdline));
        if(strlen(cmdline) == 0) {
            /* if input was newline, print a new menu */
            print_multiple_function_test_menu(test_ms);
        }
    }
    selection = atoi((const char *) cmdline) - 1;
    if(selection >= 0 && selection < test_ms->test_fn_count) {
        UnityDefaultTestRun(test_ms->fn[selection], test_ms->name, test_ms->line);
    } else {
        printf("Invalid selection, your should input number 1-%d!", test_ms->test_fn_count);
    }
}

static void unity_run_single_test(const struct test_desc_t* test)
{
    printf("Running %s...\n", test->name);
    // Unit test runner expects to see test name before the test starts
    fflush(stdout);
    unity_flush();

    Unity.TestFile = test->file;
    Unity.CurrentDetail1 = test->desc;
    if(test->test_fn_count == 1) {
        UnityDefaultTestRun(test->fn[0], test->name, test->line);
    } else {
        multiple_function_option(test);
    }
}

static void unity_run_single_test_by_index(int index)
{
    const struct test_desc_t* test;
    for (test = s_unity_tests_first; test != NULL && index != 0; test = test->next, --index)
    {

    }
    if (test != NULL)
    {
        unity_run_single_test(test);
    }
}

static void unity_run_single_test_by_index_parse(const char* filter, int index_max)
{
    if (s_invert)
    {
        printf("Inverse is not supported for that kind of filter\n");
        return;
    }
    int test_index = strtol(filter, NULL, 10);
    if (test_index >= 1 && test_index <= index_max)
    {
        uint32_t start;
        RSR(CCOUNT, start);
        unity_run_single_test_by_index(test_index - 1);
        uint32_t end;
        RSR(CCOUNT, end);
        uint32_t ms = (end - start) / (esp_clk_cpu_freq() / 1000);
        printf("Test ran in %dms\n", ms);
    }
}

void unity_run_single_test_by_name(const char* filter)
{
    if (s_invert)
    {
        printf("Inverse is not supported for that kind of filter\n");
        return;
    }
    char tmp[256];
    strncpy(tmp, filter + 1, sizeof(tmp) - 1);
    tmp[strlen(filter) - 2] = 0;
    for (const struct test_desc_t* test = s_unity_tests_first; test != NULL; test = test->next)
    {
        if (strcmp(test->name, tmp) == 0)
        {
            unity_run_single_test(test);
        }
    }
}

void unity_run_all_tests()
{
    if (s_invert)
    {
        printf("Inverse is not supported for that kind of filter\n");
        return;
    }
    for (const struct test_desc_t* test = s_unity_tests_first; test != NULL; test = test->next)
    {
        unity_run_single_test(test);
    }
}

void unity_run_tests_with_filter(const char* filter)
{
    if (s_invert)
    {
        ++filter;
    }
    printf("Running tests %smatching '%s'...\n", s_invert ? "NOT " : "", filter);

    for (const struct test_desc_t* test = s_unity_tests_first; test != NULL; test = test->next)
    {
        if ((strstr(test->desc, filter) != NULL) == !s_invert)
        {
            unity_run_single_test(test);
        }
    }
}

static void trim_trailing_space(char* str)
{
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((int) *end))
    {
        *end = 0;
        --end;
    }
}

static int print_test_menu(void)
{
    int test_counter = 0;
    printf("\n\nHere's the test menu, pick your combo:\n");
    for (const struct test_desc_t* test = s_unity_tests_first;
         test != NULL;
         test = test->next, ++test_counter)
    {
        printf("(%d)\t\"%s\" %s\n", test_counter + 1, test->name, test->desc);
        if(test->test_fn_count > 1)
        {
            for (int i = 0; i < test->test_fn_count; i++)
            {
                printf("\t(%d)\t\"%s\"\n", i+1, test->test_fn_name[i]);
            }
         }
     }
     printf("\nEnter test for running.\n"); /* unit_test.py needs it for finding the end of test menu */
     return test_counter;
}

static int get_test_count(void)
{
    int test_counter = 0;
    for (const struct test_desc_t* test = s_unity_tests_first;
         test != NULL;
         test = test->next)
    {
        ++test_counter;
    }
    return test_counter;
}

void unity_run_menu()
{
    printf("\n\nPress ENTER to see the list of tests.\n");
    int test_count = get_test_count();
    while (true)
    {
        char cmdline[256] = { 0 };
        while(strlen(cmdline) == 0)
        {
            unity_gets(cmdline, sizeof(cmdline));
            trim_trailing_space(cmdline);
            if(strlen(cmdline) == 0) {
                /* if input was newline, print a new menu */
                print_test_menu();
            }
        }
        /*use '-' to show test history. Need to do it before UNITY_BEGIN cleanup history */
        if (cmdline[0] == '-')
        {
            UNITY_END();
            continue;
        }

        UNITY_BEGIN();

        size_t idx = 0;
        if (cmdline[idx] == '!')
        {
            s_invert = true;
            ++idx;
        }
        else
        {
            s_invert = false;
        }

        if (cmdline[idx] == '*')
        {
            unity_run_all_tests();
        }
        else if (cmdline[idx] =='[')
        {
            unity_run_tests_with_filter(cmdline + idx);
        }
        else if (cmdline[idx] =='"')
        {
            unity_run_single_test_by_name(cmdline + idx);
        }
        else if (isdigit((unsigned char)cmdline[idx]))
        {
            unity_run_single_test_by_index_parse(cmdline + idx, test_count);
        }

        UNITY_END();

        printf("Enter next test, or 'enter' to see menu\n");
    }
}
