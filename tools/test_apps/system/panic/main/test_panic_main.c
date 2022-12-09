#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include "esp_flash.h"
#include "esp_system.h"

/* utility functions */
static void die(const char* msg) __attribute__ ((noreturn));
static const char* get_test_name();

/* functions which cause an exception/panic in different ways */
static void test_abort();
static void test_int_wdt();
static void test_task_wdt();
static void test_storeprohibited();
static void test_cache_error();
static void test_int_wdt_cache_disabled();
static void test_stack_overflow();
static void test_illegal_instruction();
static void test_instr_fetch_prohibited();


void app_main(void)
{
    /* Needed to allow the tick hook to set correct INT WDT timeouts */
    vTaskDelay(2);

    /* Test script sends to command over UART. Read it and determine how to proceed. */
    const char* test_name = get_test_name();
    if (test_name == NULL) {
        /* Nothing to do */
        return;
    }
    printf("Got test name: %s\n", test_name);

    #define HANDLE_TEST(name_) \
        if (strcmp(test_name, #name_) == 0) { \
            name_(); \
            die("Test function has returned"); \
        }

    HANDLE_TEST(test_abort);
    HANDLE_TEST(test_int_wdt);
    HANDLE_TEST(test_task_wdt);
    HANDLE_TEST(test_storeprohibited);
    HANDLE_TEST(test_cache_error);
    HANDLE_TEST(test_int_wdt_cache_disabled);
    HANDLE_TEST(test_stack_overflow);
    HANDLE_TEST(test_illegal_instruction);
    HANDLE_TEST(test_instr_fetch_prohibited);

    #undef HANDLE_TEST

    die("Unknown test name");
}

/* implementations of the test functions */

static void test_abort()
{
    abort();
}

static void test_int_wdt()
{
    portDISABLE_INTERRUPTS();
    while (true) {
        ;
    }
}

static void test_task_wdt()
{
    while (true) {
        ;
    }
}

static void test_storeprohibited()
{
    *(int*) 0x1 = 0;
}

static IRAM_ATTR void test_cache_error()
{
    esp_flash_default_chip->os_func->start(esp_flash_default_chip->os_func_data);
    die("this should not be printed");
}

static void IRAM_ATTR test_int_wdt_cache_disabled()
{
    esp_flash_default_chip->os_func->start(esp_flash_default_chip->os_func_data);
    portDISABLE_INTERRUPTS();
    while (true) {
        ;
    }
}

static void test_stack_overflow()
{
    volatile uint8_t stuff[CONFIG_ESP_MAIN_TASK_STACK_SIZE * 2];
    for (int i = 0; i < sizeof(stuff); ++i) {
        stuff[i] = rand();
    }
}

static void test_illegal_instruction()
{
    __asm__ __volatile__("ill");
}

static void test_instr_fetch_prohibited()
{
    typedef void (*fptr_t)(void);
    volatile fptr_t fptr = (fptr_t) 0x4;
    fptr();
}

/* implementations of the utility functions */

#define BOOT_CMD_MAX_LEN (128)

static const char* get_test_name()
{
    static char test_name_str[BOOT_CMD_MAX_LEN] = {0};

    printf("Enter test name: ");
    fflush(stdout);

    /* Not using blocking fgets(stdin) here, as QEMU doesn't yet implement RX timeout interrupt,
     * which is required for the UART driver and blocking stdio to work.
     */
    int c = EOF;
    char *p = test_name_str;
    const char *end = test_name_str + sizeof(test_name_str) - 1;
    while (p < end) {
        c = getchar();
        if (c == EOF) {
            vTaskDelay(pdMS_TO_TICKS(10));
        } else if (c == '\r') {
            continue;
        } else if (c == '\n') {
            *p = '\0';
            break;
        } else {
            *p = c;
            ++p;
        }
    }

    return test_name_str;
}

extern void esp_restart_noos(void) __attribute__ ((noreturn));

static void die(const char* msg)
{
    printf("Test error: %s\n\n", msg);
    fflush(stdout);
    fsync(fileno(stdout));
    /* Don't use abort here as it would enter the panic handler */
    esp_restart_noos();
}
