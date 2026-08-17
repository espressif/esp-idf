/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/base64.h"

#include "driver/i2s_pdm.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"

#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"

static const char *TAG = "mic_rec_example";

/* Both microphone paths produce this PCM format. */
#define EXAMPLE_SAMPLE_RATE        16000
#define EXAMPLE_CHANNEL_COUNT      2
#define EXAMPLE_BITS_PER_SAMPLE    16

#define EXAMPLE_RECORD_TIME_SECONDS  2
#define EXAMPLE_PCM_SIZE             (EXAMPLE_SAMPLE_RATE * EXAMPLE_RECORD_TIME_SECONDS * EXAMPLE_CHANNEL_COUNT * EXAMPLE_BITS_PER_SAMPLE / 8)

#define EXAMPLE_DMA_READ_SIZE        1024
#define EXAMPLE_I2C_CLOCK_HZ         100000
#define EXAMPLE_MCLK_MULTIPLE        256  /* Codec MCLK is 256 times the sample rate. */
#define EXAMPLE_STEREO_CHANNEL_MASK  0x03 /* Capture both left and right channels. */

/* A multiple of three produces fixed-size Base64 lines without padding. */
#define EXAMPLE_BASE64_CHUNK_BYTES   72
#define EXAMPLE_BASE64_BUFFER_SIZE   (4 * ((EXAMPLE_BASE64_CHUNK_BYTES + 2) / 3) + 1)

static void capture_pcm(i2s_chan_handle_t rx_handle, uint8_t *pcm_data, size_t pcm_size)
{
    size_t bytes_recorded = 0;

    /* Continue until the RAM buffer contains the requested recording duration. */
    while (bytes_recorded < pcm_size) {
        size_t bytes_to_read = pcm_size - bytes_recorded;
        bytes_to_read = MIN(bytes_to_read, EXAMPLE_DMA_READ_SIZE);
        size_t bytes_read = 0;
        ESP_ERROR_CHECK(i2s_channel_read(rx_handle, pcm_data + bytes_recorded, bytes_to_read, &bytes_read, 1000));
        bytes_recorded += bytes_read;
    }
}

static void discard_startup_pcm(i2s_chan_handle_t rx_handle)
{
    const size_t bytes_to_discard = (size_t)EXAMPLE_SAMPLE_RATE * EXAMPLE_CHANNEL_COUNT *
                                    (EXAMPLE_BITS_PER_SAMPLE / 8) * CONFIG_EXAMPLE_STARTUP_DISCARD_MS / 1000;
    if (bytes_to_discard == 0) {
        return;
    }

    uint8_t discard_buf[EXAMPLE_DMA_READ_SIZE];
    size_t bytes_discarded = 0;

    ESP_LOGI(TAG, "Discarding the first %d ms of startup data", CONFIG_EXAMPLE_STARTUP_DISCARD_MS);
    while (bytes_discarded < bytes_to_discard) {
        size_t bytes_to_read = bytes_to_discard - bytes_discarded;
        bytes_to_read = MIN(bytes_to_read, EXAMPLE_DMA_READ_SIZE);
        size_t bytes_read = 0;
        ESP_ERROR_CHECK(i2s_channel_read(rx_handle, discard_buf, bytes_to_read, &bytes_read, 1000));
        bytes_discarded += bytes_read;
    }
}

static void send_pcm_as_base64(const uint8_t *pcm_data, size_t pcm_size)
{
    unsigned char encoded_chunk[EXAMPLE_BASE64_BUFFER_SIZE];
    size_t chunks_sent = 0;

    /* Framed chunks let the host extract audio while ignoring other console logs. */
    printf("AUDIO_BASE64_BEGIN\n");
    for (size_t offset = 0; offset < pcm_size; offset += EXAMPLE_BASE64_CHUNK_BYTES) {
        size_t chunk_size = pcm_size - offset;
        chunk_size = MIN(chunk_size, EXAMPLE_BASE64_CHUNK_BYTES);
        size_t encoded_size = 0;
        ESP_ERROR_CHECK(mbedtls_base64_encode(encoded_chunk, sizeof(encoded_chunk), &encoded_size,
                                              pcm_data + offset, chunk_size) == 0 ? ESP_OK : ESP_FAIL);
        printf("AUDIO_BASE64 %.*s\n", (int)encoded_size, encoded_chunk);
        if (++chunks_sent % 16 == 0) {
            /* UART output is slow; allow the idle task to run periodically. */
            vTaskDelay(1);
        }
    }
    printf("AUDIO_BASE64_END\n");
}

