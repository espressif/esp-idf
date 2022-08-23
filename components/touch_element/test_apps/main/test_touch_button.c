/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "esp_random.h"

#include "touch_element/touch_element_private.h"
#include "touch_element/touch_button.h"

static portMUX_TYPE test_button_spinlock = portMUX_INITIALIZER_UNLOCKED;
#define TEST_BUTTON_ENTER_CRITICAL()       portENTER_CRITICAL(&test_button_spinlock)
#define TEST_BUTTON_EXIT_CRITICAL()        portEXIT_CRITICAL(&test_button_spinlock)

static const touch_pad_t button_channel_array[14] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM6,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM8,
    TOUCH_PAD_NUM9,
    TOUCH_PAD_NUM10,
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM13,
    TOUCH_PAD_NUM14,
};
const uint8_t BUTTON_CHANNEL_NUM = sizeof(button_channel_array) / sizeof(touch_pad_t);

typedef struct {
    QueueHandle_t valid_msg_handle;
    SemaphoreHandle_t response_sig_handle;
} test_monitor_t;

typedef struct {
    QueueHandle_t valid_msg_handle;
    SemaphoreHandle_t response_sig_handle;
    touch_button_handle_t button_handle;
} test_concurrent_monitor_t;

/* ------------------------------------------------------------------------------------------------------------------ */
void test_button_event_simulator(touch_button_handle_t button_handle, touch_button_event_t button_event);
void test_button_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message);
static void test_button_callback_check(touch_button_handle_t current_handle, touch_button_message_t *current_message, touch_elem_message_t *valid_message);
void test_button_event_trigger_and_check(touch_button_handle_t handle, touch_button_event_t button_event);
void test_button_callback_trigger_and_check(touch_button_handle_t handle, touch_button_event_t button_event, bool should_trigger, test_monitor_t *monitor);
/* ------------------------------------------------ Dispatch method test -------------------------------------------- */
static void test_button_disp_event(void);
static void test_button_disp_callback(void);
void test_button_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg);
/* ------------------------------------------------ Run-time test --------------------------------------------------- */
static void test_button_event_change_lp(void);
static void test_button_callback_change_lp(void);
static void test_button_change_lp_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg);
/* ------------------------------------------------ Concurrent test ------------------------------------------------- */
static void test_button_event_concurrent(void);
static void test_button_random_trigger_concurrent(void);
void test_random_trigger_concurrent_task(void *arg);
static void random_trigger_concurrent_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg);
/* ------------------------------------------------------------------------------------------------------------------ */

TEST_CASE("Touch button dispatch methods test", "[button][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_button_disp_event();
    test_button_disp_callback();
    touch_element_uninstall();
}

TEST_CASE("Touch button run-time test", "[button][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_button_event_change_lp();
    test_button_callback_change_lp();
    touch_element_uninstall();
}

TEST_CASE("Touch button concurrent test", "[button][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_button_event_concurrent();
    test_button_random_trigger_concurrent();
    touch_element_uninstall();
}

void test_button_event_simulator(touch_button_handle_t button_handle, touch_button_event_t button_event)
{
    te_button_handle_t te_button = (te_button_handle_t) button_handle;
    touch_pad_t channel = te_button->device->channel;
    if (button_event == TOUCH_BUTTON_EVT_ON_PRESS) {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else if (button_event == TOUCH_BUTTON_EVT_ON_RELEASE) {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);  //LongPress
    }
}

void test_button_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message)
{
    TEST_ASSERT_MESSAGE(current_message->handle == valid_message->handle, "check handle failed");
    TEST_ASSERT_MESSAGE(current_message->element_type == valid_message->element_type, "check element type failed");
    const touch_button_message_t *valid_button_message = touch_button_get_message(valid_message);
    const touch_button_message_t *current_button_message = touch_button_get_message(current_message);
    TEST_ASSERT_MESSAGE(current_button_message->event == valid_button_message->event, "check event failed");
}

static void test_button_callback_check(touch_button_handle_t current_handle, touch_button_message_t *current_message, touch_elem_message_t *valid_message)
{
    const touch_button_message_t *valid_button_message = touch_button_get_message(valid_message);
    TEST_ASSERT_MESSAGE(valid_message->handle == current_handle, "check handle failed");
    TEST_ASSERT_MESSAGE(valid_message->element_type == TOUCH_ELEM_TYPE_BUTTON, "check element type failed");
    TEST_ASSERT_MESSAGE(valid_button_message->event == current_message->event, "check event failed");
}

