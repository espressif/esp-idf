/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ---------------------------------------------------------- README ------------------------------------------------
 *  This doc is aimed at explain some important code block and do some records for the test result, if developer or
 *  test-owner has some question in reading this code implementation, please read it first.
 *
 *  CODE Block:
 *  `code-block-1`: Touch Element lib need some time to finish the initialization so as to configure the right threshold.
 *                  Since some hardware issue(Must to pass 2 times "meas_done" interrupt), Touch Element lib will spend
 *                  some time(Maybe <30ms) to finish the initialization, every operations (interrupts) happen to touch
 *                  sensor will be ignored before initialization. That's why "vTaskDelay()" could be saw in after call
 *                  "touch_element_start()". However, this just for the unit test, in the real application, users don't
 *                  need to delay something.
 *
 *  NOTES:
 *  `Simulator`: Currently the event simulator depend on the touch sensor driver and play some "hack" in some register so
 *               as to raise a FAKE interrupt. It means that Touch Element lib test case must be burned on dev-kit and keep
 *               the touch channel as clean as possible, ESP32-S2-Saola is good test board.  //TODO: Remove the dependent of touch sensor driver.
 ------------------------------------------------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "unity.h"

#include "touch_element/touch_button.h"
#include "touch_element/touch_slider.h"
#include "touch_element/touch_matrix.h"

typedef struct {
    QueueHandle_t valid_msg_handle;
    SemaphoreHandle_t response_sig_handle;
} test_monitor_t;

/* ------------------------------------------------------------------------------------------------------------------ */
extern void test_button_event_simulator(touch_button_handle_t button_handle, touch_button_event_t button_event);
extern void test_button_handler(touch_button_handle_t handle, touch_button_message_t *message, void *arg);
extern void test_button_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message);
extern void test_button_event_trigger_and_check(touch_button_handle_t handle, touch_button_event_t button_event);
extern void test_button_callback_trigger_and_check(touch_button_handle_t handle, touch_button_event_t button_event, bool should_trigger, test_monitor_t *monitor);
extern void test_slider_event_simulator(touch_slider_handle_t slider_handle, touch_slider_event_t slider_event, uint32_t random);
extern void test_slider_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message);
extern void test_matrix_event_simulator(touch_matrix_handle_t matrix_handle, touch_matrix_event_t matrix_event, uint32_t pos_index);
extern void test_matrix_event_check(touch_elem_message_t *valid_message, touch_elem_message_t *current_message);
/* ------------------------------------------------------------------------------------------------------------------ */
static void test_waterproof_event_simulator(touch_pad_t guard_channel, touch_button_event_t guard_state);
static void test_system_waterproof_guard(void);
static void test_integrat_btn_sld_mat(void);
static void test_integration_monitor_task(void *arg);
/* ------------------------------------------------------------------------------------------------------------------ */
TEST_CASE("Touch element integration test", "[touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_integrat_btn_sld_mat();
    touch_element_uninstall();
}

TEST_CASE("Touch element waterproof test", "[touch_element]")
{
    touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_element_install(&global_config));
    test_system_waterproof_guard(); //TODO: add waterproof work with slider and matrix
    touch_element_uninstall();
}