#if CONFIG_EXAMPLE_MIC_TYPE_DMIC
static void record_from_pdm_microphone(uint8_t *pcm_data, size_t pcm_size)
{
    /* PDM RX mode drives the microphone clock and converts its PDM stream to PCM. */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_pdm_rx_config_t pdm_rx_cfg = {
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        /* In stereo mode, the left slot (select pin pulled down) and the right slot (select pin
         * pulled up) are both sampled on the shared data pin, producing interleaved L/R channels. */
        .slot_cfg = I2S_PDM_RX_SLOT_PCM_FMT_DEFAULT_CONFIG(EXAMPLE_BITS_PER_SAMPLE, EXAMPLE_CHANNEL_COUNT),
        .gpio_cfg = {
            .clk = CONFIG_EXAMPLE_PDM_CLK_GPIO,
            .dins[0] = CONFIG_EXAMPLE_PDM_DATA_GPIO,
        },
    };

    i2s_chan_handle_t rx_handle = NULL;
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    ESP_ERROR_CHECK(i2s_channel_init_pdm_rx_mode(rx_handle, &pdm_rx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));

    /* Discard the startup PCM to skip the microphone startup transient. */
    discard_startup_pcm(rx_handle);
    ESP_LOGI(TAG, "Starting PDM recording for %d seconds!", EXAMPLE_RECORD_TIME_SECONDS);
    capture_pcm(rx_handle, pcm_data, pcm_size);

    ESP_ERROR_CHECK(i2s_channel_disable(rx_handle));
    ESP_ERROR_CHECK(i2s_del_channel(rx_handle));
}
#endif

