#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "esp_expression_with_stack.h"

#define SHARED_STACK_SIZE 8192

static StackType_t *shared_stack_sp = NULL; 

void external_stack_function(void)
{
    printf("Executing this printf from external stack! sp=%p\n", get_sp());
    shared_stack_sp = (StackType_t *)get_sp();
}

void another_external_stack_function(void) 
{
    //We can even use Freertos resources inside of this context.
    printf("We can even use FreeRTOS resources delaying..., sp=%p\n", get_sp());
    vTaskDelay(100);
    printf("Done!, sp=%p\n", get_sp());
    shared_stack_sp = (StackType_t *)get_sp();
}

TEST_CASE("test printf using shared buffer stack", "[newlib]")
{
    portSTACK_TYPE *shared_stack = malloc(SHARED_STACK_SIZE);

    TEST_ASSERT(shared_stack != NULL);

    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    TEST_ASSERT_NOT_NULL(printf_lock);
    printf("SP: %p\n", get_sp());
    printf("shared_stack: %p\n", (void *)shared_stack);

    esp_execute_shared_stack_function(printf_lock, 
                                    shared_stack,
                                    SHARED_STACK_SIZE,
                                    external_stack_function);
    
    TEST_ASSERT(((shared_stack_sp >= shared_stack_sp) && 
                (shared_stack_sp < (shared_stack + SHARED_STACK_SIZE))));
    
    esp_execute_shared_stack_function(printf_lock, 
                                    shared_stack,
                                    SHARED_STACK_SIZE,
                                    another_external_stack_function); 
    
    TEST_ASSERT(((shared_stack_sp >= shared_stack_sp) && 
                (shared_stack_sp < (shared_stack + SHARED_STACK_SIZE))));

    vSemaphoreDelete(printf_lock);   
    free(shared_stack);
}

