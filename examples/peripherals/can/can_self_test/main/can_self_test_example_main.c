/* CAN Self Test Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * The following example demonstrates the self testing capabilities of the CAN
 * peripheral by utilizing the No Acknowledgment Mode and Self Reception Request
 * capabilities. This example can be used to verify that the CAN peripheral and
 * its connections to the external transceiver operates without issue. The example
 * will execute multiple iterations, each iteration will do the following:
 * 1) Start the CAN driver
 * 2) Transmit and receive 100 messages using self reception request
 * 3) Stop the CAN driver
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/can.h"

/* --------------------- Definitions and static variables ------------------ */

//Example Configurations
#define NO_OF_MSGS              100
#define NO_OF_ITERS             3
#define TX_GPIO_NUM             21
#define RX_GPIO_NUM             22
#define TX_TASK_PRIO            8       //Sending task priority
#define RX_TASK_PRIO            9       //Receiving task priority
#define CTRL_TSK_PRIO           10      //Control task priority
#define MSG_ID                  0x555   //11 bit standard format ID
#define EXAMPLE_TAG             "CAN Self Test"

static const can_timing_config_t t_config = CAN_TIMING_CONFIG_25KBITS();
//Filter all other IDs except MSG_ID
static const can_filter_config_t f_config = {.acceptance_code = (MSG_ID << 21),
                                             .acceptance_mask = ~(CAN_STD_ID_MASK << 21),
                                             .single_filter = true};
//Set to NO_ACK mode due to self testing with single module
static const can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, CAN_MODE_NO_ACK);

static SemaphoreHandle_t tx_sem;
static SemaphoreHandle_t rx_sem;
static SemaphoreHandle_t ctrl_sem;
static SemaphoreHandle_t done_sem;

/* --------------------------- Tasks and Functions -------------------------- */

static void can_transmit_task(void *arg)
{
    can_message_t tx_msg = {.data_length_code = 1, .identifier = MSG_ID, .flags = CAN_MSG_FLAG_SELF};
    for (int iter = 0; iter < NO_OF_ITERS; iter++) {
        xSemaphoreTake(tx_sem, portMAX_DELAY);
        for (int i = 0; i < NO_OF_MSGS; i++) {
            //Transmit messages using self reception request
            tx_msg.data[0] = i;
            ESP_ERROR_CHECK(can_transmit(&tx_msg, portMAX_DELAY));
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    vTaskDelete(NULL);
}

static void can_receive_task(void *arg)
{
    can_message_t rx_message;
    for (int iter = 0; iter < NO_OF_ITERS; iter++) {
        xSemaphoreTake(rx_sem, portMAX_DELAY);
        for (int i = 0; i < NO_OF_MSGS; i++) {
            //Receive message and print message data
            ESP_ERROR_CHECK(can_receive(&rx_message, portMAX_DELAY));
            ESP_LOGI(EXAMPLE_TAG, "Msg received - Data = %d", rx_message.data[0]);
        }
        //Indicate to control task all messages received for this iteration
        xSemaphoreGive(ctrl_sem);
    }
    vTaskDelete(NULL);
}

static void can_control_task(void *arg)
{
    xSemaphoreTake(ctrl_sem, portMAX_DELAY);
    for (int iter = 0; iter < NO_OF_ITERS; iter++) {
        //Start CAN Driver for this iteration
        ESP_ERROR_CHECK(can_start());
        ESP_LOGI(EXAMPLE_TAG, "Driver started");

        //Trigger TX and RX tasks to start transmitting/receiving
        xSemaphoreGive(rx_sem);
        xSemaphoreGive(tx_sem);
        xSemaphoreTake(ctrl_sem, portMAX_DELAY);    //Wait for TX and RX tasks to finish iteration

        ESP_ERROR_CHECK(can_stop());                //Stop the CAN Driver
        ESP_LOGI(EXAMPLE_TAG, "Driver stopped");
        vTaskDelay(pdMS_TO_TICKS(100));             //Delay then start next iteration
    }
    xSemaphoreGive(done_sem);
    vTaskDelete(NULL);
}

void app_main()
{
    //Create tasks and synchronization primitives
    tx_sem = xSemaphoreCreateBinary();
    rx_sem = xSemaphoreCreateBinary();
    ctrl_sem = xSemaphoreCreateBinary();
    done_sem = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(can_control_task, "CAN_ctrl", 4096, NULL, CTRL_TSK_PRIO, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(can_receive_task, "CAN_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(can_transmit_task, "CAN_tx", 4096, NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);

    //Install CAN driver
    ESP_ERROR_CHECK(can_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(EXAMPLE_TAG, "Driver installed");

    //Start control task
    xSemaphoreGive(ctrl_sem);
    //Wait for all iterations and tasks to complete running
    xSemaphoreTake(done_sem, portMAX_DELAY);

    //Uninstall CAN driver
    ESP_ERROR_CHECK(can_driver_uninstall());
    ESP_LOGI(EXAMPLE_TAG, "Driver uninstalled");

    //Cleanup
    vSemaphoreDelete(tx_sem);
    vSemaphoreDelete(rx_sem);
    vSemaphoreDelete(ctrl_sem);
    vQueueDelete(done_sem);
}

