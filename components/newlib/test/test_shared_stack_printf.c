#include <stdio.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "test_utils.h"
#include "esp_expression_with_stack.h"

//makes sure this is not the task stack...
void check_stack(portSTACK_TYPE *sp, portSTACK_TYPE *base_sp) 
{
    StaticTask_t *hacked_task = (StaticTask_t *)xTaskGetCurrentTaskHandle();
    portSTACK_TYPE *task_sp = (portSTACK_TYPE *)hacked_task->pxDummy1;
    TEST_ASSERT((intptr_t)task_sp < (intptr_t)base_sp ||
     (intptr_t)task_sp >= (intptr_t)sp);
}

TEST_CASE("test printf using shared buffer stack", "[newlib]")
{
    portSTACK_TYPE *shared_stack = malloc(8192 * sizeof(portSTACK_TYPE));
    portSTACK_TYPE *ext_stack_top = (portSTACK_TYPE *)&shared_stack[0] + 
                                    ((sizeof(8192 * sizeof(portSTACK_TYPE))) / 
                                        sizeof(portSTACK_TYPE));

    TEST_ASSERT(shared_stack != NULL);

    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    EXECUTE_EXPRESSION_WITH_STACK(printf_lock, ext_stack_top, printf("Executing this from external stack! \n"));
    EXECUTE_EXPRESSION_WITH_STACK(printf_lock, ext_stack_top, check_stack(ext_stack_top, shared_stack));    
    free(shared_stack);
}
