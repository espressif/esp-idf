#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "esp_expression_with_stack.h"

void external_stack_function(void)
{
    printf("Executing this printf from external stack! sp=%p\n", get_sp());
}

void another_external_stack_function(void) 
{
    //We can even use Freertos resources inside of this context.
    printf("We can even use FreeRTOS resources delaying..., sp=%p\n", get_sp());
    vTaskDelay(100);
    printf("Done!, sp=%p\n", get_sp());
}

TEST_CASE("test printf using shared buffer stack", "[newlib]")
{
    portSTACK_TYPE *shared_stack = malloc(8192);

    TEST_ASSERT(shared_stack != NULL);

    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    TEST_ASSERT_NOT_NULL(printf_lock);
    printf("SP: %p\n", get_sp());
    printf("shared_stack: %p\n", (void *)shared_stack);

    esp_execute_shared_stack_function(printf_lock, shared_stack,8192,external_stack_function);
    esp_execute_shared_stack_function(printf_lock, shared_stack,8192,another_external_stack_function); 
    vSemaphoreDelete(printf_lock);   
    free(shared_stack);
}

