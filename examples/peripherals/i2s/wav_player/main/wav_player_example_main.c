/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"

#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"

static const char *TAG = "wav_player";

/* These four values are the I2S slot format AND the WAV / codec format. They must match. */
#define EXAMPLE_SAMPLE_RATE          16000
#define EXAMPLE_BITS_PER_SAMPLE      16
#define EXAMPLE_CHANNEL_COUNT        2
#define EXAMPLE_STEREO_CHANNEL_MASK  0x03 /* Bit0 = left, bit1 = right. */

/* Most codecs expect MCLK = 256 * LRCK (sample rate) when an external MCLK pin is wired. */
#define EXAMPLE_MCLK_MULTIPLE        256
#define EXAMPLE_I2C_CLOCK_HZ         100000

#if CONFIG_EXAMPLE_DAC_TYPE_ES8311
#define EXAMPLE_CODEC_I2C_ADDR     ES8311_CODEC_DEFAULT_ADDR
#define EXAMPLE_CODEC_NAME         "ES8311"
#else
#define EXAMPLE_CODEC_I2C_ADDR     ES8389_CODEC_DEFAULT_ADDR
#define EXAMPLE_CODEC_NAME         "ES8389"
#endif

/* CMake embeds the WAV in flash as a binary blob (target_add_binary_data). */
extern const uint8_t music_wav_start[] asm("_binary_music_wav_start");
extern const uint8_t music_wav_end[]   asm("_binary_music_wav_end");

static bool fourcc_eq(const uint8_t *p, const char *id)
{
    return memcmp(p, id, 4) == 0;
}

static uint16_t read_le16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

/* I2S transmits raw PCM only. A WAV file is a RIFF container: skip "fmt " / "data"
 * chunks until the little-endian PCM payload, and reject compressed encodings. */
static esp_err_t wav_get_pcm(const uint8_t *wav, size_t wav_len, const uint8_t **pcm, size_t *pcm_len)
{
    if (wav_len < 12 || !fourcc_eq(wav, "RIFF") || !fourcc_eq(wav + 8, "WAVE")) {
        ESP_LOGE(TAG, "not a RIFF/WAVE file");
        return ESP_ERR_INVALID_ARG;
    }

    const uint8_t *fmt = NULL;
    uint32_t fmt_size = 0;
    const uint8_t *data = NULL;
    uint32_t data_size = 0;

    for (size_t off = 12; off + 8 <= wav_len;) {
        uint32_t size = read_le32(wav + off + 4);
        size_t payload = off + 8;
        if (size > wav_len - payload) {
            ESP_LOGE(TAG, "WAV chunk overruns file");
            return ESP_ERR_INVALID_SIZE;
        }
        if (fourcc_eq(wav + off, "fmt ")) {
            fmt = wav + payload;
            fmt_size = size;
        } else if (fourcc_eq(wav + off, "data")) {
            data = wav + payload;
            data_size = size;
            break;
        }
        off = payload + size + (size & 1U);
    }

    if (fmt == NULL || fmt_size < 16 || data == NULL || data_size == 0) {
        ESP_LOGE(TAG, "WAV missing fmt or data chunk");
        return ESP_ERR_NOT_FOUND;
    }

    uint16_t audio_format = read_le16(fmt);
    uint16_t channels = read_le16(fmt + 2);
    uint32_t sample_rate = read_le32(fmt + 4);
    uint16_t bits = read_le16(fmt + 14);
    if (audio_format != 1) {
        ESP_LOGE(TAG, "WAV is not uncompressed PCM (format %u)", audio_format);
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (channels != EXAMPLE_CHANNEL_COUNT ||
            sample_rate != EXAMPLE_SAMPLE_RATE ||
            bits != EXAMPLE_BITS_PER_SAMPLE) {
        ESP_LOGE(TAG, "WAV is %u Hz / %u-bit / %u ch, expected %u / %u / %u",
                 (unsigned)sample_rate, bits, channels,
                 EXAMPLE_SAMPLE_RATE, EXAMPLE_BITS_PER_SAMPLE, EXAMPLE_CHANNEL_COUNT);
        return ESP_ERR_INVALID_ARG;
    }

    *pcm = data;
    *pcm_len = data_size;
    ESP_LOGI(TAG, "WAV PCM payload %u bytes", (unsigned)data_size);
    return ESP_OK;
}

static const audio_codec_if_t *create_dac_codec_if(const audio_codec_ctrl_if_t *ctrl_if,
                                                   const audio_codec_gpio_if_t *gpio_if)
{
    /* The codec is the I2S slave: it receives BCLK/WS (and optional MCLK) from the SoC.
     * pa_pin is the board amplifier enable, not a pin on the codec die. */
#if CONFIG_EXAMPLE_DAC_TYPE_ES8311
    es8311_codec_cfg_t dac_cfg = {
        .ctrl_if = ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_DAC,
        .master_mode = false,
        .use_mclk = CONFIG_EXAMPLE_I2S_MCLK_IO >= 0,
        .pa_pin = CONFIG_EXAMPLE_PA_CTRL_IO,
        .hw_gain = {
            .pa_voltage = 5.0,
            .codec_dac_voltage = 3.3,
        },
        .mclk_div = EXAMPLE_MCLK_MULTIPLE,
    };
    return es8311_codec_new(&dac_cfg);
#else
    es8389_codec_cfg_t dac_cfg = {
        .ctrl_if = ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_DAC,
        .master_mode = false,
        .use_mclk = CONFIG_EXAMPLE_I2S_MCLK_IO >= 0,
        .pa_pin = CONFIG_EXAMPLE_PA_CTRL_IO,
        .pa_reverted = false,
        .hw_gain = {
            .pa_voltage = 5.0,
            .codec_dac_voltage = 3.3,
        },
        .mclk_div = EXAMPLE_MCLK_MULTIPLE,
    };
    return es8389_codec_new(&dac_cfg);
#endif
}

static i2s_chan_handle_t i2s_driver_init(void)
{
    /* Simplex TX: only a transmit channel. I2S_ROLE_MASTER means the SoC generates
     * BCLK and WS. The codec must be configured as I2S slave (see create_dac_codec_if).
     * auto_clear writes zeros if the DMA ring underruns, which avoids a stuck last sample. */
    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    /* Standard (Philips) I2S: WS low = left slot, WS high = right slot.
     * Each slot is EXAMPLE_BITS_PER_SAMPLE bits. Stereo 16-bit uses
     * BCLK = sample_rate * 32. MCLK, if used, is sample_rate * EXAMPLE_MCLK_MULTIPLE. */
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(EXAMPLE_BITS_PER_SAMPLE, EXAMPLE_CHANNEL_COUNT),
        .gpio_cfg = {
            .mclk = CONFIG_EXAMPLE_I2S_MCLK_IO,
            .bclk = CONFIG_EXAMPLE_I2S_BCLK_IO,
            .ws = CONFIG_EXAMPLE_I2S_WS_IO,
            .dout = CONFIG_EXAMPLE_I2S_DOUT_IO,
            .din = GPIO_NUM_NC,
        },
    };

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    /* Enable now so BCLK/WS (and MCLK) are already running when the codec PLL starts. */
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
    return tx_handle;
}

