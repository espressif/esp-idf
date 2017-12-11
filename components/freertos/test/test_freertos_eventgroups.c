#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/timer.h"
#include "unity.h"

#define BIT_CALL (1 << 0)
#define BIT_RESPONSE(TASK) (1 << (TASK+1))
#define ALL_RESPONSE_BITS (((1 << NUM_TASKS) - 1) << 1)

static const int NUM_TASKS = 8;
static const int COUNT = 1000;
static EventGroupHandle_t eg;
static SemaphoreHandle_t done_sem;

static void task_event_group_call_response(void *param)
{
    int task_num = (int)param;

    printf("Started %d\n", task_num);

    for (int i = 0; i < COUNT; i++) {
        /* Wait until the common "call" bit is set, starts off all tasks
           (clear on return) */
        TEST_ASSERT( xEventGroupWaitBits(eg, BIT_CALL, true, false, portMAX_DELAY) );

        /* Set our individual "response" bit */
        xEventGroupSetBits(eg, BIT_RESPONSE(task_num));
    }

    printf("Task %d done\n", task_num);
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Event Groups", "[freertos]")
{
    eg = xEventGroupCreate();
    done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);

    /* Note: task_event_group_call_response all have higher priority than this task, so on this core
       they will always preempt this task.

       This is important because we need to know all tasks have blocked on BIT_CALL each time we signal it,
       or they get out of sync.
     */
    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_event_group_call_response, "tsk_call_resp", 4096, (void *)c, configMAX_PRIORITIES - 1, NULL, c % portNUM_PROCESSORS);
    }

    /* Tasks all start instantly, but this task will resume running at the same time as the higher priority tasks on the
       other processor may still be setting up, so allow time for them to also block on BIT_CALL... */
    vTaskDelay(10);

    for (int i = 0; i < COUNT; i++) {
        /* signal all tasks with "CALL" bit... */
        xEventGroupSetBits(eg, BIT_CALL);

        /* Only wait for 1 tick, the wakeup should be immediate... */
        TEST_ASSERT_EQUAL_HEX16(ALL_RESPONSE_BITS, xEventGroupWaitBits(eg, ALL_RESPONSE_BITS, true, true, 1));
    }

    /* Ensure all tasks cleaned up correctly */
    for (int c = 0; c < NUM_TASKS; c++) {
        TEST_ASSERT( xSemaphoreTake(done_sem, 100/portTICK_PERIOD_MS) );
    }

    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);
}

#define BIT_DONE(X) (1<<(NUM_TASKS+1+X))

static void task_test_sync(void *param)
{
    int task_num = (int)param;

    printf("Started %d\n", task_num);

    for (int i = 0; i < COUNT; i++) {
        /* set our bit, and wait on all tasks to set their bits */
        xEventGroupSync(eg, BIT_RESPONSE(task_num), ALL_RESPONSE_BITS, portMAX_DELAY);
        /* clear our bit */
        xEventGroupClearBits(eg, BIT_RESPONSE(task_num));
    }
    int after_done = xEventGroupSetBits(eg, BIT_DONE(task_num));

    printf("Done %d = 0x%08x\n", task_num, after_done);

    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

TEST_CASE("FreeRTOS Event Group Sync", "[freertos]")
{
    eg = xEventGroupCreate();
    done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);

    for (int c = 0; c < NUM_TASKS; c++) {
        xTaskCreatePinnedToCore(task_test_sync, "task_test_sync", 4096, (void *)c, configMAX_PRIORITIES - 1, NULL, c % portNUM_PROCESSORS);
    }

    for (int c = 0; c < NUM_TASKS; c++) {
        printf("Waiting on %d (0x%08x)\n", c, BIT_DONE(c));
        TEST_ASSERT( xEventGroupWaitBits(eg, BIT_DONE(c), false, false, portMAX_DELAY) );
    }

    /* Ensure all tasks cleaned up correctly */
    for (int c = 0; c < NUM_TASKS; c++) {
        TEST_ASSERT( xSemaphoreTake(done_sem, 100/portTICK_PERIOD_MS) );
    }

    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);
}

/*-----------------Test case for event group trace facilities-----------------*/
#ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
/*
 * Test event group Trace Facility functions such as
 * xEventGroupClearBitsFromISR(), xEventGroupSetBitsFromISR()
 */

//Use a timer to trigger an ISr
#define TIMER_DIVIDER   10000
#define TIMER_COUNT     1000
#define TIMER_NUMBER    0
#define SET_BITS    0xAA
#define CLEAR_BITS  0x55

static bool event_grp_cleared = false;

static void IRAM_ATTR event_group_isr()
{
    TIMERG0.int_clr_timers.t0 = 1;
    TIMERG0.hw_timer[xPortGetCoreID()].config.alarm_en = 1;
    if(!event_grp_cleared){
        xEventGroupClearBitsFromISR(eg, CLEAR_BITS);
        event_grp_cleared = true;
    }else{
        xEventGroupSetBitsFromISR(eg, SET_BITS, NULL);
        timer_pause(TIMER_GROUP_0, TIMER_NUMBER);
    }
}


static void test_event_group_trace_facility(void* arg)
{
    //Setup timer for ISR
    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_NUMBER;
    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;
    timer_init(timer_group, timer_idx, &config);    //Configure timer
    timer_pause(timer_group, timer_idx);    //Stop timer counter
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL); //Load counter value
    timer_set_alarm_value(timer_group, timer_idx, TIMER_COUNT); //Set alarm value
    timer_enable_intr(timer_group, timer_idx);  //Enable timer interrupt
    timer_set_auto_reload(timer_group, timer_idx, 1);   //Auto Reload
    timer_isr_register(timer_group, timer_idx, event_group_isr, NULL, ESP_INTR_FLAG_IRAM, NULL);    //Set ISR handler

    //Start timer to trigger isr
    timer_start(TIMER_GROUP_0, TIMER_NUMBER);
    TEST_ASSERT(xEventGroupWaitBits(eg, SET_BITS, pdFALSE, pdTRUE, portMAX_DELAY));
    //Check clear was successful
    TEST_ASSERT((xEventGroupGetBits(eg) & CLEAR_BITS) == 0);

    //Give semaphore to signal done
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);

}

TEST_CASE("FreeRTOS Event Group ISR", "[freertos]")
{

    done_sem = xSemaphoreCreateBinary();
    eg = xEventGroupCreate();
    xEventGroupSetBits(eg, CLEAR_BITS);     //Set bits to be cleared by ISR

    xTaskCreatePinnedToCore(test_event_group_trace_facility, "Testing Task", 4096, NULL, configMAX_PRIORITIES - 1, NULL, 0);

    //Wait until task and isr have finished testing
    xSemaphoreTake(done_sem, portMAX_DELAY);
    //Clean up
    vSemaphoreDelete(done_sem);
    vEventGroupDelete(eg);

    vTaskDelay(10);     //Give time for idle task to clear up delted tasks

}

#endif      //CONFIG_FREERTOS_USE_TRACE_FACILITY
