/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Tests of the U-mode domain API */
#include "esp_privilege_umode_domain.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include "esp_heap_caps.h"
#include "privilege_test_fixture.h"
#include "umode_elf.h"
#include "user/app_basic.h"

#define CODE_VA 0x20000000u
#define STACK_VA 0x20002000u
#define DATA_VA 0x10000000u

/* The U-mode program in user/app_basic.c, see umode_elf_add_program() in CMakeLists.txt */
extern const uint8_t app_basic_elf_start[] asm("_binary_app_basic_elf_start");
extern const uint8_t app_basic_elf_end[] asm("_binary_app_basic_elf_end");
#define APP_BASIC_SIZE ((size_t)(app_basic_elf_end - app_basic_elf_start))

/* A domain with an executable page at CODE_VA and a stack page at STACK_VA. The tests that use it do not run it,
 * so the code page stays zeroed. */
static esp_privilege_umode_domain_handle_t make_domain(void)
{
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ESP_OK(esp_privilege_umode_domain_create(&domain));
    TEST_ESP_OK(test_domain_map(domain, CODE_VA, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE, NULL, 0, 0, NULL));
    TEST_ESP_OK(test_domain_map(domain, STACK_VA, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_WRITE, NULL, 0, 0, NULL));
    return domain;
}

static esp_privilege_umode_domain_entry_t runtime(void)
{
    return (esp_privilege_umode_domain_entry_t) {
        .entry = CODE_VA, .stack = STACK_VA, .stack_size = 4096,
        .global_pointer = 123, .thread_pointer = 456,
    };
}

TEST_CASE("a domain checks finalization before running", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = make_domain();
    esp_privilege_umode_domain_result_t result;
    /* A domain runs only after finalize */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_privilege_umode_domain_run(domain, 0, 0, &result));
    /* The entry point must be executable, and the stack page is not */
    esp_privilege_umode_domain_entry_t rt = runtime();
    rt.entry = STACK_VA;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_finalize(domain, &rt));
    rt = runtime();
    TEST_ESP_OK(esp_privilege_umode_domain_finalize(domain, &rt));
    void *page = heap_caps_aligned_alloc(ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE, 4096, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(page);
    /* After finalize, the memory map and the start values are fixed */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_privilege_umode_domain_map(domain, DATA_VA, page, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_READ));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_privilege_umode_domain_finalize(domain, &rt));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_run(domain, 0, 0, NULL));
    TEST_ESP_OK(test_domain_destroy(domain));
    heap_caps_free(page);
}

TEST_CASE("the domain API refuses invalid pointers, handles and start values", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_result_t result;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_create(NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(NULL, DATA_VA, &result, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_READ));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_finalize(NULL, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_run(NULL, 0, 0, &result));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_destroy(NULL));
    esp_privilege_umode_domain_handle_t domain = make_domain();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, NULL, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_READ));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_finalize(domain, NULL));
    /* The entry point is 2-byte aligned, and the stack is a nonzero multiple of 16 bytes at a 16-byte boundary */
    const esp_privilege_umode_domain_entry_t bad[] = {
        { .entry = CODE_VA + 1, .stack = STACK_VA, .stack_size = 4096 },
        { .entry = CODE_VA, .stack = STACK_VA + 8, .stack_size = 4080 },
        { .entry = CODE_VA, .stack = STACK_VA, .stack_size = 4088 },
        { .entry = CODE_VA, .stack = STACK_VA, .stack_size = 0 },
        /* The whole stack must be mapped */
        { .entry = CODE_VA, .stack = STACK_VA, .stack_size = 8192 },
    };
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_finalize(domain, &bad[i]));
    }
    /* The smallest stack is 16 bytes */
    const esp_privilege_umode_domain_entry_t smallest = { .entry = CODE_VA, .stack = STACK_VA, .stack_size = 16 };
    TEST_ESP_OK(esp_privilege_umode_domain_finalize(domain, &smallest));
    TEST_ESP_OK(test_domain_destroy(domain));
}

TEST_CASE("map refuses memory, addresses and permissions that a domain cannot use", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ESP_OK(esp_privilege_umode_domain_create(&domain));
    uint8_t *page = heap_caps_aligned_alloc(ESP_PRIVILEGE_UMODE_DOMAIN_PAGE_SIZE, 12288, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(page);
    const uint32_t r = ESP_PRIVILEGE_UMODE_DOMAIN_READ, rw = ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_WRITE;
    /* Address, memory and size are multiples of the page size */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA + 1, page, 4096, r));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page + 4, 4096, r));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page, 0, r));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page, 100, r));
    /* A region is at most 4 MiB */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, esp_privilege_umode_domain_map(domain, DATA_VA, page, 4 * 1024 * 1024 + 4096, r));
    /* No permission, an unknown permission, write without read, and write with execute */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page, 4096, 0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page, 4096, 8));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_WRITE));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA, page, 4096,
                                                                          rw | ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE));
    /* The page at ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT stays unmapped, the one below it can be mapped */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT, page, 4096, r));
    TEST_ESP_OK(esp_privilege_umode_domain_map(domain, ESP_PRIVILEGE_UMODE_DOMAIN_ADDRESS_LIMIT - 4096, page + 8192, 4096, r));
    /* A region cannot overlap another region of the domain */
    TEST_ESP_OK(esp_privilege_umode_domain_map(domain, DATA_VA, page, 8192, rw));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_map(domain, DATA_VA + 4096, page, 4096, r));
    TEST_ESP_OK(esp_privilege_umode_domain_destroy(domain));
    heap_caps_free(page);
}