void test_button_event_trigger_and_check(touch_button_handle_t handle, touch_button_event_t button_event)
{//TODO: refactor this with a constructor
    touch_elem_message_t valid_message = {
        .handle = handle,
        .element_type = TOUCH_ELEM_TYPE_BUTTON,
        .arg = NULL,
    };
    touch_button_message_t button_message = {
        .event = button_event
    };
    memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message

    test_button_event_simulator(handle, button_event);  //Trigger signal

    touch_elem_message_t current_message;
    te_button_handle_t te_button = handle;
    esp_err_t ret = touch_element_message_receive(&current_message, pdMS_TO_TICKS(2 * te_button->trigger_thr * 10));
    TEST_ASSERT_MESSAGE(ret == ESP_OK, "button event receive timeout");

    test_button_event_check(&valid_message, &current_message);  //Verification
}

void test_button_callback_trigger_and_check(touch_button_handle_t handle, touch_button_event_t button_event, bool should_trigger, test_monitor_t *monitor)
{
    if (should_trigger) {
        touch_elem_message_t valid_message = {
            .handle = handle,
            .element_type = TOUCH_ELEM_TYPE_BUTTON,
            .arg = NULL
        };
        touch_button_message_t button_message = {
            .event = button_event
        };
        memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message
        xQueueSend(monitor->valid_msg_handle, &valid_message, portMAX_DELAY);
    }

    test_button_event_simulator(handle, button_event);  //Trigger signal

    te_button_handle_t te_button = handle;
    if (should_trigger) { //Verification
        BaseType_t os_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(2 * te_button->trigger_thr * 10));
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "Button queue timeout");
    } else {
        BaseType_t os_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(500));
        TEST_ASSERT_MESSAGE(os_ret == pdFALSE, "Button invalid trigger");
    }
}

static void test_button_disp_event(void)
{
    touch_button_handle_t button_handle[BUTTON_CHANNEL_NUM];
    touch_button_global_config_t global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&global_config));
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = 0.1F
        };
        TEST_ESP_OK(touch_button_create(&button_config, &button_handle[i]));
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i],
                    TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
                    (void *) button_channel_array[i]));
        TEST_ESP_OK(touch_button_set_longpress(button_handle[i], 300));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
    }
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    //10 times random press/longpress/release test
    printf("Touch button event test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch button event test... (%d/10)\n", i + 1);
        touch_button_handle_t current_handle = button_handle[random() % 14];
        test_button_event_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_PRESS);
        test_button_event_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_LONGPRESS);
        test_button_event_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_RELEASE);
    }
    printf("Touch button event test finish\n");
    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    touch_button_uninstall();
}

static void test_button_disp_callback(void)
{
    test_monitor_t monitor;
    touch_button_handle_t button_handle[BUTTON_CHANNEL_NUM];
    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    TEST_ASSERT(monitor.valid_msg_handle != NULL || monitor.response_sig_handle != NULL);

    touch_button_global_config_t button_init = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&button_init));
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = 0.1F
        };
        TEST_ESP_OK(touch_button_create(&button_config, &button_handle[i]));
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i],
                    TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE | TOUCH_ELEM_EVENT_ON_LONGPRESS,
                    (void *) &monitor));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_CALLBACK));
        TEST_ESP_OK(touch_button_set_callback(button_handle[i], &test_button_handler));
        TEST_ESP_OK(touch_button_set_longpress(button_handle[i], 300));
    }
    TEST_ESP_OK(touch_element_start());

    srandom((unsigned int)time(NULL));
    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1
    //10 times random press/longpress/release test
    printf("Touch button callback test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch button callback test... (%d/10)\n", i + 1);
        touch_button_handle_t current_handle = button_handle[random() % 14];
        test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_PRESS, true, &monitor);
        test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_LONGPRESS, true, &monitor);
        test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_RELEASE, true, &monitor);
    }
    printf("Touch button callback test finish\n");
    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    touch_button_uninstall();
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
}

