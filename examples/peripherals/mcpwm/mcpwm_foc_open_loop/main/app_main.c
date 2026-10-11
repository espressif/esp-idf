/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#include "esp_err.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "board_inverter.h"
#include "foc_core.h"

static const char *TAG = "mcpwm_foc_open_loop";

/* Notify the FOC loop on each MCPWM timer full event. */
static bool inverter_update_callback(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *event_data, void *user_ctx)
{
    (void)timer;
    (void)event_data;
    SemaphoreHandle_t sem_update = (SemaphoreHandle_t)user_ctx;
    BaseType_t high_task_woken = pdFALSE;
    xSemaphoreGiveFromISR(sem_update, &high_task_woken);
    return high_task_woken == pdTRUE;
}

void app_main(void)
{
    ESP_LOGI(TAG, "MCPWM motor FOC example started");

    SemaphoreHandle_t sem_update = xSemaphoreCreateCounting(1, 0);
    ESP_ERROR_CHECK(sem_update ? ESP_OK : ESP_ERR_NO_MEM);

    inverter_config_t inverter_config = BOARD_INVERTER_DEFAULT_CONFIG();
    inverter_config.timer_callbacks.on_full = inverter_update_callback;
    inverter_config.timer_callback_user_ctx = sem_update;

    inverter_handle_t inverter;
    ESP_ERROR_CHECK(board_inverter_new(&inverter_config, &inverter));

    foc_core_t core;
    foc_core_init(&core, inverter);

    ESP_ERROR_CHECK(board_inverter_startstop(inverter, MCPWM_TIMER_START_NO_STOP));
    ESP_LOGI(TAG, "Inverter PWM started");

    ESP_ERROR_CHECK(board_inverter_gate_enable(inverter, true));
    ESP_LOGI(TAG, "Gate driver enabled");

    while (true) {
        xSemaphoreTake(sem_update, portMAX_DELAY);

        ESP_ERROR_CHECK(foc_core_execute(&core));
    }
}
