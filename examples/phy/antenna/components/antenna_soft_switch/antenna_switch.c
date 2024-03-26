/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Antenna soft switching Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_wifi.h"
#include "esp_phy.h"
#include "esp_log.h"
#include "esp_check.h"

#include "antenna_switch.h"

#define SOFT_SWITCHING_NAME             "three ant auto"
#define SOFT_SWITCHING_PRIORITY         4
#define SOFT_SWITCHING_STACK            4096

#define WAIT_TIME                       60    /**< unit 100ms*/
#define MONITOR_TIME                    200   /**< unit 10ms*/
#define QUEUE_LENGTH_RSSI_SINGLE        10
#define QUEUE_LENGTH_RSSI_SUM           25
#define RSSI_SINGLE_SIZE                sizeof(int8_t)
#define RSSI_SUM_SIZE                   sizeof(int16_t)
#define RSSI_KD  0.01

static const char *TAG = "ANTENNA_SWITCH_EXAMPLE";
static wifi_antenna_auto_switch_config_t wifi_three_ant_auto_get_config = {
    .ant_num = ANT_TOTAL_THREE,
    .ant_zero = 0,
    .ant_one = 1,
    .ant_two = 3,
    .ant_switch = 35
};
static TaskHandle_t antenna_task_handle;

/**< Select the optimal antenna*/
static void antenna_switch_function(const wifi_antenna_auto_switch_config_t *config)
{
    esp_phy_ant_config_t wifi_ant_config;
    wifi_ap_record_t  wifi_ap_record;
    int16_t rssi_ant0 = INT16_MIN, rssi_ant1 = INT16_MIN, rssi_ant2 = INT16_MIN, rssi_max, rssi_min;

    /**< Monitor antenna zero signal strength*/
    wifi_ant_config.rx_ant_mode = ESP_PHY_ANT_MODE_ANT0;
    wifi_ant_config.rx_ant_default = ESP_PHY_ANT_MODE_ANT0;
    wifi_ant_config.tx_ant_mode = ESP_PHY_ANT_MODE_ANT0;
    wifi_ant_config.enabled_ant0 =   config->ant_zero;
    wifi_ant_config.enabled_ant1 =   config->ant_one;
    ESP_ERROR_CHECK(esp_phy_set_ant(&wifi_ant_config));
    /**< Wait for parameters to take effect*/
    vTaskDelay(100/portTICK_PERIOD_MS);
    while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
        /**< can't find AP*/
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    rssi_ant0 = 0;
    rssi_max = wifi_ap_record.rssi;
    rssi_min = wifi_ap_record.rssi;
    for(int i = 0; i < MONITOR_TIME; i++) {
       while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
        /**< can't find AP*/
         vTaskDelay(100/portTICK_PERIOD_MS);
        }
        rssi_max = rssi_max > wifi_ap_record.rssi ? rssi_max : wifi_ap_record.rssi;
        rssi_min = rssi_min < wifi_ap_record.rssi ? rssi_min : wifi_ap_record.rssi;
        rssi_ant0 += wifi_ap_record.rssi;
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    rssi_ant0 = rssi_ant0 - rssi_max - rssi_min;
    ESP_LOGD(TAG, "The signal strength of the antenna zero :%d", rssi_ant0);

    /**< Monitor antenna one signal strength*/
    wifi_ant_config.rx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
    wifi_ant_config.tx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
    ESP_ERROR_CHECK(esp_phy_set_ant(&wifi_ant_config));
    /**< Wait for parameters to take effect*/
    vTaskDelay(100/portTICK_PERIOD_MS);
    while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
        /**< can't find AP*/
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
    rssi_ant1 = 0;
    rssi_max = wifi_ap_record.rssi;
    rssi_min = wifi_ap_record.rssi;
    for(int i = 0; i < MONITOR_TIME; i++) {
        while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
            /**< can't find AP*/
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        rssi_max = rssi_max > wifi_ap_record.rssi ? rssi_max : wifi_ap_record.rssi;
        rssi_min = rssi_min < wifi_ap_record.rssi ? rssi_min : wifi_ap_record.rssi;
        rssi_ant1 += wifi_ap_record.rssi;
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
    rssi_ant1 = rssi_ant1 - rssi_max - rssi_min;
    ESP_LOGD(TAG, "The signal strength of the antenna one :%d", rssi_ant1);

    if(config->ant_num == ANT_TOTAL_THREE) {
        /**< Monitor antenna two signal strength*/
        wifi_ant_config.rx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
        wifi_ant_config.tx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
        wifi_ant_config.enabled_ant1 =   config->ant_two;
        ESP_ERROR_CHECK(esp_phy_set_ant(&wifi_ant_config));
        /**< Wait for parameters to take effect*/
        vTaskDelay(100/portTICK_PERIOD_MS);
        while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
            /**< can't find AP*/
            vTaskDelay(100/portTICK_PERIOD_MS);
        }
        rssi_ant2 = 0;
        rssi_max = wifi_ap_record.rssi;
        rssi_min = wifi_ap_record.rssi;
        for(int i = 0; i < MONITOR_TIME; i++) {
            while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
                /**< can't find AP*/
                vTaskDelay(100/portTICK_PERIOD_MS);
            }
            rssi_max = rssi_max > wifi_ap_record.rssi ? rssi_max : wifi_ap_record.rssi;
            rssi_min = rssi_min < wifi_ap_record.rssi ? rssi_min : wifi_ap_record.rssi;
            rssi_ant2 += wifi_ap_record.rssi;
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
        rssi_ant2 = rssi_ant2 - rssi_max - rssi_min;
        ESP_LOGD(TAG, "The signal strength of the antenna two :%d", rssi_ant2);
    }

    if(rssi_ant0 >= rssi_ant1 && rssi_ant0 >= rssi_ant2) {
        /**< antenna zero signal strength best*/
        ESP_LOGD(TAG, "Antenna soft switching selection ant0");
        wifi_ant_config.rx_ant_mode  =   ESP_PHY_ANT_MODE_ANT0;
        wifi_ant_config.tx_ant_mode  =   ESP_PHY_ANT_MODE_ANT0;
        wifi_ant_config.enabled_ant0 =   config->ant_zero;
        wifi_ant_config.enabled_ant1 =   config->ant_one;
        ESP_ERROR_CHECK(esp_phy_set_ant(&wifi_ant_config));
    }

    if(rssi_ant1 > rssi_ant0 && rssi_ant1 > rssi_ant2) {
        /**< antenna one signal strength best*/
        ESP_LOGD(TAG, "Antenna soft switching selection ant1");
        wifi_ant_config.rx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
        wifi_ant_config.tx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
        wifi_ant_config.enabled_ant0 =   config->ant_zero;
        wifi_ant_config.enabled_ant1 =   config->ant_one;
        ESP_ERROR_CHECK(esp_phy_set_ant(&wifi_ant_config));
    }

    if(rssi_ant2 > rssi_ant0 && rssi_ant2 > rssi_ant1) {
        /**< antenna two signal strength best*/
        ESP_LOGD(TAG, "Antenna soft switching selection ant2");
        wifi_ant_config.rx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
        wifi_ant_config.tx_ant_mode  =   ESP_PHY_ANT_MODE_ANT1;
        wifi_ant_config.enabled_ant0 =   config->ant_zero;
        wifi_ant_config.enabled_ant1 =   config->ant_two;
        ESP_ERROR_CHECK(esp_phy_set_ant(&wifi_ant_config));
    }
}

