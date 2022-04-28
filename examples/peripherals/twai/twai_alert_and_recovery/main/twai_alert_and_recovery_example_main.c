/* TWAI Alert and Recovery Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/*
 * The following example demonstrates how to use the alert and bus recovery
 * features of the TWAI driver. The example will do the following:
 * 1) Install and start the TWAI driver
 * 2) Have the TX task periodically broadcast messages expecting no ACK
 * 3) Reconfigure alerts to detect bus-off state
 * 4) Trigger bus errors by inverting TX GPIO
 * 5) Initiate bus-off recovery and wait for completion
 * 6) Uninstall TWAI driver
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_rom_gpio.h"
#include "esp_rom_sys.h"
#include "soc/gpio_sig_map.h" // For TWAI_TX_IDX

/* --------------------- Definitions and static variables ------------------ */
//Example Configuration
#define TX_GPIO_NUM                     CONFIG_EXAMPLE_TX_GPIO_NUM
#define RX_GPIO_NUM                     CONFIG_EXAMPLE_RX_GPIO_NUM
#define TX_TASK_PRIO                    9
#define CTRL_TASK_PRIO                  10
#define ERR_DELAY_US                    800     //Approximate time for arbitration phase at 25KBPS
#define ERR_PERIOD_US                   80      //Approximate time for two bits at 25KBPS
#define EXAMPLE_TAG                     "TWAI Alert and Recovery"

static const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NO_ACK);
static const twai_message_t tx_msg = {.identifier = 0, .data_length_code = 0};

static SemaphoreHandle_t tx_task_sem;
static SemaphoreHandle_t ctrl_task_sem;
static bool trigger_tx_error = false;

/* --------------------------- Tasks and Functions -------------------------- */

static void invert_tx_bits(bool enable)
{
    if (enable) {
        //Inverts output of TX to trigger errors
        esp_rom_gpio_connect_out_signal(TX_GPIO_NUM, TWAI_TX_IDX, true, false);
    } else {
        //Returns TX to default settings
        esp_rom_gpio_connect_out_signal(TX_GPIO_NUM, TWAI_TX_IDX, false, false);
    }
}

static void tx_task(void *arg)
{
    xSemaphoreTake(tx_task_sem, portMAX_DELAY);
    while (1) {
        if (twai_transmit(&tx_msg, 0) == ESP_ERR_INVALID_STATE) {
            break;  //Exit TX task when bus-off state is reached
        }
        if (trigger_tx_error) {
            //Trigger a bit error in transmission by inverting GPIO
            esp_rom_delay_us(ERR_DELAY_US);     //Wait until arbitration phase is over
            invert_tx_bits(true);           //Trigger bit error for a few bits
            esp_rom_delay_us(ERR_PERIOD_US);
            invert_tx_bits(false);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    vTaskDelete(NULL);
}

static void ctrl_task(void *arg)
{
    xSemaphoreTake(ctrl_task_sem, portMAX_DELAY);
    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(EXAMPLE_TAG, "Driver started");
    ESP_LOGI(EXAMPLE_TAG, "Starting transmissions");
    xSemaphoreGive(tx_task_sem);    //Start transmit task

    //Prepare to trigger errors, reconfigure alerts to detect change in error state
    twai_reconfigure_alerts(TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_OFF, NULL);
    for (int i = 3; i > 0; i--) {
        ESP_LOGW(EXAMPLE_TAG, "Trigger TX errors in %d", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    ESP_LOGI(EXAMPLE_TAG, "Trigger errors");
    trigger_tx_error = true;

    while (1) {
        uint32_t alerts;
        twai_read_alerts(&alerts, portMAX_DELAY);
        if (alerts & TWAI_ALERT_ABOVE_ERR_WARN) {
            ESP_LOGI(EXAMPLE_TAG, "Surpassed Error Warning Limit");
        }
        if (alerts & TWAI_ALERT_ERR_PASS) {
            ESP_LOGI(EXAMPLE_TAG, "Entered Error Passive state");
        }
        if (alerts & TWAI_ALERT_BUS_OFF) {
            ESP_LOGI(EXAMPLE_TAG, "Bus Off state");
            //Prepare to initiate bus recovery, reconfigure alerts to detect bus recovery completion
            twai_reconfigure_alerts(TWAI_ALERT_BUS_RECOVERED, NULL);
            for (int i = 3; i > 0; i--) {
                ESP_LOGW(EXAMPLE_TAG, "Initiate bus recovery in %d", i);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            twai_initiate_recovery();    //Needs 128 occurrences of bus free signal
            ESP_LOGI(EXAMPLE_TAG, "Initiate bus recovery");
        }
        if (alerts & TWAI_ALERT_BUS_RECOVERED) {
            //Bus recovery was successful, exit control task to uninstall driver
            ESP_LOGI(EXAMPLE_TAG, "Bus Recovered");
            break;
        }
    }
    //No need call twai_stop(), bus recovery will return to stopped state
    xSemaphoreGive(ctrl_task_sem);
    vTaskDelete(NULL);
}

void app_main(void)
{
    tx_task_sem = xSemaphoreCreateBinary();
    ctrl_task_sem = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(tx_task, "TWAI_tx", 4096, NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(ctrl_task, "TWAI_ctrl", 4096, NULL, CTRL_TASK_PRIO, NULL, tskNO_AFFINITY);

    //Install TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, & f_config));
    ESP_LOGI(EXAMPLE_TAG, "Driver installed");

    xSemaphoreGive(ctrl_task_sem);                  //Start control task
    vTaskDelay(pdMS_TO_TICKS(100));
    xSemaphoreTake(ctrl_task_sem, portMAX_DELAY);   //Wait for completion

    //Uninstall TWAI driver
    ESP_ERROR_CHECK(twai_driver_uninstall());
    ESP_LOGI(EXAMPLE_TAG, "Driver uninstalled");

    //Cleanup
    vSemaphoreDelete(tx_task_sem);
    vSemaphoreDelete(ctrl_task_sem);
}
