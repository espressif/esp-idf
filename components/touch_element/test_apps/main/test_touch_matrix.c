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
#include "touch_element/touch_matrix.h"

static const touch_pad_t x_axis_channel[3] = {
    TOUCH_PAD_NUM5,
    TOUCH_PAD_NUM7,
    TOUCH_PAD_NUM9,
};
static const touch_pad_t y_axis_channel[3] = {
    TOUCH_PAD_NUM11,
    TOUCH_PAD_NUM12,
    TOUCH_PAD_NUM14,
};
static const float x_axis_channel_sens[3] = {
    0.1F,
    0.1F,
    0.1F,
};
static const float y_axis_channel_sens[3] = {
    0.1F,
    0.1F,
    0.1F,
};
const uint8_t MATRIX_CHANNEL_NUM_X = sizeof(x_axis_channel) / sizeof(touch_pad_t);
const uint8_t MATRIX_CHANNEL_NUM_Y = sizeof(y_axis_channel) / sizeof(touch_pad_t);

typedef struct {
    QueueHandle_t valid_msg_handle;
    SemaphoreHandle_t response_sig_handle;
} test_monitor_t;

/* ------------------------------------------------------------------------------------------------------------------ */
void test_matrix_event_simulator(touch_matrix_handle_t matrix_handle, touch_matrix_event_t matrix_event, uint32_t pos_index);
static void test_matrix_channel_simulator(touch_pad_t channel, touch_matrix_event_t matrix_event);
void test_matrix_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message);
static void test_matrix_callback_check(touch_matrix_handle_t current_handle, touch_matrix_message_t *current_message, touch_elem_message_t *valid_message);
void test_matrix_event_trigger_and_check(touch_matrix_handle_t handle, touch_matrix_event_t matrix_event, uint32_t pos_index);
void test_matrix_callback_trigger_and_check(touch_matrix_handle_t handle, touch_matrix_event_t matrix_event, uint32_t pos_index, bool should_trigger, test_monitor_t *monitor);
/* ------------------------------------------------ Dispatch method test -------------------------------------------- */
static void test_matrix_disp_event(void);
static void test_matrix_disp_callback(void);
static void test_matrix_handler(touch_matrix_handle_t handle, touch_matrix_message_t *message, void *arg);
/* ------------------------------------------------ Run-time test --------------------------------------------------- */
static void test_matrix_event_change_lp(void);
static void test_matrix_callback_change_lp(void);
static void test_matrix_change_lp_handler(touch_matrix_handle_t out_handle, touch_matrix_message_t *out_message, void *arg);
/* ----------------------------------------------- Random channel trigger test -------------------------------------- */
static void test_matrix_random_channel_trigger(void);
/* ------------------------------------------------------------------------------------------------------------------ */

TEST_CASE("Touch matrix dispatch methods test", "[matrix][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_matrix_disp_event();
    test_matrix_disp_callback();
    touch_element_uninstall();
}

TEST_CASE("Touch matrix run-time test", "[matrix][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_matrix_event_change_lp();
    test_matrix_callback_change_lp();
    touch_element_uninstall();
}

TEST_CASE("Touch matrix random channel trigger test", "[matrix][touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_matrix_random_channel_trigger();
    touch_element_uninstall();
}

