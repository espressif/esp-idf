#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/timer.h"
#include "unity.h"

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define int_clr_timers int_clr
#define update update.update
#define int_st_timers int_st
#endif

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
#define TIMER_COUNT     100
#define TIMER_NUMBER    0
#define BITS            0xAA

static timer_isr_handle_t isr_handle;
static bool test_set_bits;
static bool test_clear_bits;

static void IRAM_ATTR event_group_isr(void *arg)
{
    portBASE_TYPE task_woken = pdFALSE;
    timer_group_intr_clr_in_isr(TIMER_GROUP_0, TIMER_0);
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, xPortGetCoreID());

    if(test_set_bits){
        xEventGroupSetBitsFromISR(eg, BITS, &task_woken);
        timer_pause(TIMER_GROUP_0, TIMER_NUMBER);
        test_set_bits = false;
    } else if (test_clear_bits){
        xEventGroupClearBitsFromISR(eg, BITS);
        xSemaphoreGiveFromISR(done_sem, &task_woken);
        timer_pause(TIMER_GROUP_0, TIMER_NUMBER);
        test_clear_bits = false;
    }
    //Switch context if necessary
    if(task_woken ==  pdTRUE){
        portYIELD_FROM_ISR();
    }
}

static void setup_timer(void)
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
    timer_isr_register(timer_group, timer_idx, event_group_isr, NULL, ESP_INTR_FLAG_IRAM, &isr_handle);    //Set ISR handler
}

static void cleanup_timer(void)
{
    timer_disable_intr(TIMER_GROUP_0, TIMER_NUMBER);
    esp_intr_free(isr_handle);
}

TEST_CASE("FreeRTOS Event Group ISR", "[freertos]")
{
    done_sem = xSemaphoreCreateBinary();
    eg = xEventGroupCreate();
    test_set_bits = false;
    test_clear_bits = false;
    setup_timer();                                   //Init timer to trigger ISR

    //Test set bits
    test_set_bits = true;
    timer_start(TIMER_GROUP_0, TIMER_NUMBER);
    TEST_ASSERT_EQUAL(BITS, xEventGroupWaitBits(eg, BITS, pdFALSE, pdTRUE, portMAX_DELAY));     //Let ISR set event group bits

    //Test clear bits
    xEventGroupSetBits(eg, BITS);                   //Set bits to be cleared
    test_clear_bits = true;
    timer_start(TIMER_GROUP_0, TIMER_NUMBER);
    xSemaphoreTake(done_sem, portMAX_DELAY);        //Wait for ISR to clear bits
    vTaskDelay(10);                                 //Event group clear bits runs via daemon task, delay so daemon can run
    TEST_ASSERT_EQUAL(0, xEventGroupGetBits(eg));   //Check bits are cleared

    //Clean up
    cleanup_timer();
    vEventGroupDelete(eg);
    vSemaphoreDelete(done_sem);
    vTaskDelay(10);     //Give time for idle task to clear up deleted tasks
}

#endif      //CONFIG_FREERTOS_USE_TRACE_FACILITY