static void dac_codec_init(i2s_chan_handle_t tx_handle)
{
    /* I2C programs codec registers (volume, mute, clock source, PA). PCM is still
     * written with i2s_channel_write(); the handle is only handed to the codec
     * so esp_codec_dev_open() can match the slot format already set on I2S. */
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

    audio_codec_i2c_cfg_t i2c_cfg = {
        .bus_handle = i2c_bus_handle,
        .addr = EXAMPLE_CODEC_I2C_ADDR,
        .clock_speed_hz = EXAMPLE_I2C_CLOCK_HZ,
    };
    const audio_codec_ctrl_if_t *ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    ESP_ERROR_CHECK(ctrl_if ? ESP_OK : ESP_FAIL);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    ESP_ERROR_CHECK(gpio_if ? ESP_OK : ESP_FAIL);

    const audio_codec_if_t *codec_if = create_dac_codec_if(ctrl_if, gpio_if);
    ESP_ERROR_CHECK(codec_if ? ESP_OK : ESP_FAIL);

    audio_codec_i2s_cfg_t i2s_cfg = {
        .tx_handle = tx_handle,
    };
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);
    ESP_ERROR_CHECK(data_if ? ESP_OK : ESP_FAIL);

    esp_codec_dev_cfg_t dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_OUT,
        .codec_if = codec_if,
        .data_if = data_if,
    };
    esp_codec_dev_handle_t codec_dev = esp_codec_dev_new(&dev_cfg);
    ESP_ERROR_CHECK(codec_dev ? ESP_OK : ESP_FAIL);

    /* open() applies the same sample format to the codec (and keeps the I2S
     * channel in sync), then enables the DAC and the board PA. */
    esp_codec_dev_sample_info_t sample_cfg = {
        .bits_per_sample = EXAMPLE_BITS_PER_SAMPLE,
        .channel = EXAMPLE_CHANNEL_COUNT,
        .channel_mask = EXAMPLE_STEREO_CHANNEL_MASK,
        .sample_rate = EXAMPLE_SAMPLE_RATE,
        .mclk_multiple = EXAMPLE_MCLK_MULTIPLE,
    };
    ESP_ERROR_CHECK(esp_codec_dev_open(codec_dev, &sample_cfg));
    ESP_ERROR_CHECK(esp_codec_dev_set_out_vol(codec_dev, CONFIG_EXAMPLE_VOICE_VOLUME));
    ESP_LOGI(TAG, "%s codec init success", EXAMPLE_CODEC_NAME);
}

void app_main(void)
{
    printf("wav player example start\n--------------------------\n");

    const uint8_t *pcm = NULL;
    size_t pcm_len = 0;
    ESP_ERROR_CHECK(wav_get_pcm(music_wav_start, music_wav_end - music_wav_start, &pcm, &pcm_len));

    i2s_chan_handle_t tx_handle = i2s_driver_init();
    ESP_LOGI(TAG, "i2s driver init success");

    dac_codec_init(tx_handle);

    /* Disable TX, preload the DMA descriptors, then enable. Otherwise the first
     * frames on the wire are zeros or stale SRAM (a click at the start of playback). */
    size_t bytes_write = 0;
    const uint8_t *data_ptr = pcm;
    ESP_ERROR_CHECK(i2s_channel_disable(tx_handle));
    ESP_ERROR_CHECK(i2s_channel_preload_data(tx_handle, data_ptr, pcm_len, &bytes_write));
    data_ptr += bytes_write;
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));

    while (1) {
        size_t remain = (size_t)(pcm + pcm_len - data_ptr);
        while (remain > 0) {
            ESP_ERROR_CHECK(i2s_channel_write(tx_handle, data_ptr, remain, &bytes_write, portMAX_DELAY));
            if (bytes_write == 0) {
                ESP_LOGE(TAG, "i2s music play failed.");
                abort();
            }
            data_ptr += bytes_write;
            remain -= bytes_write;
        }
        ESP_LOGI(TAG, "i2s music played, %d bytes are written.", (int)pcm_len);
        data_ptr = pcm;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