void test_matrix_event_simulator(touch_matrix_handle_t matrix_handle, touch_matrix_event_t matrix_event, uint32_t pos_index)
{
    te_matrix_handle_t te_matrix = (te_matrix_handle_t) matrix_handle;
    touch_pad_t x_channel = te_matrix->device[pos_index / te_matrix->y_channel_num]->channel;
    touch_pad_t y_channel = te_matrix->device[te_matrix->x_channel_num + (pos_index % te_matrix->y_channel_num)]->channel;
    if (matrix_event == TOUCH_MATRIX_EVT_ON_PRESS) {
        touch_pad_set_cnt_mode(x_channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
        touch_pad_set_cnt_mode(y_channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else if (matrix_event == TOUCH_MATRIX_EVT_ON_RELEASE) {
        touch_pad_set_cnt_mode(x_channel, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
        touch_pad_set_cnt_mode(y_channel, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else {
        touch_pad_set_cnt_mode(x_channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT); // LongPress
        touch_pad_set_cnt_mode(y_channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
    }
}

static void test_matrix_channel_simulator(touch_pad_t channel, touch_matrix_event_t matrix_event)
{
    if (matrix_event == TOUCH_MATRIX_EVT_ON_PRESS) {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else if (matrix_event == TOUCH_MATRIX_EVT_ON_RELEASE) {
        touch_pad_set_cnt_mode(channel, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
    }
}

void test_matrix_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message)
{
    TEST_ASSERT_MESSAGE(current_message->handle == valid_message->handle, "check handle failed");
    TEST_ASSERT_MESSAGE(current_message->element_type == valid_message->element_type, "check element type failed");
    const touch_matrix_message_t *valid_matrix_message = touch_matrix_get_message(valid_message);
    const touch_matrix_message_t *current_matrix_message = touch_matrix_get_message(current_message);
    TEST_ASSERT_MESSAGE(current_matrix_message->event == valid_matrix_message->event, "check event failed");
    TEST_ASSERT_MESSAGE(current_matrix_message->position.index == valid_matrix_message->position.index, "check index failed");
    TEST_ASSERT_MESSAGE(current_matrix_message->position.x_axis == valid_matrix_message->position.x_axis, "check x_axis failed");
    TEST_ASSERT_MESSAGE(current_matrix_message->position.y_axis == valid_matrix_message->position.y_axis, "check y_axis failed");
}

static inline void test_matrix_callback_check(touch_matrix_handle_t current_handle, touch_matrix_message_t *current_message, touch_elem_message_t *valid_message)
{
    const touch_matrix_message_t *valid_matrix_message = touch_matrix_get_message(valid_message);
    TEST_ASSERT_MESSAGE(valid_message->handle == current_handle, "check handle failed");
    TEST_ASSERT_MESSAGE(valid_message->element_type == TOUCH_ELEM_TYPE_MATRIX, "check element type failed");
    TEST_ASSERT_MESSAGE(valid_matrix_message->event == current_message->event, "check event failed");
    TEST_ASSERT_MESSAGE(valid_matrix_message->position.index == current_message->position.index, "check index failed");
    TEST_ASSERT_MESSAGE(valid_matrix_message->position.x_axis == current_message->position.x_axis, "check x_axis failed");
    TEST_ASSERT_MESSAGE(valid_matrix_message->position.y_axis == current_message->position.y_axis, "check y_axis failed");
}

void test_matrix_event_trigger_and_check(touch_matrix_handle_t handle, touch_matrix_event_t matrix_event, uint32_t pos_index)
{
    touch_elem_message_t valid_message = {
        .handle = handle,
        .element_type = TOUCH_ELEM_TYPE_MATRIX,
        .arg = NULL
    };
    touch_matrix_message_t matrix_message = {
        .event = matrix_event,
        .position.index = pos_index,
        .position.x_axis = pos_index / MATRIX_CHANNEL_NUM_Y,
        .position.y_axis = pos_index % MATRIX_CHANNEL_NUM_Y
    };
    memcpy(valid_message.child_msg, &matrix_message, sizeof(touch_matrix_message_t));  //Construct valid_message

    test_matrix_event_simulator(handle, matrix_event, pos_index);  //Trigger signal

    touch_elem_message_t current_message;
    te_matrix_handle_t te_matrix = handle;
    esp_err_t ret = touch_element_message_receive(&current_message, pdMS_TO_TICKS(2 * te_matrix->trigger_thr * 10));  //Get current message for verification
    TEST_ASSERT_MESSAGE(ret == ESP_OK, "matrix event receive timeout");

    test_matrix_event_check(&valid_message, &current_message);  //Verification
}

void test_matrix_callback_trigger_and_check(touch_matrix_handle_t handle, touch_matrix_event_t matrix_event, uint32_t pos_index, bool should_trigger, test_monitor_t *monitor)
{
    if (should_trigger) {
        touch_elem_message_t valid_message = {
            .handle = handle,
            .element_type = TOUCH_ELEM_TYPE_MATRIX,
            .arg = NULL
        };
        touch_matrix_message_t matrix_message = {
            .event = matrix_event,
            .position.index = pos_index,
            .position.x_axis = pos_index / MATRIX_CHANNEL_NUM_Y,
            .position.y_axis = pos_index % MATRIX_CHANNEL_NUM_Y
        };
        memcpy(valid_message.child_msg, &matrix_message, sizeof(touch_matrix_message_t));  //Construct valid_message
        xQueueSend(monitor->valid_msg_handle, &valid_message, portMAX_DELAY);
    }

    test_matrix_event_simulator(handle, matrix_event, pos_index);  //Trigger signal

    te_matrix_handle_t te_matrix = handle;
    if (should_trigger) {
        BaseType_t os_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(2 * te_matrix->trigger_thr * 10));
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "Matrix queue timeout");
    } else {
        BaseType_t os_ret = xSemaphoreTake(monitor->response_sig_handle, pdMS_TO_TICKS(500));
        TEST_ASSERT_MESSAGE(os_ret == pdFALSE, "Matrix invalid trigger");
    }
}

static void test_matrix_disp_event(void)
{
    touch_matrix_handle_t matrix_handle = NULL;
    touch_matrix_global_config_t global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_matrix_install(&global_config));
    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = MATRIX_CHANNEL_NUM_X,
        .y_channel_num = MATRIX_CHANNEL_NUM_Y
    };
    TEST_ESP_OK(touch_matrix_create(&matrix_config, &matrix_handle));
    TEST_ESP_OK(touch_matrix_subscribe_event(matrix_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_LONGPRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL));
    TEST_ESP_OK(touch_matrix_set_longpress(matrix_handle, 300));
    TEST_ESP_OK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_EVENT));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    printf("Touch matrix event test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch matrix event test... (%d/10)\n", i + 1);
        uint32_t button_num = random() % ( MATRIX_CHANNEL_NUM_X * MATRIX_CHANNEL_NUM_Y );
        test_matrix_event_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_PRESS, button_num);
        test_matrix_event_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_LONGPRESS, button_num);
        test_matrix_event_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_RELEASE, button_num);
    }
    printf("Touch matrix event test finish\n");

    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_matrix_delete(matrix_handle));
    touch_matrix_uninstall();
}

