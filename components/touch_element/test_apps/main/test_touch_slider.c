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

#include "touch_element/touch_element_private.h"
#include "touch_element/touch_slider.h"

static const touch_pad_t slider_channel_array[5] = {
    TOUCH_PAD_NUM1,
    TOUCH_PAD_NUM2,
    TOUCH_PAD_NUM3,
    TOUCH_PAD_NUM4,
    TOUCH_PAD_NUM5
};
static const float slider_sens_array[5] = {
    0.1F,
    0.1F,
    0.1F,
    0.1F,
    0.1F
};
const uint8_t SLIDER_CHANNEL_NUM = sizeof(slider_channel_array) / sizeof(touch_pad_t);

typedef struct {
    QueueHandle_t valid_msg_handle;
    SemaphoreHandle_t response_sig_handle;
} test_monitor_t;

/* ------------------------------------------------------------------------------------------------------------------ */
void test_slider_event_simulator(touch_slider_handle_t slider_handle, touch_slider_event_t slider_event, uint32_t random);
void test_slider_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message);
static void test_slider_callback_check(touch_slider_handle_t current_handle, touch_slider_message_t *current_message, touch_elem_message_t *valid_message);
void test_slider_event_trigger_and_check(touch_slider_handle_t handle, touch_slider_event_t slider_event, uint32_t random_channel);
void test_slider_callback_trigger_and_check(touch_slider_handle_t handle, touch_slider_event_t slider_event, bool should_trigger, test_monitor_t *monitor, uint32_t random_channel);
/* ------------------------------------------------ Dispatch method test -------------------------------------------- */
static void test_slider_disp_event(void);
static void test_slider_disp_callback(void);
static void test_slider_handler(touch_slider_handle_t handle, touch_slider_message_t *message, void *arg);
/* ------------------------------------------------------------------------------------------------------------------ */

TEST_CASE("Touch slider dispatch methods test", "[slider][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_slider_disp_event();
    test_slider_disp_callback();
    touch_element_uninstall();
}

void test_slider_event_simulator(touch_slider_handle_t slider_handle, touch_slider_event_t slider_event, uint32_t random)
{
    te_slider_handle_t te_slider = (te_slider_handle_t) slider_handle;
    touch_pad_t channel = te_slider->device[random % te_slider->channel_sum]->channel;
    if (slider_event == TOUCH_SLIDER_EVT_ON_PRESS) {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else if (slider_event == TOUCH_SLIDER_EVT_ON_RELEASE) {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
    }
}

void test_slider_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message)
{
    TEST_ASSERT_MESSAGE(current_message->handle == valid_message->handle, "check handle failed");
    TEST_ASSERT_MESSAGE(current_message->element_type == valid_message->element_type, "check element type failed");
    const touch_slider_message_t *valid_slider_message = touch_slider_get_message(valid_message);
    const touch_slider_message_t *current_slider_message = touch_slider_get_message(current_message);
    TEST_ASSERT_MESSAGE(current_slider_message->event == valid_slider_message->event, "check event failed");
}

static void test_slider_callback_check(touch_slider_handle_t current_handle, touch_slider_message_t *current_message, touch_elem_message_t *valid_message)
{
    const touch_slider_message_t *valid_slider_message = touch_slider_get_message(valid_message);
    TEST_ASSERT_MESSAGE(valid_message->handle == current_handle, "check handle failed");
    TEST_ASSERT_MESSAGE(valid_message->element_type == TOUCH_ELEM_TYPE_SLIDER, "check element type failed");
    TEST_ASSERT_MESSAGE(valid_slider_message->event == current_message->event, "check event failed");
}

void test_slider_event_trigger_and_check(touch_slider_handle_t handle, touch_slider_event_t slider_event, uint32_t random_channel)
{
    touch_elem_message_t valid_message, current_message;
    touch_slider_message_t slider_message;
    valid_message.handle = handle;
    valid_message.element_type = TOUCH_ELEM_TYPE_SLIDER;
    slider_message.event = slider_event;
    memcpy(valid_message.child_msg, &slider_message, sizeof(touch_slider_message_t));
    test_slider_event_simulator(handle, slider_event, random_channel);
    esp_err_t ret = touch_element_message_receive(&current_message, 300);
    TEST_ASSERT_MESSAGE(ret == ESP_OK, "slider event receive timeout");
    test_slider_event_check(&valid_message, &current_message);
}

