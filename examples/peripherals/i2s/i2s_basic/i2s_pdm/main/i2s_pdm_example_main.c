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

void app_main(void)
{
#if CONFIG_EXAMPLE_PDM_TX
    printf("I2S PDM TX example start\n---------------------------\n");
    xTaskCreate(i2s_example_pdm_tx_task, "i2s_example_pdm_tx_task", 4096, NULL, 5, NULL);
#elif CONFIG_EXAMPLE_PDM_RX
    printf("I2S PDM RX example start\n---------------------------\n");
    xTaskCreate(i2s_example_pdm_rx_task, "i2s_example_pdm_rx_task", 4096, NULL, 5, NULL);
#endif
}
