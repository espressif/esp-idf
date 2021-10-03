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
static const char* get_test_name(void);

/* functions which cause an exception/panic in different ways */
static void test_abort(void);
static void test_abort_cache_disabled(void);
static void test_int_wdt(void);
static void test_task_wdt(void);
static void test_storeprohibited(void);
static void test_cache_error(void);
static void test_int_wdt_cache_disabled(void);
static void test_stack_overflow(void);
static void test_illegal_instruction(void);
static void test_instr_fetch_prohibited(void);
static void test_ub(void);
static void test_assert(void);
static void test_assert_cache_disabled(void);


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
    HANDLE_TEST(test_abort_cache_disabled);
    HANDLE_TEST(test_int_wdt);
    HANDLE_TEST(test_task_wdt);
    HANDLE_TEST(test_storeprohibited);
    HANDLE_TEST(test_cache_error);
    HANDLE_TEST(test_int_wdt_cache_disabled);
    HANDLE_TEST(test_stack_overflow);
    HANDLE_TEST(test_illegal_instruction);
    HANDLE_TEST(test_instr_fetch_prohibited);
    HANDLE_TEST(test_ub);
    HANDLE_TEST(test_assert);
    HANDLE_TEST(test_assert_cache_disabled);

    #undef HANDLE_TEST

    die("Unknown test name");
}

/* implementations of the test functions */

static void test_abort(void)
{
    abort();
}

static void IRAM_ATTR test_abort_cache_disabled(void)
{
    esp_flash_default_chip->os_func->start(esp_flash_default_chip->os_func_data);
    abort();
}

static void test_int_wdt(void)
{
    portDISABLE_INTERRUPTS();
    while (true) {
        ;
    }
}

static void test_task_wdt(void)
{
    while (true) {
        ;
    }
}

static void __attribute__((no_sanitize_undefined)) test_storeprohibited(void)
{
    *(int*) 0x1 = 0;
}

static IRAM_ATTR void test_cache_error(void)
{
    esp_flash_default_chip->os_func->start(esp_flash_default_chip->os_func_data);
    die("this should not be printed");
}

static void IRAM_ATTR test_int_wdt_cache_disabled(void)
{
    esp_flash_default_chip->os_func->start(esp_flash_default_chip->os_func_data);
    portDISABLE_INTERRUPTS();
    while (true) {
        ;
    }
}

static void test_assert(void)
{
    assert(0);
}

static void IRAM_ATTR test_assert_cache_disabled(void)
{
    esp_flash_default_chip->os_func->start(esp_flash_default_chip->os_func_data);
    assert(0);
}

/**
 * This function overwrites the stack beginning from the valid area continuously towards and beyond
 * the end of the stack (stack base) of the current task.
 * This is to test stack protection measures like a watchpoint at the end of the stack.
 *
 * @note: This test DOES NOT write beyond the stack limit. It only writes up to exactly the limit itself.
 *        The FreeRTOS stack protection mechanisms all trigger shortly before the end of the stack.
 */
static void test_stack_overflow(void)
{
    register uint32_t* sp asm("sp");
    TaskStatus_t pxTaskStatus;
    vTaskGetInfo(NULL, &pxTaskStatus, pdFALSE, pdFALSE);
    uint32_t *end = (uint32_t*) pxTaskStatus.pxStackBase;

    // offset - 20 bytes from SP in order to not corrupt the current frame.
    // Need to write from higher to lower addresses since the stack grows downwards and the watchpoint/canary is near
    // the end of the stack (lowest address).
    for (uint32_t* ptr = sp - 5; ptr != end; --ptr) {
        *ptr = 0;
    }

    // trigger a context switch to initiate checking the FreeRTOS stack canary
    vTaskDelay(pdMS_TO_TICKS(0));
}

static void test_illegal_instruction(void)
{
#if __XTENSA__
    __asm__ __volatile__("ill");
#elif __riscv
    __asm__ __volatile__("unimp");
#endif
}

static void test_instr_fetch_prohibited(void)
{
    typedef void (*fptr_t)(void);
    volatile fptr_t fptr = (fptr_t) 0x4;
    fptr();
}

static void test_ub(void)
{
    uint8_t stuff[1] = {rand()};
    printf("%d\n", stuff[rand()]);
}

/* implementations of the utility functions */

#define BOOT_CMD_MAX_LEN (128)

static const char* get_test_name(void)
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
    usleep(1000);
    /* Don't use abort here as it would enter the panic handler */
    esp_restart_noos();
}
