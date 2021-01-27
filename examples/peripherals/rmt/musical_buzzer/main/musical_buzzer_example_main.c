/* RMT example -- Musical Buzzer

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_log.h"
#include "driver/rmt.h"
#include "musical_buzzer.h"

static const char *TAG = "example";

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_TX_GPIO_NUM (4)

/**
 * @brief Musical Notation: Beethoven's Ode to joy
 *
 */
static const musical_buzzer_notation_t notation[] = {
    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {740, 400}, {740, 200}, {659, 200}, {659, 800},

    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {659, 400}, {659, 200}, {587, 200}, {587, 800},

    {659, 400}, {659, 400}, {740, 400}, {587, 400},
    {659, 400}, {740, 200}, {784, 200}, {740, 400}, {587, 400},
    {659, 400}, {740, 200}, {784, 200}, {740, 400}, {659, 400},
    {587, 400}, {659, 400}, {440, 400}, {440, 400},

    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {659, 400}, {659, 200}, {587, 200}, {587, 800},
};

void app_main(void)
{
    // Apply default RMT configuration
    rmt_config_t dev_config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO_NUM, RMT_TX_CHANNEL);
    dev_config.tx_config.loop_en = true; // Enable loop mode

    // Install RMT driver
    ESP_ERROR_CHECK(rmt_config(&dev_config));
    ESP_ERROR_CHECK(rmt_driver_install(RMT_TX_CHANNEL, 0, 0));

    // This example take the RMT channel number as the device handle
    musical_buzzer_config_t buzzer_config = MUSICAL_BUZZER_DEFAULT_CONFIG((musical_buzzer_dev_t)RMT_TX_CHANNEL);
    musical_buzzer_t *buzzer = NULL;
    // Install buzzer driver
    ESP_ERROR_CHECK(musical_buzzer_create_rmt(&buzzer_config, &buzzer));

    ESP_LOGI(TAG, "Playing Beethoven's Ode to joy");

    ESP_ERROR_CHECK(buzzer->play(buzzer, notation, sizeof(notation) / sizeof(notation[0])));
}
