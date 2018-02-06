#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "unity.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_clk.h"
#include "soc/cpu.h"
#include "esp_heap_caps.h"
#include "test_utils.h"

#ifdef CONFIG_HEAP_TRACING
#include "esp_heap_trace.h"
#endif

#define unity_printf ets_printf

// Pointers to the head and tail of linked list of test description structs:
static struct test_desc_t* s_unity_tests_first = NULL;
static struct test_desc_t* s_unity_tests_last = NULL;

// Inverse of the filter
static bool s_invert = false;


static size_t before_free_8bit;
static size_t before_free_32bit;

/* Each unit test is allowed to "leak" this many bytes.

   TODO: Make this value editable by the test.

   Will always need to be some value here, as fragmentation can reduce free space even when no leak is occuring.
*/
const size_t WARN_LEAK_THRESHOLD = 256;
const size_t CRITICAL_LEAK_THRESHOLD = 4096;

/* setUp runs before every test */
void setUp(void)
{
// If heap tracing is enabled in kconfig, leak trace the test
#ifdef CONFIG_HEAP_TRACING
    const size_t num_heap_records = 80;
    static heap_trace_record_t *record_buffer;
    if (!record_buffer) {
        record_buffer = malloc(sizeof(heap_trace_record_t) * num_heap_records);
        assert(record_buffer);
        heap_trace_init_standalone(record_buffer, num_heap_records);
    }
#endif

    printf("%s", ""); /* sneakily lazy-allocate the reent structure for this test task */
    get_test_data_partition();  /* allocate persistent partition table structures */

    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);

#ifdef CONFIG_HEAP_TRACING
    heap_trace_start(HEAP_TRACE_LEAKS);
#endif
}

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    if (before_free <= after_free) {
        return;
    }
    size_t leaked = before_free - after_free;
    if (leaked < WARN_LEAK_THRESHOLD) {
        return;
    }

    printf("MALLOC_CAP_%s %s leak: Before %u bytes free, After %u bytes free (delta %u)\n",
           type,
           leaked < CRITICAL_LEAK_THRESHOLD ? "potential" : "critical",
           before_free, after_free, leaked);
    fflush(stdout);
    TEST_ASSERT_MESSAGE(leaked < CRITICAL_LEAK_THRESHOLD, "The test leaked too much memory");
}

/* tearDown runs after every test */
void tearDown(void)
{
    /* some FreeRTOS stuff is cleaned up by idle task */
    vTaskDelay(5);

    /* We want the teardown to have this file in the printout if TEST_ASSERT fails */
    const char *real_testfile = Unity.TestFile;
    Unity.TestFile = __FILE__;

    /* check if unit test has caused heap corruption in any heap */
    TEST_ASSERT_MESSAGE( heap_caps_check_integrity(MALLOC_CAP_INVALID, true), "The test has corrupted the heap");

    /* check for leaks */
#ifdef CONFIG_HEAP_TRACING
    heap_trace_stop();
    heap_trace_dump();
#endif
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);

    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");

    Unity.TestFile = real_testfile; // go back to the real filename
}

void unity_putc(int c)
{
    if (c == '\n') 
    {
        uart_tx_one_char('\r');
        uart_tx_one_char('\n');
    }
    else if (c == '\r') 
    {
    }
    else 
    {
        uart_tx_one_char(c);
    }
}

void unity_flush()
{
    uart_tx_wait_idle(0);   // assume that output goes to UART0
}

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

/* print the multiple devices case name and its sub-menu
 * e.g:
 * (1) spi master/slave case
 *       (1)master case
 *       (2)slave case
 * */
static void print_multiple_devices_test_menu(const struct test_desc_t* test_ms)
 {
    unity_printf("%s\n", test_ms->name);
    for (int i = 0; i < test_ms->test_fn_count; i++)
    {
        unity_printf("\t(%d)\t\"%s\"\n", i+1, test_ms->test_fn_name[i]);
    }
 }

void multiple_devices_option(const struct test_desc_t* test_ms)
{
    int selection;
    char cmdline[256] = {0};

    print_multiple_devices_test_menu(test_ms);
    while(strlen(cmdline) == 0)
    {
        /* Flush anything already in the RX buffer */
        while(uart_rx_one_char((uint8_t *) cmdline) == OK) {

        }
        UartRxString((uint8_t*) cmdline, sizeof(cmdline) - 1);
        if(strlen(cmdline) == 0) {
            /* if input was newline, print a new menu */
            print_multiple_devices_test_menu(test_ms);
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
    Unity.TestFile = test->file;
    Unity.CurrentDetail1 = test->desc;
    if(test->test_fn_count == 1) {
        UnityDefaultTestRun(test->fn[0], test->name, test->line);
    } else {
        multiple_devices_option(test);
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

static void unity_run_single_test_by_name(const char* filter)
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
    unity_printf("\n\nHere's the test menu, pick your combo:\n");
    for (const struct test_desc_t* test = s_unity_tests_first;
         test != NULL;
         test = test->next, ++test_counter)
    {
        unity_printf("(%d)\t\"%s\" %s\n", test_counter + 1, test->name, test->desc);
        if(test->test_fn_count > 1)
        {
            for (int i = 0; i < test->test_fn_count; i++)
            {
                unity_printf("\t(%d)\t\"%s\"\n", i+1, test->test_fn_name[i]);
            }
         }
     }
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
    unity_printf("\n\nPress ENTER to see the list of tests.\n");
    int test_count = get_test_count();
    while (true)
    {
        char cmdline[256] = { 0 };
        while(strlen(cmdline) == 0)
        {
            /* Flush anything already in the RX buffer */
            while(uart_rx_one_char((uint8_t *) cmdline) == OK) {
            }
            /* Read input */
            UartRxString((uint8_t*) cmdline, sizeof(cmdline) - 1);
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
