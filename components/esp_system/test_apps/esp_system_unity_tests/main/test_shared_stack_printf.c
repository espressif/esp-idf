/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "esp_expression_with_stack.h"

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4) // TODO IDF-8832: RISC-V FPU does not work on shared stack,
// esp_execute_shared_stack_function might be deprecated because of
// the FPU issue and all the other issues with it.

#define SHARED_STACK_SIZE 8192

static StackType_t *shared_stack_sp = NULL;

void external_stack_function(void)
{
    printf("Executing this printf from external stack! sp=%p\n", esp_cpu_get_sp());

    shared_stack_sp = (StackType_t *)esp_cpu_get_sp();

    char *res = NULL;
    /* Test return value from asprintf, this could potentially help catch a misaligned
       stack pointer error */
    asprintf(&res, "%d %011i %lu %p %x %c %.4f\n", 42, 2147483647, 2147483648UL, (void *) 0x40010000, 0x40020000, 'Q', 1.0f / 137.0f);
    TEST_ASSERT_NOT_NULL(res);
    TEST_ASSERT_EQUAL_STRING("42 02147483647 2147483648 0x40010000 40020000 Q 0.0073\n", res);
    free(res);
}

void another_external_stack_function(void)
{
    //We can even use Freertos resources inside of this context.
    printf("We can even use FreeRTOS resources... yielding, sp=%p\n", esp_cpu_get_sp());
    taskYIELD();
    shared_stack_sp = (StackType_t *)esp_cpu_get_sp();
}

TEST_CASE("test printf using shared buffer stack", "[shared_stack]")
{
    StackType_t *shared_stack = malloc(SHARED_STACK_SIZE);

    TEST_ASSERT_NOT_NULL(shared_stack);

    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    TEST_ASSERT_NOT_NULL(printf_lock);
    printf("current task sp: %p\n", esp_cpu_get_sp());
    printf("shared_stack: %p\n", (void *)shared_stack);
    printf("shared_stack expected top: %p\n", (void *)(shared_stack + SHARED_STACK_SIZE));

    esp_execute_shared_stack_function(printf_lock,
                                      shared_stack,
                                      SHARED_STACK_SIZE,
                                      external_stack_function);

    TEST_ASSERT(((shared_stack_sp >= shared_stack) &&
                 (shared_stack_sp < (shared_stack + SHARED_STACK_SIZE))));

    esp_execute_shared_stack_function(printf_lock,
                                      shared_stack,
                                      SHARED_STACK_SIZE,
                                      another_external_stack_function);

    TEST_ASSERT(((shared_stack_sp >= shared_stack) &&
                 (shared_stack_sp < (shared_stack + SHARED_STACK_SIZE))));

    vSemaphoreDelete(printf_lock);
    free(shared_stack);
}
#endif // !TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4)