static void test_system_waterproof_guard(void)
{
    static const touch_pad_t button_channel_array[12] = {
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
        TOUCH_PAD_NUM12
    };
    const uint8_t BUTTON_CHANNEL_NUM = sizeof(button_channel_array) / sizeof(touch_pad_t);

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
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, (void *)&monitor));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_CALLBACK));
        TEST_ESP_OK(touch_button_set_callback(button_handle[i], test_button_handler));
    }
    printf("Touch Element waterproof guard sensor test start\n");

    srandom((unsigned int)time(NULL));
    {//No use waterproof guard sensor
        touch_elem_waterproof_config_t waterproof_config = {
            .guard_channel = TOUCH_WATERPROOF_GUARD_NOUSE,
            .guard_sensitivity = 0.0F
        };
        TEST_ESP_OK(touch_element_waterproof_install(&waterproof_config));
        TEST_ESP_OK(touch_element_start());
        vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1
        for (int i = 0; i < 10; i++) { //Start state test
            printf("Touch Element waterproof no-use guard sensor test... (%d/10)\n", i + 1);
            touch_button_handle_t current_handle = button_handle[random() % 12];
            test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_PRESS, true, &monitor);
            test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_RELEASE, true, &monitor);
        }
        TEST_ESP_OK(touch_element_stop());
        touch_element_waterproof_uninstall();
    }

    {//Use waterproof guard sensor(Add all handles)
        touch_elem_waterproof_config_t waterproof_config = {
            .guard_channel = TOUCH_PAD_NUM13,
            .guard_sensitivity = 0.1F
        };
        TEST_ESP_OK(touch_element_waterproof_install(&waterproof_config));
        for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
            TEST_ESP_OK(touch_element_waterproof_add(button_handle[i]));
        }
        TEST_ESP_OK(touch_element_start());
        vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1
        for (int i = 0; i < 10; i++) {
            printf("Touch Element waterproof use guard sensor random trigger test... (%d/10)\n", i + 1);
            bool should_trigger = random() % 2;
            if (should_trigger) {
                touch_button_handle_t current_handle = button_handle[random() % 12];
                test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_PRESS, should_trigger, &monitor);
                test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_RELEASE, should_trigger, &monitor);
            } else {
                test_waterproof_event_simulator(waterproof_config.guard_channel, TOUCH_BUTTON_EVT_ON_PRESS);  //Waterproof guard sensor trigger
                touch_button_handle_t current_handle = button_handle[random() % 12];
                test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_PRESS, should_trigger, &monitor);
                test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_RELEASE, should_trigger, &monitor);
                test_waterproof_event_simulator(waterproof_config.guard_channel, TOUCH_BUTTON_EVT_ON_RELEASE);  //Waterproof guard sensor release
            }
        }
        TEST_ESP_OK(touch_element_stop());
        touch_element_waterproof_uninstall();
    }

    {//Put half button handles into guard ring
        const uint8_t protect_handle_threshold = BUTTON_CHANNEL_NUM / 2;
        touch_elem_waterproof_config_t waterproof_config = {
            .guard_channel = TOUCH_PAD_NUM13,
            .guard_sensitivity = 0.1F
        };
        TEST_ESP_OK(touch_element_waterproof_install(&waterproof_config));
        for (int i = 0; i < protect_handle_threshold; i++) {
            TEST_ESP_OK(touch_element_waterproof_add(button_handle[i]));
        }
        TEST_ESP_OK(touch_element_start());
        vTaskDelay(pdMS_TO_TICKS(500));  //Mention in README, code-block-1
        for (int i = 0; i < 10; i++) {
            printf("Touch Element waterproof use guard sensor test(guard sensor is triggered will half button handles)... (%d/10)\n", i + 1);
            test_waterproof_event_simulator(waterproof_config.guard_channel, TOUCH_BUTTON_EVT_ON_PRESS);  //Waterproof guard sensor trigger
            uint32_t handle_index = random() % 12;
            touch_button_handle_t current_handle = button_handle[handle_index];
            bool should_trigger = (handle_index < protect_handle_threshold) ? false : true;
            test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_PRESS, should_trigger, &monitor);
            test_button_callback_trigger_and_check(current_handle, TOUCH_BUTTON_EVT_ON_RELEASE, should_trigger, &monitor);
            test_waterproof_event_simulator(waterproof_config.guard_channel, TOUCH_BUTTON_EVT_ON_RELEASE);  //Waterproof guard sensor release
        }
        TEST_ESP_OK(touch_element_stop());
        touch_element_waterproof_uninstall();
    }

    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    touch_button_uninstall();
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
    printf("Touch Element waterproof guard sensor test finish\n");
}