static void test_matrix_disp_callback(void)
{
    test_monitor_t monitor;
    touch_matrix_handle_t matrix_handle = NULL;
    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    TEST_ASSERT(monitor.valid_msg_handle != NULL || monitor.response_sig_handle != NULL);

    touch_matrix_global_config_t global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_matrix_install(&global_config));
    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = MATRIX_CHANNEL_NUM_X,
        .y_channel_num = MATRIX_CHANNEL_NUM_Y
    };
    TEST_ESP_OK(touch_matrix_create(&matrix_config, &matrix_handle));
    TEST_ESP_OK(touch_matrix_subscribe_event(matrix_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_LONGPRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *)&monitor));
    TEST_ESP_OK(touch_matrix_set_longpress(matrix_handle, 300));
    TEST_ESP_OK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_CALLBACK));
    TEST_ESP_OK(touch_matrix_set_callback(matrix_handle, test_matrix_handler));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    printf("Touch matrix callback test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch matrix callback test... (%d/10)\n", i + 1);
        uint32_t button_num = random() % (MATRIX_CHANNEL_NUM_X * MATRIX_CHANNEL_NUM_Y);
        test_matrix_callback_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_PRESS, button_num, true, &monitor);
        test_matrix_callback_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_LONGPRESS, button_num, true, &monitor);
        test_matrix_callback_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_RELEASE, button_num, true, &monitor);
    }
    printf("Touch matrix callback test finish\n");

    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_matrix_delete(matrix_handle));
    touch_matrix_uninstall();
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
}