void test_slider_callback_trigger_and_check(touch_slider_handle_t handle, touch_slider_event_t slider_event, bool should_trigger, test_monitor_t *monitor, uint32_t random_channel)
{
    if (should_trigger) {
        touch_elem_message_t valid_message = {
            .handle = handle,
            .element_type = TOUCH_ELEM_TYPE_SLIDER,
            .arg = NULL
        };
        touch_slider_message_t slider_message = {
            .event = slider_event,
            .position = 0  //No use
        };
        memcpy(valid_message.child_msg, &slider_message, sizeof(touch_slider_message_t));  //Construct valid_message
        xQueueSend(monitor->valid_msg_handle, &valid_message, portMAX_DELAY);
    }

    test_slider_event_simulator(handle, slider_event, random_channel);  //Trigger signal

    BaseType_t os_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(300));
    if (should_trigger) {
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "Button queue timeout");
    } else {
        TEST_ASSERT_MESSAGE(os_ret == pdFALSE, "Button invalid trigger");
    }
}


static void test_slider_disp_event(void)
{
    touch_slider_handle_t slider_handle;
    touch_slider_global_config_t global_config = TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_slider_install(&global_config));
    /*< Create Touch Slider */
    touch_slider_config_t slider_config = {
        .channel_array = slider_channel_array,
        .sensitivity_array = slider_sens_array,
        .channel_num = SLIDER_CHANNEL_NUM,
        .position_range = 101
    };
    TEST_ESP_OK(touch_slider_create(&slider_config, &slider_handle));
    TEST_ESP_OK(touch_slider_subscribe_event(slider_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *) slider_handle));
    TEST_ESP_OK(touch_slider_set_dispatch_method(slider_handle, TOUCH_ELEM_DISP_EVENT));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    //10 times random (x channels) press/release test
    printf("Touch slider event test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch slider event test... (%d/10)\n", i + 1);
        uint32_t random_channel = random() % SLIDER_CHANNEL_NUM;
        test_slider_event_trigger_and_check(slider_handle, TOUCH_SLIDER_EVT_ON_PRESS, random_channel);
        test_slider_event_trigger_and_check(slider_handle, TOUCH_SLIDER_EVT_ON_RELEASE, random_channel);
    }
    printf("Touch slider event test finish\n");
    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_slider_delete(slider_handle));
    touch_slider_uninstall();
}


static void test_slider_disp_callback(void)
{
    test_monitor_t monitor;
    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    TEST_ASSERT(monitor.valid_msg_handle != NULL || monitor.response_sig_handle != NULL);

    touch_slider_handle_t slider_handle;
    touch_slider_global_config_t global_config = TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_slider_install(&global_config));
    touch_slider_config_t slider_config = {
        .channel_array = slider_channel_array,
        .sensitivity_array = slider_sens_array,
        .channel_num = SLIDER_CHANNEL_NUM,
        .position_range = 101
    };
    TEST_ESP_OK(touch_slider_create(&slider_config, &slider_handle));
    TEST_ESP_OK(touch_slider_subscribe_event(slider_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *) &monitor));
    TEST_ESP_OK(touch_slider_set_dispatch_method(slider_handle, TOUCH_ELEM_DISP_CALLBACK));
    TEST_ESP_OK(touch_slider_set_callback(slider_handle, test_slider_handler));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    printf("Touch slider callback test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch slider callback test... (%d/10)\n", i + 1);
        uint32_t random_channel = random() % SLIDER_CHANNEL_NUM;
        test_slider_callback_trigger_and_check(slider_handle, TOUCH_SLIDER_EVT_ON_PRESS, true, &monitor, random_channel);
        test_slider_callback_trigger_and_check(slider_handle, TOUCH_SLIDER_EVT_ON_RELEASE, true, &monitor, random_channel);
    }
    printf("Touch slider callback test finish\n");

    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_slider_delete(slider_handle));
    touch_slider_uninstall();
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
}

static void test_slider_handler(touch_slider_handle_t handle, touch_slider_message_t *message, void *arg)
{
    test_monitor_t *monitor = (test_monitor_t *)arg;
    touch_elem_message_t valid_message;
    BaseType_t os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(200));  //Get the valid message for verification, 200ms timeout
    TEST_ASSERT_MESSAGE(os_ret == pdPASS, "test_slider_handler: queue timeout");

    test_slider_callback_check(handle, message, &valid_message);  //Verification

    xSemaphoreGive(monitor->response_sig_handle);
}
