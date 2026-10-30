/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_vddbat.h"

static const char *TAG = "vddbat_example";

/* A rejected deep sleep is retried until VDDBAT recovers or the cell is empty */
#define SLEEP_RETRY_MS          100

static StaticSemaphore_t s_under_volt_sem_buf;

static IRAM_ATTR bool under_volt_isr_cb(vddbat_protect_handle_t handle, void *user_arg)
{
    BaseType_t task_unblocked = pdFALSE;

    /* Stop further reports until the task has handled this one */
    esp_vddbat_protect_disable(handle);
    xSemaphoreGiveFromISR((SemaphoreHandle_t)user_arg, &task_unblocked);
    return task_unblocked == pdTRUE;
}

static void under_volt_handler(vddbat_protect_handle_t protect)
{
    ESP_LOGW(TAG, "VDDBAT under voltage, shutting down");

    while (esp_vddbat_is_under_volt(protect)) {
        if (esp_vddbat_prepare_deep_sleep(protect) != ESP_OK) {
            ESP_LOGI(TAG, "deep sleep skipped, VDDBAT already recovered");
            break;
        }
        /* Powers down unless rejected. Other shutdown work can run before this call. */
        esp_deep_sleep_try_to_start();
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_VDDBAT_UP_VOLT);
        if (!esp_vddbat_is_under_volt(protect)) {
            ESP_LOGI(TAG, "deep sleep rejected, VDDBAT already recovered");
            break;
        }
        /* Another enabled wakeup source is already pending and keeps rejecting the sleep */
        ESP_LOGW(TAG, "deep sleep rejected, VDDBAT still low, retry later");
        vTaskDelay(pdMS_TO_TICKS(SLEEP_RETRY_MS));
    }

    /* Disabled in the callback, enable it again now that VDDBAT recovered */
    ESP_ERROR_CHECK(esp_vddbat_protect_enable(protect));
    ESP_LOGI(TAG, "VDDBAT recovered, protection enabled again");
}

void app_main(void)
{
    SemaphoreHandle_t under_volt_sem = xSemaphoreCreateBinaryStatic(&s_under_volt_sem_buf);

    vddbat_protect_config_t config = VDDBAT_PROTECT_DEFAULT_CONFIG();
    vddbat_protect_handle_t protect = NULL;
    ESP_ERROR_CHECK(esp_vddbat_protect_install(&config, &protect));

    vddbat_protect_event_callbacks_t cbs = {
        .on_under_volt = under_volt_isr_cb,
    };
    ESP_ERROR_CHECK(esp_vddbat_protect_register_callbacks(protect, &cbs, under_volt_sem));
    ESP_ERROR_CHECK(esp_vddbat_protect_enable(protect));

    ESP_LOGI(TAG, "VDDBAT protection enabled, waiting for under voltage");

    while (true) {
        xSemaphoreTake(under_volt_sem, portMAX_DELAY);
        under_volt_handler(protect);
    }
}