void test_button_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg)
{
    test_monitor_t *monitor = (test_monitor_t *)arg;
    touch_elem_message_t valid_message;
    BaseType_t os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(200));  //Get the valid message for the verification, 500ms timeout
    TEST_ASSERT_MESSAGE(os_ret == pdPASS, "test_button_handler: queue timeout");
    test_button_callback_check(handle, message, &valid_message);
    xSemaphoreGive(monitor->response_sig_handle);
}

static void test_button_event_change_lp(void)
{
    touch_button_handle_t button_handle[BUTTON_CHANNEL_NUM];
    touch_button_global_config_t global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&global_config));
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = 0.1F
        };
        TEST_ESP_OK(touch_button_create(&button_config, &button_handle[i]));
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_LONGPRESS, NULL));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
    }
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    //10 times random press/longpress/release test
    printf("Touch button event change longtime test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch button event change longtime test... (%d/10)\n", i + 1);
        esp_err_t ret;
        uint8_t channel_index = random() % BUTTON_CHANNEL_NUM;
        touch_elem_message_t valid_message = {
            .handle = button_handle[channel_index],
            .element_type = TOUCH_ELEM_TYPE_BUTTON,
            .arg = NULL
        };
        touch_button_message_t button_message = {
            .event = TOUCH_BUTTON_EVT_ON_LONGPRESS
        };
        memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message

        TEST_ESP_OK(touch_button_set_longpress(valid_message.handle, 200 + (i + 1) * 50));
        test_button_event_simulator(valid_message.handle, button_message.event);  //Trigger signal

        touch_elem_message_t current_message;
        ret = touch_element_message_receive(&current_message, pdMS_TO_TICKS(10 * 1000));
        TEST_ASSERT_MESSAGE(ret == ESP_OK, "button event LongPress timeout");
        test_button_event_check(&valid_message, &current_message);  //Verification

        test_button_event_simulator(valid_message.handle, TOUCH_BUTTON_EVT_ON_RELEASE); //Release the button.
    }
    printf("Touch button event change longtime test finish\n");
    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    touch_button_uninstall();
}

static void test_button_callback_change_lp(void)
{
    test_monitor_t monitor;
    touch_button_handle_t button_handle[BUTTON_CHANNEL_NUM];
    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    TEST_ASSERT(monitor.valid_msg_handle != NULL || monitor.response_sig_handle != NULL);

    touch_button_global_config_t global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&global_config));
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = 0.1F
        };
        TEST_ESP_OK(touch_button_create(&button_config, &button_handle[i]));
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_LONGPRESS, (void *)&monitor));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_CALLBACK));
        TEST_ESP_OK(touch_button_set_callback(button_handle[i], &test_button_change_lp_handler));
    }
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    //10 times random press/longpress/release test
    printf("Touch button event change longtime test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch button event change longtime test... (%d/10)\n", i + 1);
        uint8_t channel_index = 5;  //Always this channel
        touch_elem_message_t valid_message = {
            .handle = button_handle[channel_index],
            .element_type = TOUCH_ELEM_TYPE_BUTTON,
            .arg = NULL,
        };
        touch_button_message_t button_message = {
            .event = TOUCH_BUTTON_EVT_ON_LONGPRESS
        };
        memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message

        xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
        test_button_event_simulator(button_handle[channel_index], button_message.event);

        BaseType_t os_ret = xSemaphoreTake(monitor.response_sig_handle, pdMS_TO_TICKS(10 * 1000)); //100ms timeout
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "Button LongPress queue timeout");
        test_button_event_simulator(valid_message.handle, TOUCH_BUTTON_EVT_ON_RELEASE); //Reset hardware
    }
    printf("Touch button event change longtime test finish\n");
    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    touch_button_uninstall();
}

static void test_button_change_lp_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg)
{
    test_monitor_t *monitor = (test_monitor_t *)arg;
    touch_elem_message_t valid_message;
    BaseType_t os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(200));  //Get the valid message for the verification, 500ms timeout
    TEST_ASSERT_MESSAGE(os_ret == pdPASS, "test_button_handler: queue timeout");
    test_button_callback_check(handle, message, &valid_message);
    xSemaphoreGive(monitor->response_sig_handle);
    TEST_ESP_OK(touch_button_set_longpress(valid_message.handle, 300)); // Always 300ms
}