static void test_waterproof_event_simulator(touch_pad_t guard_channel, touch_button_event_t guard_state)
{
    if (guard_state == TOUCH_BUTTON_EVT_ON_PRESS) {
        touch_pad_set_cnt_mode(guard_channel, TOUCH_PAD_SLOPE_3, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else if (guard_state == TOUCH_BUTTON_EVT_ON_RELEASE) {
        touch_pad_set_cnt_mode(guard_channel, TOUCH_PAD_SLOPE_7, TOUCH_PAD_TIE_OPT_DEFAULT);
    } else {
        printf("guard sensor simulator doesn't support this operation\n");
    }
    /* Fixme: If the normal instance and guard sensor trigger at the same time, guard sensor will lock the state failed */
    vTaskDelay(pdMS_TO_TICKS(100));
}

static void test_integrat_btn_sld_mat(void)
{
    static const touch_pad_t button_channel_array[3] = {
        TOUCH_PAD_NUM1,
        TOUCH_PAD_NUM2,
        TOUCH_PAD_NUM3
    };
    static const float button_sens_array[3] = {
        0.1F,
        0.1F,
        0.1F
    };
    static const touch_pad_t slider_channel_array[5] = {
        TOUCH_PAD_NUM4,
        TOUCH_PAD_NUM5,
        TOUCH_PAD_NUM6,
        TOUCH_PAD_NUM7,
        TOUCH_PAD_NUM8
    };
    static const float slider_sens_array[5] = {
        0.1F,
        0.1F,
        0.1F,
        0.1F,
        0.1F
    };
    static const touch_pad_t x_axis_channel[3] = {
        TOUCH_PAD_NUM9,
        TOUCH_PAD_NUM10,
        TOUCH_PAD_NUM11,
    };
    static const touch_pad_t y_axis_channel[3] = {
        TOUCH_PAD_NUM12,
        TOUCH_PAD_NUM13,
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
    const uint8_t BUTTON_CHANNEL_NUM = sizeof(button_channel_array) / sizeof(touch_pad_t);
    const uint8_t SLIDER_CHANNEL_NUM = sizeof(slider_channel_array) / sizeof(touch_pad_t);
    const uint8_t MATRIX_CHANNEL_NUM_X = sizeof(x_axis_channel) / sizeof(touch_pad_t);
    const uint8_t MATRIX_CHANNEL_NUM_Y = sizeof(y_axis_channel) / sizeof(touch_pad_t);

    printf("Integration test(button + slider + matrix) start\n");

    BaseType_t os_ret;
    touch_button_handle_t button_handle[BUTTON_CHANNEL_NUM];
    touch_slider_handle_t slider_handle;
    touch_matrix_handle_t matrix_handle;
    test_monitor_t monitor;
    TaskHandle_t task_handle = NULL;

    monitor.valid_msg_handle = xQueueCreate(10, sizeof(touch_elem_message_t));
    monitor.response_sig_handle = xSemaphoreCreateBinary();
    os_ret = xTaskCreate(&test_integration_monitor_task, "test_integration_monitor_task", 4096, (void *)&monitor, 5, &task_handle);
    TEST_ASSERT(monitor.valid_msg_handle != NULL && monitor.response_sig_handle != NULL && os_ret == pdPASS);

    touch_button_global_config_t button_global_config = TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG();
    touch_slider_global_config_t slider_global_config = TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG();
    touch_matrix_global_config_t matrix_global_config = TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG();
    TEST_ESP_OK(touch_button_install(&button_global_config));
    TEST_ESP_OK(touch_slider_install(&slider_global_config));
    TEST_ESP_OK(touch_matrix_install(&matrix_global_config));

    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        touch_button_config_t button_config = {
            .channel_num = button_channel_array[i],
            .channel_sens = button_sens_array[i]
        };
        TEST_ESP_OK(touch_button_create(&button_config, &button_handle[i]));
        TEST_ESP_OK(touch_button_subscribe_event(button_handle[i], TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL));
        TEST_ESP_OK(touch_button_set_dispatch_method(button_handle[i], TOUCH_ELEM_DISP_EVENT));
    }

    touch_slider_config_t slider_config = {
        .channel_array = slider_channel_array,
        .sensitivity_array = slider_sens_array,
        .channel_num = SLIDER_CHANNEL_NUM,
        .position_range = 101
    };
    TEST_ESP_OK(touch_slider_create(&slider_config, &slider_handle));
    TEST_ESP_OK(touch_slider_subscribe_event(slider_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL));
    TEST_ESP_OK(touch_slider_set_dispatch_method(slider_handle, TOUCH_ELEM_DISP_EVENT));

    touch_matrix_config_t matrix_config = {
        .x_channel_array = x_axis_channel,
        .y_channel_array = y_axis_channel,
        .x_sensitivity_array = x_axis_channel_sens,
        .y_sensitivity_array = y_axis_channel_sens,
        .x_channel_num = MATRIX_CHANNEL_NUM_X,
        .y_channel_num = MATRIX_CHANNEL_NUM_Y
    };
    TEST_ESP_OK(touch_matrix_create(&matrix_config, &matrix_handle));
    TEST_ESP_OK(touch_matrix_subscribe_event(matrix_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL));
    TEST_ESP_OK(touch_matrix_set_dispatch_method(matrix_handle, TOUCH_ELEM_DISP_EVENT));
    TEST_ESP_OK(touch_element_start());

    vTaskDelay(pdMS_TO_TICKS(500)); //Mention in README, code-block-1

    srandom((unsigned int)time(NULL));
    for (int i = 0; i < 30; i++) {
        printf("Integration test... (%d/30)\n", i + 1);
        touch_elem_message_t valid_message;
        touch_button_message_t button_message;
        touch_slider_message_t slider_message;
        touch_matrix_message_t matrix_message;

        valid_message.element_type = (random() % (TOUCH_ELEM_TYPE_MATRIX + 1));
        if (valid_message.element_type == TOUCH_ELEM_TYPE_BUTTON) {
            uint32_t button_index = random() % BUTTON_CHANNEL_NUM;
            valid_message.handle = button_handle[button_index];
            button_message.event = TOUCH_BUTTON_EVT_ON_PRESS;
            memcpy(valid_message.child_msg, &button_message, sizeof(button_message));  //Construct child message
            xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
            test_button_event_simulator(valid_message.handle, button_message.event);
        } else if (valid_message.element_type == TOUCH_ELEM_TYPE_SLIDER) {
            valid_message.handle = slider_handle;
            slider_message.event = TOUCH_SLIDER_EVT_ON_PRESS;
            slider_message.position = 0; //No check
            memcpy(valid_message.child_msg, &slider_message, sizeof(slider_message));  //Construct child message
            xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
            test_slider_event_simulator(valid_message.handle, slider_message.event, 1);
        } else if (valid_message.element_type == TOUCH_ELEM_TYPE_MATRIX) {
            uint32_t matrix_x_axis_index = random() % MATRIX_CHANNEL_NUM_X;
            uint32_t matrix_y_axis_index = random() % MATRIX_CHANNEL_NUM_Y;
            valid_message.handle = matrix_handle;
            matrix_message.event = TOUCH_MATRIX_EVT_ON_PRESS;
            matrix_message.position.x_axis = matrix_x_axis_index;
            matrix_message.position.y_axis = matrix_y_axis_index;
            matrix_message.position.index = matrix_x_axis_index * MATRIX_CHANNEL_NUM_Y + matrix_y_axis_index;
            memcpy(valid_message.child_msg, &matrix_message, sizeof(matrix_message));  //Construct child message
            xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
            test_matrix_event_simulator(valid_message.handle, matrix_message.event, matrix_message.position.index);
        } else {
            TEST_ABORT();
        }
        os_ret = xSemaphoreTake(monitor.response_sig_handle, pdMS_TO_TICKS(500));
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "response queue timeout (500ms)");

        if (valid_message.element_type == TOUCH_ELEM_TYPE_BUTTON) {
            button_message.event = TOUCH_BUTTON_EVT_ON_RELEASE;
            memcpy(valid_message.child_msg, &button_message, sizeof(button_message));
            xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
            test_button_event_simulator(valid_message.handle, button_message.event);
        } else if (valid_message.element_type == TOUCH_ELEM_TYPE_SLIDER) {
            slider_message.event = TOUCH_SLIDER_EVT_ON_RELEASE;
            memcpy(valid_message.child_msg, &slider_message, sizeof(slider_message));
            xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
            test_slider_event_simulator(valid_message.handle, slider_message.event, 1);
        } else if (valid_message.element_type == TOUCH_ELEM_TYPE_MATRIX) {
            matrix_message.event = TOUCH_MATRIX_EVT_ON_RELEASE;
            memcpy(valid_message.child_msg, &matrix_message, sizeof(matrix_message));
            xQueueSend(monitor.valid_msg_handle, &valid_message, portMAX_DELAY);
            const touch_matrix_message_t *matrix_message_ptr = touch_matrix_get_message(&valid_message);
            test_matrix_event_simulator(valid_message.handle, matrix_message.event, matrix_message_ptr->position.index);
        }
        os_ret = xSemaphoreTake(monitor.response_sig_handle, pdMS_TO_TICKS(500));
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "response queue timeout (500ms)");
    }

    TEST_ESP_OK(touch_element_stop());
    for (int i = 0; i < BUTTON_CHANNEL_NUM; i++) {
        TEST_ESP_OK(touch_button_delete(button_handle[i]));
    }
    TEST_ESP_OK(touch_slider_delete(slider_handle));
    TEST_ESP_OK(touch_matrix_delete(matrix_handle));
    touch_button_uninstall();
    touch_slider_uninstall();
    touch_matrix_uninstall();

    while (eTaskGetState(task_handle) == eRunning) {
        vTaskDelay(pdTICKS_TO_MS(1));
    }
    vTaskDelete(task_handle);
    vQueueDelete(monitor.valid_msg_handle);
    vSemaphoreDelete(monitor.response_sig_handle);
    printf("Integration test(button + slider + matrix) finish\n");
}

static void test_integration_monitor_task(void *arg)
{
    test_monitor_t *monitor = (test_monitor_t *)arg;
    BaseType_t os_ret;
    touch_elem_message_t current_message, valid_message;
    while (1) {
        touch_element_message_receive(&current_message, portMAX_DELAY);
        os_ret = xQueueReceive(monitor->valid_msg_handle, &valid_message, pdMS_TO_TICKS(500)); //Get the valid message for the verification, 500ms timeout
        TEST_ASSERT_MESSAGE(os_ret == pdPASS, "trigger queue timeout (500ms)");
        if (current_message.element_type == TOUCH_ELEM_TYPE_BUTTON) {
            test_button_event_check(&valid_message, &current_message);
        } else if (current_message.element_type == TOUCH_ELEM_TYPE_SLIDER) {
            test_slider_event_check(&valid_message, &current_message);
        } else if (current_message.element_type == TOUCH_ELEM_TYPE_MATRIX) {
            test_matrix_event_check(&valid_message, &current_message);
        }
        xSemaphoreGive(monitor->response_sig_handle);
    }
}