/**< Kalman filter function*/
static int16_t KalmanFilter(int16_t inData)
{
    static float  kalman = 0;
    static float  p = 10;
    /**< Process noise*/
    float  q = 0.01;
    /**< Measurement noise*/
    float  r = 0.2;
    /**< Kalman gain*/
    float  kg = 0;

    p += q;
    /**< Calculate the Kalman gain*/
    kg = p / ( p + r );
    /**< Calculate the estimated value of this filtering*/
    kalman = kalman + (kg * (inData - kalman));
    /**< Updated measurement variance*/
    p = (1 - kg) * p;

    return (int16_t)(kalman + 0.5);
}

static void antenna_soft_switching_task(void *arg)
{
    QueueHandle_t xQueue_rssi_single_handle, xQueue_rssi_sum_handle;
    wifi_ap_record_t  wifi_ap_record;
    int8_t  rssi_data_in = 0, rssi_data_out = 0;
    int16_t rssi_sum = 0, rssi_last_sum = 0, rssi_save_sum = 0;
    int8_t rssi_flag = 0;
    uint16_t queue_rssi_single_size = 0, queue_rssi_sum_size = 0;
    BaseType_t ret;

    wifi_antenna_auto_switch_config_t *config = ( wifi_antenna_auto_switch_config_t *)arg;

    xQueue_rssi_single_handle = xQueueCreate(QUEUE_LENGTH_RSSI_SINGLE, RSSI_SINGLE_SIZE);
    xQueue_rssi_sum_handle    = xQueueCreate(QUEUE_LENGTH_RSSI_SUM, RSSI_SUM_SIZE);

    while(true) {
        while(ESP_OK != esp_wifi_sta_get_ap_info(&wifi_ap_record)) {
            /**< Can't find AP*/
             vTaskDelay(100/portTICK_PERIOD_MS);
        }
        /**< Filter the current rssi*/
        rssi_data_in = KalmanFilter(wifi_ap_record.rssi);

        /**< Save real-time rssi single*/
        if (xQueueSend(xQueue_rssi_single_handle, (void *)&rssi_data_in, ( TickType_t ) 0) == pdPASS ) {
            rssi_sum += rssi_data_in;
            queue_rssi_single_size++;
            ESP_LOGD(TAG, "rssi_data_in:%d", rssi_data_in);
        }

        /**< Rssi single queue full*/
        if(queue_rssi_single_size >= RSSI_SINGLE_SIZE) {
            xQueueReceive( xQueue_rssi_single_handle, &rssi_data_out, ( TickType_t ) 0);
            queue_rssi_single_size--;
            rssi_sum -= rssi_data_out;
            /**< Save real-time rssi sum*/
            if( xQueueSend(xQueue_rssi_sum_handle, (void *)&rssi_sum, ( TickType_t ) 0) == pdPASS ) {
                queue_rssi_sum_size++;
            }

            /**< Rssi sum queue full*/
            if (queue_rssi_sum_size >= RSSI_SUM_SIZE) {
                xQueueReceive( xQueue_rssi_sum_handle, &rssi_last_sum, ( TickType_t ) 0);
                queue_rssi_sum_size--;
                if (abs(rssi_last_sum - rssi_sum)> ((config->ant_switch) * 2 / 3 ) &&  rssi_flag == 0) {
                    rssi_save_sum = rssi_last_sum;
                    rssi_flag = 1;
                    ESP_LOGD(TAG, "Start listening rssi change");
                }
                if(rssi_flag > 0) {
                    rssi_flag++;
                    /**< Wait for 6 seconds to check whether the environment is normal*/
                    if (rssi_flag > WAIT_TIME) {
                        ESP_LOGD(TAG, "End of listening rssi");
                        /**< Ambient noise factor: abs(rssi_last_sum - rssi_sum) * RSSI_KD*/
                        int16_t and = abs(rssi_save_sum - rssi_sum) - abs(rssi_last_sum - rssi_sum) * RSSI_KD;
                        rssi_flag = 0;
                        if(and > (config->ant_switch)) {
                            /**< Determine that the signal has changed*/
                            queue_rssi_sum_size = 0;
                            queue_rssi_single_size = 0;
                            ESP_LOGD(TAG, "monitor result:%d  >   %d", and, (config->ant_switch));
                            /**< Select the optimal antenna*/
                            antenna_switch_function(config);
                            /**< Wait for parameters to take effect*/
                            vTaskDelay(100 / portTICK_PERIOD_MS);
                            do
                            {
                                /**< Clear the rssi single queue*/
                                ret = xQueueReceive(xQueue_rssi_single_handle, &rssi_data_out, 0);
                            } while (ret != pdFALSE);
                            do
                            {
                                /**< Clear the rssi sum queue*/
                                ret = xQueueReceive(xQueue_rssi_sum_handle, &rssi_data_out, 0);
                            } while (ret != pdFALSE);
                        }
                    }
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

esp_err_t  esp_wifi_set_ant_soft_switch(const wifi_antenna_auto_switch_config_t *config)
{
    BaseType_t ret;

    ESP_RETURN_ON_FALSE(config->ant_num < ANT_TOTAL_MAX, ESP_ERR_INVALID_ARG, TAG, "antenna nunmbers error!");
    /**< Refresh configuration parameters*/
    wifi_three_ant_auto_get_config = *config;
    /**< Select the optimal antenna*/
    antenna_switch_function(config);

    ret = xTaskCreatePinnedToCore(antenna_soft_switching_task, SOFT_SWITCHING_NAME, SOFT_SWITCHING_STACK, (void *)config, SOFT_SWITCHING_PRIORITY, &antenna_task_handle, CONFIG_FREERTOS_NUMBER_OF_CORES - 1);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "create task %s failed", SOFT_SWITCHING_NAME);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_wifi_get_ant_soft_switch_config(wifi_antenna_auto_switch_config_t *config)
{
    *config = wifi_three_ant_auto_get_config;

    return ESP_OK;
}

void esp_deinit_ant_soft_switch(void)
{
    vTaskDelete(antenna_task_handle);
}