TEST_CASE("finalize rejects a stack that has writable memory right below it", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ESP_OK(esp_privilege_umode_domain_create(&domain));
    TEST_ESP_OK(test_domain_map(domain, CODE_VA, 4096, ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_EXECUTE, NULL, 0, 0, NULL));
    TEST_ESP_OK(test_domain_map(domain, STACK_VA - 4096, 8192, ESP_PRIVILEGE_UMODE_DOMAIN_READ | ESP_PRIVILEGE_UMODE_DOMAIN_WRITE, NULL, 0, 0,
                                NULL));
    /* A stack overflow would write into the page below the stack instead of raising an exception */
    esp_privilege_umode_domain_entry_t rt = runtime();
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_finalize(domain, &rt));
    rt.stack = STACK_VA + 2048;
    rt.stack_size = 2048;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_privilege_umode_domain_finalize(domain, &rt));
    /* The whole writable region as the stack, with the read-only code page below it */
    rt.stack = STACK_VA - 4096;
    rt.stack_size = 8192;
    TEST_ESP_OK(esp_privilege_umode_domain_finalize(domain, &rt));
    TEST_ESP_OK(test_domain_destroy(domain));
}

TEST_CASE("a domain is destroyed before and after finalize", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = make_domain();
    TEST_ESP_OK(test_domain_destroy(domain));
    domain = make_domain();
    const esp_privilege_umode_domain_entry_t rt = runtime();
    TEST_ESP_OK(esp_privilege_umode_domain_finalize(domain, &rt));
    TEST_ESP_OK(test_domain_destroy(domain));
}

TEST_CASE("M-mode illegal instruction still panics", "[privilege_panic][ignore]")
{
    __asm__ volatile(".word 0");
    TEST_FAIL_MESSAGE("Should have panicked");
}

TEST_CASE("M-mode fault with nonzero scratch retains the machine stack", "[privilege_panic][ignore]")
{
    /* mscratch holds a stale value, as it does while U-mode code runs. A trap from M-mode must
     * keep the M-mode stack. */
    __asm__ volatile("csrci mstatus, 8; li t0, 0x1234; csrw mscratch, t0; .word 0" ::: "t0", "memory");
}

/* Runs the program once and returns the value that it ended the run with */
static int32_t run_app(esp_privilege_umode_domain_handle_t domain, uint32_t selector, uint32_t arg)
{
    esp_privilege_umode_domain_result_t result;
    TEST_ESP_OK(esp_privilege_umode_domain_run(domain, selector, arg, &result));
    TEST_ASSERT_EQUAL(ESP_PRIVILEGE_UMODE_DOMAIN_EXIT_RETURN, result.reason);
    return result.value;
}

TEST_CASE("a loaded program runs and returns a value computed from its arguments", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ESP_OK(test_domain_load_app(app_basic_elf_start, APP_BASIC_SIZE, &domain));
    esp_privilege_umode_domain_result_t first, second;
    TEST_ESP_OK(esp_privilege_umode_domain_run(domain, APP_BASIC_RETURN, 5, &first));
    TEST_ASSERT_EQUAL(ESP_PRIVILEGE_UMODE_DOMAIN_EXIT_RETURN, first.reason);
    TEST_ASSERT_EQUAL(16, first.value);
    /* The domain runs again */
    TEST_ESP_OK(esp_privilege_umode_domain_run(domain, APP_BASIC_RETURN, 0x7fffffffu / 3, &second));
    TEST_ASSERT_EQUAL((int32_t)(0x7fffffffu / 3 * 3 + 1), second.value);
    /* An unknown selector is just a value that the program returns */
    TEST_ASSERT_EQUAL(APP_BASIC_ERROR_SELECTOR, run_app(domain, 99, 0));
    TEST_ESP_OK(test_domain_destroy(domain));
}

