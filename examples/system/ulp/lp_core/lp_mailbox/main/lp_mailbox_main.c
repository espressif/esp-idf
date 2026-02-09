/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_sleep.h"
#include "esp_err.h"
#include "lp_core_main.h"
#include "ulp_lp_core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lp_core_mailbox.h"

#define VALUES_COUNT    10

extern const uint8_t lp_core_main_bin_start[] asm("_binary_lp_core_main_bin_start");
extern const uint8_t lp_core_main_bin_end[]   asm("_binary_lp_core_main_bin_end");

static lp_mailbox_t s_mailbox;
static lp_message_t s_values[VALUES_COUNT];
static volatile SemaphoreHandle_t ready;
static volatile int idx = 0;

static void lp_core_init(void)
{
    /* Set LP core wakeup source as the HP CPU */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    /* Load LP core firmware */
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(lp_core_main_bin_start, (lp_core_main_bin_end - lp_core_main_bin_start)));

    /* Run LP core */
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));

    // printf("LP core loaded with firmware and running successfully\n");
}

static void rcv_callback(lp_message_t msg)
{
    BaseType_t task_awoken = pdFALSE;
    /* Add the new message to the array */
    s_values[idx++] = msg;
    if (idx == VALUES_COUNT) {
        xSemaphoreGiveFromISR(ready, &task_awoken);
        if (task_awoken != pdFALSE) {
            portYIELD_FROM_ISR();
        }
    }
}

void app_main(void)
{
    lp_message_t sum = 0;
    esp_err_t err = ESP_OK;
    ready = xSemaphoreCreateBinary();
    assert(ready != NULL);

    /* Load LP Core binary and start the coprocessor */
    lp_core_init();

    /* Give the LP core some time to initialize the LP mailbox. This is not strictly required
     * for targets that have a hardware mailbox but in it is required for the software implementation
     * of the mailbox since shared data will be used to communicate between HP and LP cores.  */
    vTaskDelay(100 / portTICK_PERIOD_MS);

    if ((err = lp_core_mailbox_init(&s_mailbox, NULL)) != ESP_OK) {
        printf("Could not initialize mailbox: %x\n", err);
        return;
    }
    printf("LP Mailbox initialized successfully\n");

    /* Receive the values from the LP core asynchronously */
    err = lp_core_mailbox_receive_async(s_mailbox, VALUES_COUNT, rcv_callback);
    assert(err == ESP_OK);

    /* In practice, we can do something else while we receive the messages, in this example let's just wait */
    BaseType_t success = xSemaphoreTake(ready, portMAX_DELAY);
    if (!success) {
        printf("Error %d taking the semaphore\n", err);
        return;
    }

    /* Received all the value, calculate the squares and send them back */
    for (int i = 0; i < VALUES_COUNT; i++) {
        const int val = s_values[i];
        const int snd = val * val;
        err = lp_core_mailbox_send(s_mailbox, snd, portMAX_DELAY);
        if (err != ESP_OK) {
            printf("Error receiving data from LP core\n");
            break;
        }
    }

    /* Try to receive a message synchronously, corresponding to the sum of them all */
    err = lp_core_mailbox_receive(s_mailbox, &sum, portMAX_DELAY);
    if (err != ESP_OK) {
        printf("Error receiving the sum from LP core\n");
    } else {
        printf("Final sum: %d\n", sum);
    }

    vSemaphoreDelete(ready);
}
