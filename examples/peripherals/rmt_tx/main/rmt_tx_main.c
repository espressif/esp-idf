/* RMT transmit example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"

static const char *RMT_TX_TAG = "RMT Tx";

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define RMT_TX_GPIO 18
#define RMT_MESSAGE_LENGTH 12

rmt_item32_t items[RMT_MESSAGE_LENGTH];

/*
 * Prepare a table with a message in the Morse code
 *
 * The message is "ESP" : . ... .--.
 *
 * The table structure:
 * {duration, level, duration, level}
 *
 */
uint32_t message[RMT_MESSAGE_LENGTH][4] = {
    // E : dot
    {32767, 1, 32767, 0}, // dot
    //
    {32767, 0, 32767, 0}, // SPACE
    // S : dot, dot, dot
    {32767, 1, 32767, 0}, // dot
    {32767, 1, 32767, 0}, // dot
    {32767, 1, 32767, 0}, // dot
    //
    {32767, 0, 32767, 0}, // SPACE
    // P : dot, dash, dash, dot
    {32767, 1, 32767, 0}, // dot
    {32767, 1, 32767, 1},
    {32767, 1, 32767, 0}, // dash
    {32767, 1, 32767, 1},
    {32767, 1, 32767, 0}, // dash
    {32767, 1, 32767, 0}  // dot
};


/*
 * Populate the RMT items array
 * with a previously prepared message
 */
static void populate_rmt_items(void)
{
    for (int i = 0; i < RMT_MESSAGE_LENGTH; i++) {
        items[i].duration0 = message[i][0];
        items[i].level0    = message[i][1];
        items[i].duration1 = message[i][2];
        items[i].level1    = message[i][3];
    }
}


/*
 * Initialize the RMT Tx channel
 */
static void rmt_tx_int()
{
    rmt_config_t config;
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = RMT_TX_GPIO;
    config.mem_block_num = 1;
    config.tx_config.loop_en = 0;
    // enable the carrier to be able to hear the Morse sound
    // if the RMT_TX_GPIO is connected to a speaker
    config.tx_config.carrier_en = 1;
    config.tx_config.idle_output_en = 1;
    config.tx_config.idle_level = 0;
    config.tx_config.carrier_duty_percent = 50;
    // set audible career frequency of 611 Hz
    // actually 611 Hz is the minimum, that can be set
    // with current implementation of the RMT API
    config.tx_config.carrier_freq_hz = 611;
    config.tx_config.carrier_level = 1;
    // set the maximum clock divider to be able to output
    // RMT pulses in range of about one hundred milliseconds
    config.clk_div = 255;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
}


/*
 * Transmit all the items in a loop
 */
static void rmt_tx_task(void *ignore)
{
    while (1) {
        ESP_ERROR_CHECK(rmt_write_items(RMT_TX_CHANNEL, items, RMT_MESSAGE_LENGTH, 1));
        ESP_LOGI(RMT_TX_TAG, "Transmission complete");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}


void app_main(void *ignore)
{
    ESP_LOGI(RMT_TX_TAG, "Configuring transmitter");
    rmt_tx_int();
    populate_rmt_items();

    ESP_LOGI(RMT_TX_TAG, "Spinning out transmit task");
    xTaskCreatePinnedToCore(&rmt_tx_task, "rmt_tx_task", 4 * 1024, NULL, 5, NULL, 0);
}