TEST_CASE("a loaded program ends the run from a nested call", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ESP_OK(test_domain_load_app(app_basic_elf_start, APP_BASIC_SIZE, &domain));
    TEST_ASSERT_EQUAL(42 + 7, run_app(domain, APP_BASIC_EXIT_NESTED, 42));
    /* The stack of the ended run does not affect the next one */
    TEST_ASSERT_EQUAL(4, run_app(domain, APP_BASIC_RETURN, 1));
    TEST_ESP_OK(test_domain_destroy(domain));
}

TEST_CASE("a loaded program has initialized data, zeroed bss and relocated pointers", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ESP_OK(test_domain_load_app(app_basic_elf_start, APP_BASIC_SIZE, &domain));
    TEST_ASSERT_EQUAL(0, run_app(domain, APP_BASIC_CHECK_DATA, 100));
    /* The data written in one run is there in the next: .data is 100 and .bss byte 10 is 110 */
    TEST_ASSERT_EQUAL(100 + 110, run_app(domain, APP_BASIC_READ_DATA, 10));
    TEST_ASSERT_EQUAL(APP_BASIC_ERROR_DATA, run_app(domain, APP_BASIC_CHECK_DATA, 0));
    /* Tables of pointers to functions and to read-only strings */
    TEST_ASSERT_EQUAL(13 * 100 + 3, run_app(domain, APP_BASIC_CALL_TABLE, 0));
    TEST_ASSERT_EQUAL(42 * 100 + 8, run_app(domain, APP_BASIC_CALL_TABLE, 1));
    TEST_ESP_OK(test_domain_destroy(domain));
}

TEST_CASE("programs loaded from one file at one address keep separate data", "[privilege][privilege_umode]")
{
    esp_privilege_umode_domain_handle_t first = NULL, second = NULL;
    TEST_ESP_OK(test_domain_load_app(app_basic_elf_start, APP_BASIC_SIZE, &first));
    TEST_ESP_OK(test_domain_load_app(app_basic_elf_start, APP_BASIC_SIZE, &second));
    TEST_ASSERT_EQUAL(1, run_app(first, APP_BASIC_COUNT, 0));
    TEST_ASSERT_EQUAL(2, run_app(first, APP_BASIC_COUNT, 0));
    TEST_ASSERT_EQUAL(1, run_app(second, APP_BASIC_COUNT, 0));
    TEST_ASSERT_EQUAL(0, run_app(first, APP_BASIC_CHECK_DATA, 50));
    TEST_ASSERT_EQUAL(0, run_app(second, APP_BASIC_CHECK_DATA, 70));
    TEST_ASSERT_EQUAL(50 + 50, run_app(first, APP_BASIC_READ_DATA, 0));
    TEST_ASSERT_EQUAL(70 + 70, run_app(second, APP_BASIC_READ_DATA, 0));
    TEST_ASSERT_EQUAL(3, run_app(first, APP_BASIC_COUNT, 0));
    TEST_ESP_OK(test_domain_destroy(first));
    TEST_ASSERT_EQUAL(2, run_app(second, APP_BASIC_COUNT, 0));
    TEST_ESP_OK(test_domain_destroy(second));
}

TEST_CASE("the loader refuses a truncated or damaged file", "[privilege][privilege_umode]")
{
    /* The unity leak check of the test case verifies that the failed loads free all memory */
    esp_privilege_umode_domain_handle_t domain = NULL;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, test_domain_load_app(app_basic_elf_start, 16, &domain));
    TEST_ASSERT_NULL(domain);
    /* Program headers cut off */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, test_domain_load_app(app_basic_elf_start, 64, &domain));
    /* Segments cut off */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, test_domain_load_app(app_basic_elf_start, APP_BASIC_SIZE / 2, &domain));
    uint8_t *copy = heap_caps_malloc(APP_BASIC_SIZE, MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(copy);
    memcpy(copy, app_basic_elf_start, APP_BASIC_SIZE);
    copy[1] = 'X';
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, test_domain_load_app(copy, APP_BASIC_SIZE, &domain));
    TEST_ASSERT_NULL(domain);
    /* The intact copy loads */
    copy[1] = 'E';
    TEST_ESP_OK(test_domain_load_app(copy, APP_BASIC_SIZE, &domain));
    heap_caps_free(copy);
    TEST_ASSERT_EQUAL(16, run_app(domain, APP_BASIC_RETURN, 5));
    TEST_ESP_OK(test_domain_destroy(domain));
    /* Invalid arguments */
    umode_elf_image_t image;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, umode_elf_load(NULL, app_basic_elf_start, APP_BASIC_SIZE, TEST_APP_BASE, &image));
    TEST_ESP_OK(esp_privilege_umode_domain_create(&domain));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, umode_elf_load(domain, app_basic_elf_start, APP_BASIC_SIZE, TEST_APP_BASE + 4, &image));
    TEST_ASSERT_EQUAL(0, image.count);
    TEST_ESP_OK(esp_privilege_umode_domain_destroy(domain));
}
