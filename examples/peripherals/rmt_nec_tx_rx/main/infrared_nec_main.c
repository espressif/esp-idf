/* NEC remote infrared RMT example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/rmt.h"
#include "driver/periph_ctrl.h"
extern void rmt_nec_tx_task();
extern void rmt_nec_rx_task();

void app_main()
{
    xTaskCreate(rmt_nec_rx_task, "rmt_nec_rx_task", 2048, NULL, 10, NULL);
    xTaskCreate(rmt_nec_tx_task, "rmt_nec_tx_task", 2048, NULL, 10, NULL);
}
