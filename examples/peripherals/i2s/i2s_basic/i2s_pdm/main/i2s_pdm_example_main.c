/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "i2s_pdm_example.h"

#define EXAMPLE_PDM_TX      0
/* ESP32-C3 does not support PDM RX currently */
#if !CONFIG_IDF_TARGET_ESP32C3
#define EXAMPLE_PDM_RX      1
#endif

#define EXAMPLE_PDM_DIR     EXAMPLE_PDM_TX

void app_main(void)
{
#if EXAMPLE_PDM_DIR == EXAMPLE_PDM_TX
    printf("I2S PDM TX example start\n---------------------------\n");
    xTaskCreate(i2s_example_pdm_tx_task, "i2s_example_pdm_tx_task", 4096, NULL, 5, NULL);
#else
    printf("I2S PDM RX example start\n---------------------------\n");
    xTaskCreate(i2s_example_pdm_rx_task, "i2s_example_pdm_rx_task", 4096, NULL, 5, NULL);
#endif
}
