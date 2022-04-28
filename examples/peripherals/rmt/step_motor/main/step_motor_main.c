/* RMT example -- step motor */

/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "step_motor.h"
#include "step_motor_driver_io_a4988.h"

// GPIO configuration
#define STEP_MOTOR_DIRECTION_PIN GPIO_NUM_18
#define STEP_MOTOR_STEP_PIN GPIO_NUM_17
#define STEP_MOTOR_SLEEP_PIN GPIO_NUM_16
#define STEP_MOTOR_RESET_PIN GPIO_NUM_15
#define STEP_MOTOR_MS3_PIN GPIO_NUM_7
#define STEP_MOTOR_MS2_PIN GPIO_NUM_6
#define STEP_MOTOR_MS1_PIN GPIO_NUM_5
#define STEP_MOTOR_ENABLE_PIN GPIO_NUM_4

#define RMT_TX_CHANNEL RMT_CHANNEL_0

static const char *TAG = "step_motor";

void app_main(void)
{
    // Apply default RMT configuration
    rmt_config_t dev_config = RMT_DEFAULT_CONFIG_TX(STEP_MOTOR_STEP_PIN, RMT_TX_CHANNEL);

    step_motor_io_a4988_conf_t a4988_conf = {
        .direction_pin = STEP_MOTOR_DIRECTION_PIN,
        .sleep_pin = STEP_MOTOR_SLEEP_PIN,
        .reset_pin = STEP_MOTOR_RESET_PIN,
        .ms3_pin = STEP_MOTOR_MS3_PIN,
        .ms2_pin = STEP_MOTOR_MS2_PIN,
        .ms1_pin = STEP_MOTOR_MS1_PIN,
        .enable_pin = STEP_MOTOR_ENABLE_PIN,
    };

    // Install low part driver
    step_motor_driver_io_t *a4988_io;
    ESP_ERROR_CHECK(step_motor_new_a4988_io_driver(&a4988_conf, &a4988_io));

    // Install rmt driver
    step_motor_t *motor = NULL;
    ESP_ERROR_CHECK(step_motor_create_rmt(a4988_io, &dev_config, &motor));

    step_motor_init(motor);
    ESP_LOGI(TAG, "init");

    ESP_LOGI(TAG, "set_step");
    // configure Microstep to Full Step
    step_motor_set_step(motor, 1, STEP_MOTOR_DIRECTION_POSITIVE);
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "step 10 @ 1000/s");
    step_motor_step(motor, 10, 1000);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "step 100 @ 1000/s");
    step_motor_step(motor, 100, 1000);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "step 1000 @ 1200/s");
    step_motor_step(motor, 1000, 1200);
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "step 5000 @ 1400/s");
    step_motor_step(motor, 5000, 1400);
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "smoothstep start 5000 steps @ 500~1400/s");
    step_motor_smooth_step(motor, 5000, 1000, 500, 1400);
    ESP_LOGI(TAG, "smoothstep finish");
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "continuous running for 5s");
    step_motor_step(motor, UINT32_MAX, 1000);
    vTaskDelay(pdMS_TO_TICKS(5000));
    ESP_LOGI(TAG, "stop");
    step_motor_step(motor, 0, 1000);

    vTaskDelay(pdMS_TO_TICKS(1000));
    step_motor_deinit(motor);
    ESP_LOGI(TAG, "deinit");
    ESP_ERROR_CHECK(step_motor_delete_rmt(motor));

    ESP_ERROR_CHECK(step_motor_delete_a4988_io_driver(a4988_io));
}
