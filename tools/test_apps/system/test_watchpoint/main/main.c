/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * This test checks that there are no orphan watchpoints are left on temporary stacks used by
 * esp_execute_shared_stack_function().
 */

#include <assert.h>
#include <stdio.h>
#include "esp_expression_with_stack.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TEST_TASK_STACK_SIZE 1024
#define SHARED_STACK_SIZE 8192

void external_stack_function(void)
{
    printf("Executing this printf from external stack! \n");
}

/**
 * The two stacks are statically allocated so that we can later examine them and trip any potential remaining
 * watchpoints.
 */
uint8_t static test_task_stack[TEST_TASK_STACK_SIZE];
uint8_t static shared_stack[SHARED_STACK_SIZE];

static void shared_stack_executer(void *arg)
{
    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    assert(printf_lock != NULL);

    //Call the desired function using the macro helper:
    esp_execute_shared_stack_function(printf_lock,
                                    shared_stack,
                                    sizeof(shared_stack),
                                    external_stack_function);

    vSemaphoreDelete(printf_lock);

    vTaskDelete(NULL);
}

void app_main(void)
{
    StaticTask_t task_buffer;
    TaskHandle_t task_handle = xTaskCreateStatic(shared_stack_executer,
                                    "test_shared_test_task",
                                    sizeof(test_task_stack),
                                    NULL,
                                    tskIDLE_PRIORITY + 1,
                                    test_task_stack,
                                    &task_buffer);

    assert(task_handle);

    vTaskDelay(pdMS_TO_TICKS(500));

    // If any watchpoints have been left on the temporary stacks, we will trigger them here:
    for (int i = 0; i < sizeof(test_task_stack); i++) {
        test_task_stack[i] = 0;
    }
    for (int i = 0; i < sizeof(shared_stack); i++) {
        shared_stack[i] = 0;
    }

    printf("stacks clean\n");
}