#if CONFIG_EXAMPLE_MIC_TYPE_AMIC
static void record_from_es8389_microphone(uint8_t *pcm_data, size_t pcm_size)
{
    /* The analog signal is sampled by the ES8389 ADC. I2S RX receives its digital samples
     * and, as bus master, generates the BCLK/WS (and optional MCLK) required by the codec. */
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    i2c_master_bus_config_t i2c_bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1, /* Let the driver select a free I2C port */
        .sda_io_num = CONFIG_EXAMPLE_I2C_SDA_IO,
        .scl_io_num = CONFIG_EXAMPLE_I2C_SCL_IO,
        .glitch_ignore_cnt = 7,
        /* Enable the internal pull-ups in case there is no external pull-up on the board.
         * External pull-ups are recommended to make the bus more robust. */
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_handle));

    /* I2C configures codec registers; PCM samples use I2S. */
    audio_codec_i2c_cfg_t i2c_cfg = {
        .bus_handle = i2c_bus_handle,
        .addr = ES8389_CODEC_DEFAULT_ADDR,
        .clock_speed_hz = EXAMPLE_I2C_CLOCK_HZ,
    };
    const audio_codec_ctrl_if_t *ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    ESP_ERROR_CHECK(ctrl_if ? ESP_OK : ESP_FAIL);

    es8389_codec_cfg_t es8389_cfg = {
        .ctrl_if = ctrl_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_ADC,
        .pa_pin = GPIO_NUM_NC,
        /* When MCLK is unused, the ES8389 derives its clock from BCLK. */
        .use_mclk = CONFIG_EXAMPLE_I2S_MCLK_IO >= 0,
        .mclk_div = EXAMPLE_MCLK_MULTIPLE,
    };
    const audio_codec_if_t *codec_if = es8389_codec_new(&es8389_cfg);
    ESP_ERROR_CHECK(codec_if ? ESP_OK : ESP_FAIL);

    /* Recording needs only an RX channel. In master mode, RX also drives BCLK/WS. */
    i2s_chan_handle_t rx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(EXAMPLE_BITS_PER_SAMPLE, EXAMPLE_CHANNEL_COUNT),
        .gpio_cfg = {
            .mclk = CONFIG_EXAMPLE_I2S_MCLK_IO,
            .bclk = CONFIG_EXAMPLE_I2S_BCLK_IO,
            .ws = CONFIG_EXAMPLE_I2S_WS_IO,
            .dout = GPIO_NUM_NC,
            .din = CONFIG_EXAMPLE_I2S_DIN_IO,
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));

    /* Pass the already-configured RX channel to the codec data interface. */
    audio_codec_i2s_cfg_t i2s_cfg = {
        .rx_handle = rx_handle,
    };
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);
    ESP_ERROR_CHECK(data_if ? ESP_OK : ESP_FAIL);

    esp_codec_dev_cfg_t dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN,
        .codec_if = codec_if,
        .data_if = data_if,
    };
    esp_codec_dev_handle_t codec_handle = esp_codec_dev_new(&dev_cfg);
    ESP_ERROR_CHECK(codec_handle ? ESP_OK : ESP_FAIL);

    esp_codec_dev_sample_info_t sample_cfg = {
        .bits_per_sample = EXAMPLE_BITS_PER_SAMPLE,
        .channel = EXAMPLE_CHANNEL_COUNT,
        .channel_mask = EXAMPLE_STEREO_CHANNEL_MASK,
        .sample_rate = EXAMPLE_SAMPLE_RATE,
        .mclk_multiple = EXAMPLE_MCLK_MULTIPLE,
    };
    ESP_ERROR_CHECK(esp_codec_dev_open(codec_handle, &sample_cfg));
    ESP_ERROR_CHECK(esp_codec_dev_set_in_gain(codec_handle, CONFIG_EXAMPLE_MIC_GAIN));
    ESP_LOGI(TAG, "ES8389 codec initialized");

    /* Discard the startup PCM to skip the microphone or codec startup transient. */
    discard_startup_pcm(rx_handle);
    ESP_LOGI(TAG, "Starting ES8389 recording for %d seconds!", EXAMPLE_RECORD_TIME_SECONDS);
    capture_pcm(rx_handle, pcm_data, pcm_size);

    /* Closing the input device disables RX before its channel is deleted. */
    ESP_ERROR_CHECK(esp_codec_dev_close(codec_handle));
    esp_codec_dev_delete(codec_handle);
    ESP_ERROR_CHECK(audio_codec_delete_codec_if(codec_if));
    ESP_ERROR_CHECK(audio_codec_delete_data_if(data_if));
    ESP_ERROR_CHECK(audio_codec_delete_ctrl_if(ctrl_if));
    ESP_ERROR_CHECK(i2s_del_channel(rx_handle));
    ESP_ERROR_CHECK(i2c_del_master_bus(i2c_bus_handle));
}
#endif

void app_main(void)
{
    /* Capture to RAM first so slow UART output cannot overflow the I2S DMA buffers. */
    uint8_t *pcm_data = calloc(1, EXAMPLE_PCM_SIZE);
    ESP_ERROR_CHECK(pcm_data ? ESP_OK : ESP_FAIL);

#if CONFIG_EXAMPLE_MIC_TYPE_DMIC
    printf("PDM MIC recording example start\n--------------------------------------\n");
    record_from_pdm_microphone(pcm_data, EXAMPLE_PCM_SIZE);
#else
    printf("ES8389 MIC recording example start\n--------------------------------------------\n");
    record_from_es8389_microphone(pcm_data, EXAMPLE_PCM_SIZE);
#endif

    ESP_LOGI(TAG, "Recording done, sending PCM data over console");
    printf("AUDIO_META sample_rate=%d bits_per_sample=%d channels=%d data_size=%d encoding=base64\n",
           EXAMPLE_SAMPLE_RATE, EXAMPLE_BITS_PER_SAMPLE, EXAMPLE_CHANNEL_COUNT, EXAMPLE_PCM_SIZE);
    send_pcm_as_base64(pcm_data, EXAMPLE_PCM_SIZE);
    printf("mic recorder example done\n");
    free(pcm_data);
}
