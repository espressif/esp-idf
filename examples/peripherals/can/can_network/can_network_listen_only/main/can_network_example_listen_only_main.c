/* CAN Network Listen Only Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * The following example demonstrates a Listen Only node in a CAN network. The
 * Listen Only node will not take part in any CAN bus activity (no acknowledgments
 * and no error frames). This example will execute multiple iterations, with each
 * iteration the Listen Only node will do the following:
 * 1) Listen for ping and ping response
 * 2) Listen for start command
 * 3) Listen for data messages
 * 4) Listen for stop and stop response
 */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/can.h"

/* --------------------- Definitions and static variables ------------------ */
//Example Configuration
#define NO_OF_ITERS                     3
#define RX_TASK_PRIO                    9
#define TX_GPIO_NUM                     21
#define RX_GPIO_NUM                     22
#define EXAMPLE_TAG                     "CAN Listen Only"

#define ID_MASTER_STOP_CMD              0x0A0
#define ID_MASTER_START_CMD             0x0A1
#define ID_MASTER_PING                  0x0A2
#define ID_SLAVE_STOP_RESP              0x0B0
#define ID_SLAVE_DATA                   0x0B1
#define ID_SLAVE_PING_RESP              0x0B2

static const can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();
static const can_timing_config_t t_config = CAN_TIMING_CONFIG_25KBITS();
//Set TX queue length to 0 due to listen only mode
static const can_general_config_t g_config = {.mode = CAN_MODE_LISTEN_ONLY,
                                              .tx_io = TX_GPIO_NUM, .rx_io = RX_GPIO_NUM,
                                              .clkout_io = CAN_IO_UNUSED, .bus_off_io = CAN_IO_UNUSED,
                                              .tx_queue_len = 0, .rx_queue_len = 5,
                                              .alerts_enabled = CAN_ALERT_NONE,
                                              .clkout_divider = 0};

static SemaphoreHandle_t rx_sem;

/* --------------------------- Tasks and Functions -------------------------- */

static void can_receive_task(void *arg)
{
    xSemaphoreTake(rx_sem, portMAX_DELAY);
    bool start_cmd = false;
    bool stop_resp = false;
    uint32_t iterations = 0;

    while (iterations < NO_OF_ITERS) {
        can_message_t rx_msg;
        can_receive(&rx_msg, portMAX_DELAY);
        if (rx_msg.identifier == ID_MASTER_PING) {
            ESP_LOGI(EXAMPLE_TAG, "Received master ping");
        } else if (rx_msg.identifier == ID_SLAVE_PING_RESP) {
            ESP_LOGI(EXAMPLE_TAG, "Received slave ping response");
        } else if (rx_msg.identifier == ID_MASTER_START_CMD) {
            ESP_LOGI(EXAMPLE_TAG, "Received master start command");
            start_cmd = true;
        } else if (rx_msg.identifier == ID_SLAVE_DATA) {
            uint32_t data = 0;
            for (int i = 0; i < rx_msg.data_length_code; i++) {
                data |= (rx_msg.data[i] << (i * 8));
            }
            ESP_LOGI(EXAMPLE_TAG, "Received data value %d", data);
        } else if (rx_msg.identifier == ID_MASTER_STOP_CMD) {
            ESP_LOGI(EXAMPLE_TAG, "Received master stop command");
        } else if (rx_msg.identifier == ID_SLAVE_STOP_RESP) {
            ESP_LOGI(EXAMPLE_TAG, "Received slave stop response");
            stop_resp = true;
        }
        if (start_cmd && stop_resp) {
            //Each iteration is complete after a start command and stop response is received
            iterations++;
            start_cmd = 0;
            stop_resp = 0;
        }
    }

    xSemaphoreGive(rx_sem);
    vTaskDelete(NULL);
}

void app_main()
{
    rx_sem = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(can_receive_task, "CAN_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);

    //Install and start CAN driver
    ESP_ERROR_CHECK(can_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(EXAMPLE_TAG, "Driver installed");
    ESP_ERROR_CHECK(can_start());
    ESP_LOGI(EXAMPLE_TAG, "Driver started");

    xSemaphoreGive(rx_sem);                     //Start RX task
    vTaskDelay(pdMS_TO_TICKS(100));
    xSemaphoreTake(rx_sem, portMAX_DELAY);      //Wait for RX task to complete

    //Stop and uninstall CAN driver
    ESP_ERROR_CHECK(can_stop());
    ESP_LOGI(EXAMPLE_TAG, "Driver stopped");
    ESP_ERROR_CHECK(can_driver_uninstall());
    ESP_LOGI(EXAMPLE_TAG, "Driver uninstalled");

    //Cleanup
    vSemaphoreDelete(rx_sem);
}
