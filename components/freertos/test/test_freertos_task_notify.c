/*
 Test of FreeRTOS task notifications. This test creates a sender and receiver
 task under different core permutations. For each permutation, the sender task
 will test the xTaskNotify(), xTaskNotifyGive(), xTaskNotifyFromISR(), and
 vTaskNotifyGiveFromISR(), whereas the receiver task will test
 xTaskNotifyWait() and ulTaskNotifyTake().
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/semphr.h>
#include "driver/timer.h"
#include "esp_ipc.h"
#include "unity.h"
#include "test_utils.h"

#define NO_OF_NOTIFS    4
#define NO_OF_TASKS     2       //Sender and receiver
#define TIMER_DIVIDER 10000
#define TIMER_COUNT 100
#define MESSAGE 0xFF

static uint32_t send_core_message = 0;
static TaskHandle_t rec_task_handle;
static bool isr_give = false;

static SemaphoreHandle_t trigger_send_semphr;
static SemaphoreHandle_t task_delete_semphr;

//Test tracking vars
static volatile uint32_t notifs_sent = 0;
static volatile uint32_t notifs_rec = 0;
static bool wrong_core = false;

static void sender_task (void* arg){
    int curcore = xPortGetCoreID();

    //Test xTaskNotify
    xSemaphoreTake(trigger_send_semphr, portMAX_DELAY);
    notifs_sent++;
    xTaskNotify(rec_task_handle, (MESSAGE << curcore), eSetValueWithOverwrite);

    //Test xTaskNotifyGive
    xSemaphoreTake(trigger_send_semphr, portMAX_DELAY);
    notifs_sent++;
    xTaskNotifyGive(rec_task_handle);

    //Test xTaskNotifyFromISR
    xSemaphoreTake(trigger_send_semphr, portMAX_DELAY);
    isr_give = false;
    timer_start(TIMER_GROUP_0, curcore);

    //Test vTaskNotifyGiveFromISR
    xSemaphoreTake(trigger_send_semphr, portMAX_DELAY);
    isr_give = true;
    timer_start(TIMER_GROUP_0, curcore);

    //Delete Task and Semaphores
    xSemaphoreGive(task_delete_semphr);
    vTaskDelete(NULL);
}

static void receiver_task (void* arg){
    uint32_t notify_value;

    //Test xTaskNotifyWait from task
    xTaskNotifyWait(0, 0xFFFFFFFF, &notify_value, portMAX_DELAY);
    if(notify_value != send_core_message){
        wrong_core = true;
    }
    notifs_rec++;

    //Test ulTaskNotifyTake from task
    xSemaphoreGive(trigger_send_semphr);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    notifs_rec++;

    //Test xTaskNotifyWait from ISR
    xSemaphoreGive(trigger_send_semphr);
    xTaskNotifyWait(0, 0xFFFFFFFF, &notify_value, portMAX_DELAY);
    if(notify_value != send_core_message){
        wrong_core = true;
    }
    notifs_rec++;

    //Test ulTaskNotifyTake from ISR
    xSemaphoreGive(trigger_send_semphr);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    notifs_rec++;

    //Test complete, stop timer and delete task
    xSemaphoreGive(task_delete_semphr);
    vTaskDelete(NULL);
}

static void IRAM_ATTR sender_ISR (void *arg)
{
    int curcore = xPortGetCoreID();
    timer_group_intr_clr_in_isr(TIMER_GROUP_0, curcore);
    timer_group_set_counter_enable_in_isr(TIMER_GROUP_0, curcore, TIMER_PAUSE);
    //Re-enable alarm
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, curcore);

    if(isr_give){   //Test vTaskNotifyGiveFromISR() on same core
        notifs_sent++;
        vTaskNotifyGiveFromISR(rec_task_handle, NULL);
    }
    else {  //Test xTaskNotifyFromISR()
        notifs_sent++;
        xTaskNotifyFromISR(rec_task_handle, (MESSAGE << curcore), eSetValueWithOverwrite, NULL);
    }
    portYIELD_FROM_ISR();
    return;
}

static void timerg0_init(void *isr_handle)
{
    int timer_group = TIMER_GROUP_0;
    int timer_idx = xPortGetCoreID();

    timer_config_t config;
    config.alarm_en = 1;
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_LEVEL;
    config.counter_en = TIMER_PAUSE;

    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx, TIMER_COUNT);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    //Auto Reload
    timer_set_auto_reload(timer_group, timer_idx, 1);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, sender_ISR, NULL, ESP_INTR_FLAG_IRAM, (intr_handle_t *)isr_handle);
}

static void timerg0_deinit(void* isr_handle)
{
    int timer_group = TIMER_GROUP_0;
    int timer_idx = xPortGetCoreID();
    intr_handle_t handle = *((intr_handle_t *) isr_handle);
    //Pause timer then free registered ISR
    timer_pause(timer_group, timer_idx);
    esp_intr_free(handle);
}

TEST_CASE("Test Task_Notify", "[freertos]")
{
    //Initialize and pause timers. Used to trigger ISR
    intr_handle_t isr_handle_0 = NULL;
    timerg0_init(&isr_handle_0);       //Core 0 timer
#ifndef CONFIG_FREERTOS_UNICORE
    intr_handle_t isr_handle_1 = NULL;
    esp_ipc_call(1, timerg0_init, &isr_handle_1);      //Core 1 timer
#endif

    trigger_send_semphr = xSemaphoreCreateBinary();
    task_delete_semphr = xQueueCreateCountingSemaphore(NO_OF_TASKS, 0);

    for(int i = 0; i < portNUM_PROCESSORS; i++){   //Sending Core
        for(int j = 0; j < portNUM_PROCESSORS; j++){   //Receiving Core
            //Reset Values
            notifs_sent = 0;
            notifs_rec = 0;
            wrong_core = false;

            send_core_message = (0xFF << i);        //0xFF if core 0, 0xFF0 if core 1

            xTaskCreatePinnedToCore(receiver_task, "rec task", 1000, NULL, UNITY_FREERTOS_PRIORITY + 2, &rec_task_handle, j);
            xTaskCreatePinnedToCore(sender_task, "send task", 1000, NULL, UNITY_FREERTOS_PRIORITY + 1, NULL, i);
            vTaskDelay(5);      //Wait for task creation to complete

            xSemaphoreGive(trigger_send_semphr);    //Trigger sender task
            for(int k = 0; k < NO_OF_TASKS; k++){             //Wait for sender and receiver task deletion
                xSemaphoreTake(task_delete_semphr, portMAX_DELAY);
            }
            vTaskDelay(5);      //Give time tasks to delete

            TEST_ASSERT(notifs_sent == NO_OF_NOTIFS);
            TEST_ASSERT(notifs_rec == NO_OF_NOTIFS);
            TEST_ASSERT(wrong_core == false);
        }
    }

    //Delete Semaphroes and timer ISRs
    vSemaphoreDelete(trigger_send_semphr);
    vSemaphoreDelete(task_delete_semphr);
    timerg0_deinit(&isr_handle_0);
    isr_handle_0 = NULL;
#ifndef CONFIG_FREERTOS_UNICORE
    esp_ipc_call(1, timerg0_deinit, &isr_handle_1);
    isr_handle_1 = NULL;
#endif
}