static void test_button_event_concurrent(void)
{
    touch_button_handle_t button_handle[BUTTON_CHANNEL_NUM];
    touch_button_global_config_t global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&global_config));
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = 0.1F
        };
        TEST_ESP_OK(touch_button_create(&button_config, &button_handle[i]));
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
    }
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    //10 times random press/longpress/release test
    printf("Touch button event concurrent test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch button event concurrent test... (%d/10)\n", i + 1);
        esp_err_t ret;
        uint32_t message_count = 0;
        touch_elem_message_t current_message;

        TEST_BUTTON_ENTER_CRITICAL();
        for (int idx = 0; idx < BUTTON_CHANNEL_NUM; idx++) {
            test_button_event_simulator(button_handle[idx], TOUCH_BUTTON_EVT_ON_PRESS);  //All channels trigger
        }
        TEST_BUTTON_EXIT_CRITICAL();
        message_count = 0;
        do {
            ret = touch_element_message_receive(&current_message, pdMS_TO_TICKS(500));
            if (ret == ESP_OK) {
                message_count++;
            }
        } while (ret == ESP_OK);
        TEST_ASSERT_MESSAGE(message_count == BUTTON_CHANNEL_NUM, "button concurrent Press failed");

        TEST_BUTTON_ENTER_CRITICAL();
        for (int idx = 0; idx < BUTTON_CHANNEL_NUM; idx++) {
            test_button_event_simulator(button_handle[idx], TOUCH_BUTTON_EVT_ON_RELEASE);  //All channels trigger
        }
        TEST_BUTTON_EXIT_CRITICAL();
        message_count = 0;
        do {
            ret = touch_element_message_receive(&current_message, pdMS_TO_TICKS(500));
            if (ret == ESP_OK) {
                message_count++;
            }
        } while (ret == ESP_OK);
        TEST_ASSERT_MESSAGE(message_count == BUTTON_CHANNEL_NUM, "button concurrent Release failed");
    }
    printf("Touch button event concurrent test finish\n");
    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    touch_button_uninstall();
}

static void test_button_random_trigger_concurrent(void)
{
    uint64_t sem_and_monitor[BUTTON_CHANNEL_NUM];
    printf("Touch button random trigger concurrent test start\n");
    test_concurrent_monitor_t monitor[BUTTON_CHANNEL_NUM];

    SemaphoreHandle_t count_sem = xSemaphoreCreateCounting(BUTTON_CHANNEL_NUM, 0);

    touch_button_global_config_t global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&global_config));
    for (uint32_t i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = 0.1F
        };
        monitor[i].response_sig_handle = xSemaphoreCreateBinary();
        monitor[i].valid_msg_handle = xQueueCreate(BUTTON_CHANNEL_NUM, sizeof(touch_elem_message_t));
        TEST_ASSERT(monitor[i].valid_msg_handle != NULL && monitor[i].response_sig_handle != NULL);
        uintptr_t temp_count_sem = (uint32_t)count_sem;
        uintptr_t temp_monitor = (uint32_t)&monitor[i];  //Prevent compiler warning
        sem_and_monitor[i] = (uint64_t)(((uint64_t)temp_count_sem << 32) | (uint64_t) temp_monitor);
        TEST_ESP_OK(touch_button_create(&button_config, &monitor[i].button_handle));
        TEST_ESP_OK(touch_button_subscribe_event(monitor[i].button_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_LONGPRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *)&sem_and_monitor[i]));
        TEST_ESP_OK(touch_button_set_longpress(monitor[i].button_handle, 500));
        TEST_ESP_OK(touch_button_set_dispatch_method(monitor[i].button_handle, TOUCH_ELEM_DISP_CALLBACK));
        TEST_ESP_OK(touch_button_set_callback(monitor[i].button_handle, &random_trigger_concurrent_handler));
    }
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    for (uint32_t i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        BaseType_t os_ret = xTaskCreate(test_random_trigger_concurrent_task, "test_random_trigger_concurrent_task", 1024 * 4, (void *)&sem_and_monitor[i], 10, NULL);
        TEST_ASSERT(os_ret == pdPASS);
    }


    uint32_t run_count = 0;
    while (1) {
        if (run_count++ % 1000 == 0) {
            printf("Touch button random trigger concurrent test running... (1/1)\n");
        }
        uint8_t count = uxSemaphoreGetCount(count_sem);
        if (count == BUTTON_CHANNEL_NUM) {
            vTaskDelay(1); //Let IDLE task running and get tasks cleanup
            break;
        }
        vTaskDelay(1);
    }

    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        vQueueDelete(monitor[i].valid_msg_handle);
        vSemaphoreDelete(monitor[i].response_sig_handle);
        TEST_ESP_OK(touch_button_delete(monitor[i].button_handle));
    }
    touch_button_uninstall();
    printf("Touch button random trigger concurrent test stop\n");
}

