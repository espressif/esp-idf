#include <esp_types.h>
#include <stdio.h>
#include "esp32/rom/ets_sys.h"

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

#ifdef CONFIG_FREERTOS_ISR_STATS

static SemaphoreHandle_t end_sema;
extern uint32_t isr_enter_spent_time_cycles;
volatile static uint32_t isr_enter_spent_time_cycles_copy;

static void testint(void *arg) {
    xthal_set_ccompare(1, xthal_get_ccount()+8000000);
}

static void nested3(void) {
    intr_handle_t handle;

    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, 0, &testint, NULL, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);
    xthal_set_ccompare(1, xthal_get_ccount()+8000000);
    vTaskDelay(10);

    isr_enter_spent_time_cycles_copy = isr_enter_spent_time_cycles;

    ets_printf("Average time spent on context save is: %d cycles\n\n", isr_enter_spent_time_cycles_copy);
    xSemaphoreGive(end_sema);
    vTaskDelete(NULL);
}

static void nested2(void) {
    nested3();
}

static void nested1(void) {
    nested2();
}

static void test_task(void *arg) {
    (void)arg;
    nested1();
 }

TEST_CASE("isr handling time test", "[freertos] [ignore]")
{
    end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(end_sema != NULL);
    xTaskCreatePinnedToCore(test_task, "tst" , 4096, NULL, 3, NULL, 0);
    BaseType_t result = xSemaphoreTake(end_sema, portMAX_DELAY);    
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);
    TEST_PERFORMANCE_LESS_THAN(SPILL_REG_CYCLES, "%d cycles" ,isr_enter_spent_time_cycles_copy); 
}

#endif