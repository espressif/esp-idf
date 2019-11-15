#include <stdio.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "soc/rtc.h"
#include "esp_system.h"
#include "test_utils.h"
#include "esp_expression_with_stack.h"
#include <stdio.h>

static portSTACK_TYPE shared_stack[8192];
static portSTACK_TYPE *ext_stack_top = (portSTACK_TYPE *)&shared_stack[0] + (sizeof(shared_stack) / sizeof(portSTACK_TYPE));

//makes sure this is not the task stack...
void check_stack(portSTACK_TYPE *sp) 
{
    StaticTask_t *hacked_task = (StaticTask_t *)xTaskGetCurrentTaskHandle();
    portSTACK_TYPE *task_sp = (portSTACK_TYPE *)hacked_task->pxDummy1;
    TEST_ASSERT((intptr_t)sp <= (intptr_t)ext_stack_top);
    TEST_ASSERT((intptr_t)sp >= (intptr_t)&shared_stack);

    TEST_ASSERT((intptr_t)task_sp < (intptr_t)&shared_stack ||
     (intptr_t)task_sp >= (intptr_t)&ext_stack_top);
}

TEST_CASE("test printf using shared buffer stack", "[newlib]")
{
    SemaphoreHandle_t printf_lock = xSemaphoreCreateMutex();
    EXECUTE_EXPRESSION_WITH_STACK(printf_lock, ext_stack_top, printf("Executing this from external stack! \n"));
    EXECUTE_EXPRESSION_WITH_STACK(printf_lock, ext_stack_top, check_stack(ext_stack_top));    
}