void test_random_trigger_concurrent_task(void *arg)
{
    uintptr_t temp_monitor = *((uint32_t *) arg);
    uintptr_t temp_count_sem = (*((uint64_t *) arg) >> 32);  //Prevent compiler warning
    test_concurrent_monitor_t *monitor = (test_concurrent_monitor_t *)temp_monitor;
    SemaphoreHandle_t count_sem = (SemaphoreHandle_t) temp_count_sem;
    uint32_t start_delay_time = (esp_random() % 100) * 10;
    vTaskDelay(pdMS_TO_TICKS(start_delay_time));

    touch_elem_message_t valid_message = {
        .handle = monitor->button_handle,
        .element_type = TOUCH_ELEM_TYPE_BUTTON,
        .arg = NULL,
    };
    touch_button_message_t button_message;
    button_message.event = TOUCH_BUTTON_EVT_ON_PRESS;
    memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message
    xQueueSend(monitor->valid_msg_handle, &valid_message, portMAX_DELAY);
    test_button_event_simulator(valid_message.handle, button_message.event);  //Trigger signal
    BaseType_t res_sem_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(1000));
    TEST_ASSERT_MESSAGE(res_sem_ret == pdPASS, "Response timeout");

    uint32_t hold_state_time_ms = (esp_random() % 100) * 10 + 100;
    te_button_handle_t te_button = (te_button_handle_t) valid_message.handle;
    if ((int)(hold_state_time_ms - te_button->trigger_thr * 10) > 50) {  //should raise longpress event
        button_message.event = TOUCH_BUTTON_EVT_ON_LONGPRESS;
        memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message
        xQueueSend(monitor->valid_msg_handle, &valid_message, portMAX_DELAY);
        test_button_event_simulator(valid_message.handle, button_message.event);  //Trigger signal
        res_sem_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(1000)); //+100 make sure it will really raise longpress event
        TEST_ASSERT_MESSAGE(res_sem_ret == pdPASS, "Response timeout");
    } else { //should not raise longpress event
        //Do nothing
    }

    button_message.event = TOUCH_BUTTON_EVT_ON_RELEASE;
    memcpy(valid_message.child_msg, &button_message, sizeof(touch_button_message_t));  //Construct valid_message
    xQueueSend(monitor->valid_msg_handle, &valid_message, portMAX_DELAY);
    test_button_event_simulator(valid_message.handle, button_message.event);  //Trigger signal
    res_sem_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(1000));
    TEST_ASSERT_MESSAGE(res_sem_ret == pdPASS, "Response timeout");

    xSemaphoreGive(count_sem);
    vTaskDelete(NULL);
}

static void random_trigger_concurrent_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg)
{
    uintptr_t temp_monitor = *((uint32_t *) arg);  //Prevent compiler warning
    test_concurrent_monitor_t *monitor = (test_concurrent_monitor_t *) temp_monitor;
    touch_elem_message_t valid_message;
    BaseType_t os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(1000));
    TEST_ASSERT_MESSAGE(os_ret == pdPASS, "valid message timeout");
    const touch_button_message_t *button_message =  touch_button_get_message(&valid_message);
    if (button_message->event == TOUCH_BUTTON_EVT_ON_LONGPRESS) {
        touch_button_set_longpress(handle, portMAX_DELAY);  //Prevent button triggers LongPress event again
    }
    TEST_ASSERT_MESSAGE(handle == valid_message.handle, "check handle failed");
    TEST_ASSERT_MESSAGE(valid_message.element_type == TOUCH_ELEM_TYPE_BUTTON, "check element type failed");
    TEST_ASSERT_MESSAGE(message->event == button_message->event, "check event failed");
    xSemaphoreGive(monitor->response_sig_handle);
}
