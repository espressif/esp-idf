/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "hal/gpio_ll.h"
#include "soc/interrupts.h"
#include "example_gpio.h"

extern volatile int nmi_triggered;

extern void xt_nmi(void*);

void app_main(void)
{
    intr_handle_t handle;
    esp_err_t err;

    printf("example: Start\n");

    gpio_reset_pin(EXAMPLE_GPIO_IN);
    /* Make sure we have a pull-down on the input GPIO to prevent noise (when disconnected) */
    gpio_pulldown_en(EXAMPLE_GPIO_IN);
    gpio_set_direction(EXAMPLE_GPIO_IN, GPIO_MODE_INPUT_OUTPUT);

    /* Register the interrupt handler as an NMI. When registering high level interrupts,
     * the interrupt allocator expects the handler passed as an argument to be NULL. */
    err = esp_intr_alloc(ETS_GPIO_INTR_SOURCE, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_NMI, NULL, NULL, &handle);
    if (err != ESP_OK) {
        printf("Failure: could not install NMI ISR, %d(0x%x)\n", err, err);
        return;
    }
    gpio_set_intr_type(EXAMPLE_GPIO_IN, GPIO_INTR_HIGH_LEVEL);
    gpio_intr_enable(EXAMPLE_GPIO_IN);

    vTaskDelay(200 / portTICK_PERIOD_MS);

    /* Disable interrupts on the CPU side and make sure the NMI is still triggered */
    const uint32_t mask = esp_cpu_intr_get_enabled_mask();
    esp_cpu_intr_disable(0xFFFFFFFF);
    nmi_triggered = 0;

    /* Setting EXAMPLE_GPIO_IN to 1 will trigger the NMI interrupt. */
    gpio_set_level(EXAMPLE_GPIO_IN, 1);

    /* Wait for the interrupt to occur */
    while (nmi_triggered == 0) {
        /* We cannot use vTaskDelay since the interrupts are disabled */
    }

    esp_cpu_intr_enable(mask);

    gpio_intr_disable(EXAMPLE_GPIO_IN);
    esp_intr_free(handle);
    printf("example: Success\n");
}
