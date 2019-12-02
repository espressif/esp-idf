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

#define SW_ISR_LEVEL_1  7

static SemaphoreHandle_t sync;
static SemaphoreHandle_t end_sema;
static uint32_t cycle_before_trigger;
static uint32_t cycle_before_exit;
static uint32_t delta_enter_cycles = 0;
static uint32_t delta_exit_cycles = 0;

static void software_isr(void *arg) {
    (void)arg;
    BaseType_t yield;
    delta_enter_cycles += portGET_RUN_TIME_COUNTER_VALUE() - cycle_before_trigger;
    
    xt_set_intclear(1 << SW_ISR_LEVEL_1);

    xSemaphoreGiveFromISR(sync, &yield);
    if(yield) {
        portYIELD_FROM_ISR();
    }

    cycle_before_exit = portGET_RUN_TIME_COUNTER_VALUE();
}

static void test_task(void *arg) {
    (void)arg;
    
    intr_handle_t handle;
 
    esp_err_t err = esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1, &software_isr, NULL, &handle);
    TEST_ASSERT_EQUAL_HEX32(ESP_OK, err);

    for(int i = 0;i < 10000; i++) {
        cycle_before_trigger = portGET_RUN_TIME_COUNTER_VALUE();
        xt_set_intset(1 << SW_ISR_LEVEL_1);
        xSemaphoreTake(sync, portMAX_DELAY);
        delta_exit_cycles += portGET_RUN_TIME_COUNTER_VALUE() - cycle_before_exit;
    }

    delta_enter_cycles /= 10000;
    delta_exit_cycles /= 10000;

    esp_intr_free(handle);
    xSemaphoreGive(end_sema);
    vTaskDelete(NULL);
}

TEST_CASE("isr latency test", "[freertos] [ignore]")
{
    sync = xSemaphoreCreateBinary();
    TEST_ASSERT(sync != NULL);
    end_sema = xSemaphoreCreateBinary();
    TEST_ASSERT(end_sema != NULL);
    xTaskCreatePinnedToCore(test_task, "tst" , 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);
    BaseType_t result = xSemaphoreTake(end_sema, portMAX_DELAY);    
    TEST_ASSERT_EQUAL_HEX32(pdTRUE, result);
    TEST_PERFORMANCE_LESS_THAN(ISR_ENTER_CYCLES, "%d cycles" ,delta_enter_cycles);
    TEST_PERFORMANCE_LESS_THAN(ISR_EXIT_CYCLES, "%d cycles" ,delta_exit_cycles);
}