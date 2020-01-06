#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "esp_expression_with_stack.h"

//makes sure this is not the task stack...
void another_external_stack_function(void) 
{
    //We can even use Freertos resources inside of this context.
    vTaskDelay(100);
    printf("Executing this another printf inside a function with external stack");
}

TEST_CASE("test printf using shared buffer stack", "[newlib]")
{
    portSTACK_TYPE *shared_stack = malloc(8192 * sizeof(portSTACK_TYPE));

    TEST_ASSERT(shared_stack != NULL);

    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    TEST_ASSERT_NOT_NULL(printf_lock);

    ESP_EXECUTE_EXPRESSION_WITH_STACK(printf_lock, shared_stack,8192,printf("Executing this printf from external stack! \n"));
    ESP_EXECUTE_EXPRESSION_WITH_STACK(printf_lock, shared_stack,8192,another_external_stack_function()); 
    vSemaphoreDelete(printf_lock);   
    free(shared_stack);
}

