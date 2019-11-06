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

typedef struct {
    uint32_t noof_runs;
    SemaphoreHandle_t end_sema;
    uint32_t before_sched;
    uint32_t cycles_to_sched;
} test_context_t;

static void test_task_1(void *arg) {
    test_context_t *context = (test_context_t *)arg;

    for(context->noof_runs = 0 ;context->noof_runs < 10000; ) {
        context->before_sched = portGET_RUN_TIME_COUNTER_VALUE();
        vPortYield();
    }

    vTaskDelete(NULL); 
}

static void test_task_2(void *arg) {
    test_context_t *context = (test_context_t *)arg;

    for( ; context->noof_runs < 10000; context->noof_runs++) {
        context->cycles_to_sched += (portGET_RUN_TIME_COUNTER_VALUE() - context->before_sched);
        vPortYield();
    }

    context->cycles_to_sched /= 10000;
    xSemaphoreGive(context->end_sema);
    vTaskDelete(NULL);
}

TEST_CASE("scheduling time test", "[freertos]")
{
    test_context_t context; 

    context.end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(context.end_sema != NULL);

    xTaskCreatePinnedToCore(test_task_1, "test1" , 4096, &context, 1, NULL,1);
    xTaskCreatePinnedToCore(test_task_2, "test2" , 4096, &context, 1, NULL,1);

    BaseType_t result = xSemaphoreTake(context.end_sema, portMAX_DELAY);    
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);
    TEST_PERFORMANCE_LESS_THAN(SCHEDULING_TIME , "scheduling time %d cycles" ,context.cycles_to_sched);
}