static void test_matrix_handler(touch_matrix_handle_t handle, touch_matrix_message_t *message, void *arg)
{
    test_monitor_t *monitor = (test_monitor_t *)arg;
    touch_elem_message_t valid_message;
    BaseType_t os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(200));
    TEST_ASSERT_MESSAGE(os_ret == pdPASS, "test_matrix_handler: queue timeout");
    test_matrix_callback_check(handle, message, &valid_message);
    xSemaphoreGive(monitor->response_sig_handle);
}

static void test_matrix_random_channel_trigger(void)
{
    test_monitor_t monitor;
    touch_matrix_handle_t matrix_handle = NULL;
    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    TEST_ASSERT(monitor.valid_msg_handle != NULL || monitor.response_sig_handle != NULL);

    touch_matrix_global_config_t global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_matrix_install(&global_config));
    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = MATRIX_CHANNEL_NUM_X,
        .y_channel_num = MATRIX_CHANNEL_NUM_Y
    };
    TEST_ESP_OK(touch_matrix_create(&matrix_config, &matrix_handle));
    TEST_ESP_OK(touch_matrix_subscribe_event(matrix_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *) &monitor));
    TEST_ESP_OK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_CALLBACK));
    TEST_ESP_OK(touch_matrix_set_callback(matrix_handle, test_matrix_handler));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    printf("Touch matrix random channel trigger test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch matrix random channel trigger test... (%d/10)\n", i + 1);
        uint32_t channel_index_1 = random() % (MATRIX_CHANNEL_NUM_X + MATRIX_CHANNEL_NUM_Y);
        uint32_t channel_index_2 = random() % (MATRIX_CHANNEL_NUM_X + MATRIX_CHANNEL_NUM_Y);
        touch_pad_t channel_1 = (channel_index_1 < MATRIX_CHANNEL_NUM_X) ? x_axis_channel[channel_index_1] : y_axis_channel[channel_index_1 - MATRIX_CHANNEL_NUM_X];
        touch_pad_t channel_2 = (channel_index_2 < MATRIX_CHANNEL_NUM_X) ? x_axis_channel[channel_index_2] : y_axis_channel[channel_index_2 - MATRIX_CHANNEL_NUM_X];
        if ((channel_index_1 <= 2 && channel_index_2 <= 2) || (channel_index_1 > 2 && channel_index_2 > 2)) {  //all x channels triggered or all y channels triggered
            //Should not be triggered
            BaseType_t os_ret;
            test_matrix_channel_simulator(channel_1, TOUCH_MATRIX_EVT_ON_PRESS);
            test_matrix_channel_simulator(channel_2, TOUCH_MATRIX_EVT_ON_PRESS);
            os_ret = xSemaphoreTake(monitor.response_sig_handle, pdMS_TO_TICKS(500));
            TEST_ASSERT_MESSAGE(os_ret == pdFAIL, "Matrix Press event invalid trigger");

            test_matrix_channel_simulator(channel_1, TOUCH_MATRIX_EVT_ON_RELEASE);
            test_matrix_channel_simulator(channel_2, TOUCH_MATRIX_EVT_ON_RELEASE);
            os_ret = xSemaphoreTake(monitor.response_sig_handle, pdMS_TO_TICKS(500));
            TEST_ASSERT_MESSAGE(os_ret == pdFAIL, "Matrix Release event invalid trigger");
        } else {
            //Should be triggered
            uint8_t button_num;
            if (channel_index_1 <= 2) {
                button_num = channel_index_1 * matrix_config.y_channel_num + (channel_index_2 - MATRIX_CHANNEL_NUM_X);
            } else {
                button_num = channel_index_2 * matrix_config.x_channel_num + (channel_index_1 - MATRIX_CHANNEL_NUM_Y);
            }
            test_matrix_callback_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_PRESS, button_num, true, &monitor);
            test_matrix_callback_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_RELEASE, button_num, true, &monitor);
        }
    }
    printf("Touch matrix random channel trigger test finish\n");

    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_matrix_delete(matrix_handle));
    touch_matrix_uninstall();
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
}

