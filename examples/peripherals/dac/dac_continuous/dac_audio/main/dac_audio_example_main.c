/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/dac_continuous.h"
#include "esp_check.h"
#include "audio_example_file.h"

static const char *TAG = "dac_audio";

#if CONFIG_EXAMPLE_DAC_WRITE_ASYNC
static bool IRAM_ATTR  dac_on_convert_done_callback(dac_continuous_handle_t handle, const dac_event_data_t *event, void *user_data)
{
    QueueHandle_t que = (QueueHandle_t)user_data;
    BaseType_t need_awoke;
    /* When the queue is full, drop the oldest item */
    if (xQueueIsQueueFullFromISR(que)) {
        dac_event_data_t dummy;
        xQueueReceiveFromISR(que, &dummy, &need_awoke);
    }
    /* Send the event from callback */
    xQueueSendFromISR(que, event, &need_awoke);
    return need_awoke;
}

static void dac_write_data_asynchronously(dac_continuous_handle_t handle, QueueHandle_t que, uint8_t *data, size_t data_size)
{
    ESP_LOGI(TAG, "Audio size %d bytes, played at frequency %d Hz asynchronously", data_size, CONFIG_EXAMPLE_AUDIO_SAMPLE_RATE);
    uint32_t cnt = 1;
    while (1) {
        printf("Play count: %"PRIu32"\n", cnt++);
        dac_event_data_t evt_data;
        size_t byte_written = 0;
        /* Receive the event from callback and load the data into the DMA buffer until the whole audio loaded */
        while (byte_written < data_size) {
            xQueueReceive(que, &evt_data, portMAX_DELAY);
            size_t loaded_bytes = 0;
            ESP_ERROR_CHECK(dac_continuous_write_asynchronously(handle, evt_data.buf, evt_data.buf_size,
                                                                data + byte_written, data_size - byte_written, &loaded_bytes));
            byte_written += loaded_bytes;
        }
        /* Clear the legacy data in DMA, clear times equal to the 'dac_continuous_config_t::desc_num' */
        for (int i = 0; i < 4; i++) {
            xQueueReceive(que, &evt_data, portMAX_DELAY);
            memset(evt_data.buf, 0, evt_data.buf_size);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#else
static void dac_write_data_synchronously(dac_continuous_handle_t handle, uint8_t *data, size_t data_size)
{
    ESP_LOGI(TAG, "Audio size %d bytes, played at frequency %d Hz synchronously", data_size, CONFIG_EXAMPLE_AUDIO_SAMPLE_RATE);
    uint32_t cnt = 1;
    while (1) {
        printf("Play count: %"PRIu32"\n", cnt++);
        ESP_ERROR_CHECK(dac_continuous_write(handle, data, data_size, NULL, -1));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif

void app_main(void)
{
    ESP_LOGI(TAG, "DAC audio example start");
    ESP_LOGI(TAG, "--------------------------------------");

    dac_continuous_handle_t dac_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 4,
        .buf_size = 2048,
        .freq_hz = CONFIG_EXAMPLE_AUDIO_SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,   // Using APLL as clock source to get a wider frequency range
        /* Assume the data in buffer is 'A B C D E F'
         * DAC_CHANNEL_MODE_SIMUL:
         *      - channel 0: A B C D E F
         *      - channel 1: A B C D E F
         * DAC_CHANNEL_MODE_ALTER:
         *      - channel 0: A C E
         *      - channel 1: B D F
         */
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    /* Allocate continuous channels */
    ESP_ERROR_CHECK(dac_continuous_new_channels(&cont_cfg, &dac_handle));
#if CONFIG_EXAMPLE_DAC_WRITE_ASYNC
    /* Create a queue to transport the interrupt event data */
    QueueHandle_t que = xQueueCreate(10, sizeof(dac_event_data_t));
    assert(que);
    dac_event_callbacks_t cbs = {
        .on_convert_done = dac_on_convert_done_callback,
        .on_stop = NULL,
    };
    /* Must register the callback if using asynchronous writing */
    ESP_ERROR_CHECK(dac_continuous_register_event_callback(dac_handle, &cbs, que));
#endif
    /* Enable the continuous channels */
    ESP_ERROR_CHECK(dac_continuous_enable(dac_handle));
    ESP_LOGI(TAG, "DAC initialized success, DAC DMA is ready");

    size_t audio_size = sizeof(audio_table);
#if CONFIG_EXAMPLE_DAC_WRITE_ASYNC
    ESP_ERROR_CHECK(dac_continuous_start_async_writing(dac_handle));
    dac_write_data_asynchronously(dac_handle, que, (uint8_t *)audio_table, audio_size);
#else
    dac_write_data_synchronously(dac_handle, (uint8_t *)audio_table, audio_size);
#endif
}
