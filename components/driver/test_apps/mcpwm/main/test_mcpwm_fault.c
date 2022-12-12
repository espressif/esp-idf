/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/mcpwm_fault.h"
#include "driver/mcpwm_oper.h"
#include "driver/gpio.h"

TEST_CASE("mcpwm_fault_install_uninstall", "[mcpwm]")
{
    printf("install and uninstall gpio faults\r\n");
    mcpwm_gpio_fault_config_t gpio_fault_config = {
        .gpio_num = 0,
    };
    int total_gpio_faults = SOC_MCPWM_GPIO_FAULTS_PER_GROUP * SOC_MCPWM_GROUPS;
    mcpwm_fault_handle_t gpio_faults[total_gpio_faults];
    int fault_itor = 0;
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        gpio_fault_config.group_id = i;
        for (int j = 0; j < SOC_MCPWM_GPIO_FAULTS_PER_GROUP; j++) {
            TEST_ESP_OK(mcpwm_new_gpio_fault(&gpio_fault_config, &gpio_faults[fault_itor++]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_gpio_fault(&gpio_fault_config, &gpio_faults[0]));
    }
    for (int i = 0; i < total_gpio_faults; i++) {
        TEST_ESP_OK(mcpwm_del_fault(gpio_faults[i]));
    }

    printf("install and uninstall software fault\r\n");
    mcpwm_soft_fault_config_t soft_fault_config = {};
    mcpwm_fault_handle_t soft_fault = NULL;
    TEST_ESP_OK(mcpwm_new_soft_fault(&soft_fault_config, &soft_fault));
    TEST_ESP_OK(mcpwm_del_fault(soft_fault));
}

static bool test_fault_enter_callback(mcpwm_fault_handle_t detector, const mcpwm_fault_event_data_t *status, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    esp_rom_printf("fault found\r\n");
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static bool test_fault_exit_callback(mcpwm_fault_handle_t detector, const mcpwm_fault_event_data_t *status, void *user_data)
{
    TaskHandle_t task_handle = (TaskHandle_t)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    esp_rom_printf("fault relieved\r\n");
    vTaskNotifyGiveFromISR(task_handle, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("mcpwm_gpio_fault_event_callbacks", "[mcpwm]")
{
    printf("create gpio fault\r\n");
    const int fault_gpio = 0;
    mcpwm_fault_handle_t fault = NULL;
    mcpwm_gpio_fault_config_t gpio_fault_config = {
        .group_id = 0,
        .gpio_num = fault_gpio,
        .flags.active_level = true, // active on high level
        .flags.pull_down = true,
        .flags.io_loop_back = true, // for debug, so that we can use gpio_set_level to mimic a fault source
    };
    TEST_ESP_OK(mcpwm_new_gpio_fault(&gpio_fault_config, &fault));

    // put fault GPIO into a safe state
    gpio_set_level(fault_gpio, 0);

    printf("register callback for the gpio fault\r\n");
    mcpwm_fault_event_callbacks_t cbs = {
        .on_fault_enter = test_fault_enter_callback,
        .on_fault_exit = test_fault_exit_callback,
    };
    TaskHandle_t task_to_notify = xTaskGetCurrentTaskHandle();
    TEST_ESP_OK(mcpwm_fault_register_event_callbacks(fault, &cbs, task_to_notify));
    TEST_ASSERT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));

    printf("trigget a fault event\r\n");
    gpio_set_level(fault_gpio, 1);
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10)));

    printf("remove the fault source\r\n");
    gpio_set_level(fault_gpio, 0);
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(10)));

    TEST_ESP_OK(mcpwm_del_fault(fault));
}