static void test_matrix_event_change_lp(void)
{
    touch_matrix_handle_t matrix_handle = NULL;
    touch_matrix_global_config_t global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_matrix_install(&global_config));
    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = MATRIX_CHANNEL_NUM_X,
        .y_channel_num = MATRIX_CHANNEL_NUM_Y
    };
    TEST_ESP_OK(touch_matrix_create(&matrix_config, &matrix_handle));
    TEST_ESP_OK(touch_matrix_subscribe_event(matrix_handle, TOUCH_ELEM_EVENT_ON_LONGPRESS, NULL));
    TEST_ESP_OK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_EVENT));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    //10 times random press/longpress/release test
    printf("Touch matrix event change longtime test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch matrix event change longtime test... (%d/10)\n", i + 1);
        uint32_t button_num = random() % ( MATRIX_CHANNEL_NUM_X * MATRIX_CHANNEL_NUM_Y );
        TEST_ESP_OK(touch_matrix_set_longpress(matrix_handle, 200 + (i + 1) * 50));
        test_matrix_event_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_LONGPRESS, button_num);
        test_matrix_event_simulator(matrix_handle, TOUCH_MATRIX_EVT_ON_RELEASE, button_num); //Reset hardware
        vTaskDelay(pdMS_TO_TICKS(100));  //Fixme: Waiting for driver core handle release event
    }
    printf("Touch matrix event change longtime test finish\n");
    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_matrix_delete(matrix_handle));
    touch_matrix_uninstall();
}

static void test_matrix_callback_change_lp(void)
{
    test_monitor_t monitor;
    touch_matrix_handle_t matrix_handle = NULL;
    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    TEST_ASSERT(monitor.valid_msg_handle != NULL || monitor.response_sig_handle != NULL);

    touch_matrix_global_config_t global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_matrix_install(&global_config));
    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = MATRIX_CHANNEL_NUM_X,
        .y_channel_num = MATRIX_CHANNEL_NUM_Y
    };
    TEST_ESP_OK(touch_matrix_create(&matrix_config, &matrix_handle));
    TEST_ESP_OK(touch_matrix_subscribe_event(matrix_handle, TOUCH_ELEM_EVENT_ON_LONGPRESS, (void *)&monitor));
    TEST_ESP_OK(touch_matrix_set_longpress(matrix_handle, 300));
    TEST_ESP_OK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_CALLBACK));
    TEST_ESP_OK(touch_matrix_set_callback(matrix_handle, test_matrix_change_lp_handler));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    printf("Touch matrix callback change longtime test start\n");
    for (int i = 0; i < 10; i++) {
        printf("Touch matrix callback change longtime test... (%d/10)\n", i + 1);
        uint32_t button_num = random() % (MATRIX_CHANNEL_NUM_X * MATRIX_CHANNEL_NUM_Y);
        test_matrix_callback_trigger_and_check(matrix_handle, TOUCH_MATRIX_EVT_ON_LONGPRESS, button_num, true, &monitor);
        test_matrix_event_simulator(matrix_handle, TOUCH_MATRIX_EVT_ON_RELEASE, button_num); //Reset hardware
        vTaskDelay(pdMS_TO_TICKS(100));  //Fixme: Waiting for driver core handle release event

    }
    printf("Touch matrix callback change longtime test finish\n");

    TEST_ESP_OK(touch_element_stop());
    TEST_ESP_OK(touch_matrix_delete(matrix_handle));
    touch_matrix_uninstall();
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
}

static void test_matrix_change_lp_handler(touch_matrix_handle_t out_handle, touch_matrix_message_t *out_message, void *arg)
{
    test_monitor_t *monitor = (test_monitor_t *)arg;
    touch_elem_message_t valid_message;
    BaseType_t os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(200));  //500ms timeout
    TEST_ASSERT_MESSAGE(os_ret == pdPASS, "test_matrix_handler: queue timeout");
    test_matrix_callback_check(out_handle, out_message, &valid_message);
    xSemaphoreGive(monitor->response_sig_handle);
    TEST_ESP_OK(touch_matrix_set_longpress(valid_message.handle, 300)); // Always 300ms
}
