#include <esp_types.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "esp_intr_alloc.h"
#include "xtensa/hal.h"
#include "unity.h"
#include "soc/cpu.h"
#include "test_utils.h"

#define NUMBER_OF_ITERATIONS 10

typedef struct {
    SemaphoreHandle_t end_sema;
    uint32_t before_sched;
    uint32_t cycles_to_sched;
    TaskHandle_t t1_handle;
} test_context_t;

static void test_task_1(void *arg) {
    test_context_t *context = (test_context_t *)arg;

    for( ;; ) {
        context->before_sched = portGET_RUN_TIME_COUNTER_VALUE();
        vPortYield();
    }

    vTaskDelete(NULL); 
}

static void test_task_2(void *arg) {
    test_context_t *context = (test_context_t *)arg;
    uint64_t accumulator = 0;

    vTaskPrioritySet(NULL, CONFIG_UNITY_FREERTOS_PRIORITY + 1);
    vTaskPrioritySet(context->t1_handle, CONFIG_UNITY_FREERTOS_PRIORITY + 1);
    vPortYield();

    for(int i = 0; i < NUMBER_OF_ITERATIONS; i++) {
        accumulator += (portGET_RUN_TIME_COUNTER_VALUE() - context->before_sched);
        vPortYield();
    }

    context->cycles_to_sched = accumulator / NUMBER_OF_ITERATIONS;
    vTaskDelete(context->t1_handle);
    xSemaphoreGive(context->end_sema);
    vTaskDelete(NULL);
}

TEST_CASE("scheduling time test", "[freertos]")
{
    test_context_t context; 

    context.end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(context.end_sema != NULL);

#if !CONFIG_FREERTOS_UNICORE
    xTaskCreatePinnedToCore(test_task_1, "test1" , 4096, &context, 1, &context.t1_handle,1);
    xTaskCreatePinnedToCore(test_task_2, "test2" , 4096, &context, 1, NULL,1);
#else
    xTaskCreatePinnedToCore(test_task_1, "test1" , 4096, &context, CONFIG_UNITY_FREERTOS_PRIORITY - 1, &context.t1_handle,0);
    xTaskCreatePinnedToCore(test_task_2, "test2" , 4096, &context, CONFIG_UNITY_FREERTOS_PRIORITY - 1, NULL,0);
#endif

    BaseType_t result = xSemaphoreTake(context.end_sema, portMAX_DELAY);    
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);
    TEST_PERFORMANCE_LESS_THAN(SCHEDULING_TIME , "%d cycles" ,context.cycles_to_sched);
}
