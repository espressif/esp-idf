/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "usb_device_uac.h"
#include "esp_codec_dev_defaults.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_vol.h"
#include "i2s_usb_example.h"
#include "esp_log.h"

static const char *TAG = "i2s_usb";

static i2s_chan_handle_t i2s_tx_handle = NULL;
static i2s_chan_handle_t i2s_rx_handle = NULL;
static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static esp_codec_dev_handle_t codec_handle = NULL;
static uint32_t init_mclk_freq_hz = 0;

static void i2s_driver_init(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(EXAMPLE_I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true; // Auto clear the legacy data in the DMA buffer
    chan_cfg.dma_desc_num = EXAMPLE_I2S_DMA_DESC_NUM;
    chan_cfg.dma_frame_num = EXAMPLE_I2S_DMA_FRAME_NUM;
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &i2s_tx_handle, &i2s_rx_handle));
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_AUDIO_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(EXAMPLE_AUDIO_BIT_WIDTH, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = EXAMPLE_I2S_MCK_IO,
            .bclk = EXAMPLE_I2S_BCK_IO,
            .ws = EXAMPLE_I2S_WS_IO,
            .dout = EXAMPLE_I2S_DO_IO,
            .din = EXAMPLE_I2S_DI_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    std_cfg.clk_cfg.mclk_multiple = EXAMPLE_CODEC_MCLK_MULTIPLE;

    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_tx_handle, &std_cfg));

    ESP_ERROR_CHECK(i2s_channel_enable(i2s_rx_handle));
    ESP_ERROR_CHECK(i2s_channel_enable(i2s_tx_handle));

    /* Get the initial MCLK */
    i2s_tuning_info_t tune_info = {};
    ESP_ERROR_CHECK(i2s_channel_tune_rate(i2s_tx_handle, NULL, &tune_info));
    init_mclk_freq_hz = tune_info.curr_mclk_hz;
    printf("initial MCLK: %"PRIu32" Hz\n", init_mclk_freq_hz);

#if EXAMPLE_DYNAMIC_TUNING_EN
    /* Force to deviate the MCLK to demonstrate how the frequency tuning works.
     * Please REMOVE the following code in your actual project!! */
    i2s_tuning_config_t tune_cfg = {
        .tune_mode = I2S_TUNING_MODE_ADDSUB,
        .tune_mclk_val = -800,
        .max_delta_mclk = 800,
        .min_delta_mclk = -800,
    };
    ESP_ERROR_CHECK(i2s_channel_tune_rate(i2s_tx_handle, &tune_cfg, &tune_info));
#endif  // EXAMPLE_DYNAMIC_TUNING_EN
}

static void i2c_driver_init(void)
{
    /* Initialize I2C peripheral */
    i2c_master_bus_config_t i2c_mst_cfg = {
        .i2c_port = EXAMPLE_I2C_NUM,
        .sda_io_num = EXAMPLE_I2C_SDA_IO,
        .scl_io_num = EXAMPLE_I2C_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        /* Pull-up internally for no external pull-up case.
           Suggest to use external pull-up to ensure a strong enough pull-up. */
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_cfg, &i2c_bus_handle));
}

static void esp_codec_es8311_init(void)
{
    /* Create control interface with I2C bus handle */
    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = EXAMPLE_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_bus_handle,
    };
    const audio_codec_ctrl_if_t *ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    assert(ctrl_if);

    /* Create data interface with I2S bus handle */
    audio_codec_i2s_cfg_t i2s_cfg = {
        .port = EXAMPLE_I2S_NUM,
        .rx_handle = i2s_rx_handle,
        .tx_handle = i2s_tx_handle,
    };
    const audio_codec_data_if_t *data_if = audio_codec_new_i2s_data(&i2s_cfg);
    assert(data_if);

    /* Create ES8311 interface handle */
    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    assert(gpio_if);
    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,
        .master_mode = false,
        .use_mclk = true,
        .pa_pin = EXAMPLE_PA_CTRL_IO,
        .pa_reverted = false,
        .hw_gain = {
            .pa_voltage = 5.0,
            .codec_dac_voltage = 3.3,
        },
        .mclk_div = EXAMPLE_CODEC_MCLK_MULTIPLE,
    };
    const audio_codec_if_t *es8311_if = es8311_codec_new(&es8311_cfg);
    assert(es8311_if);

    /* Create the top codec handle with ES8311 interface handle and data interface */
    esp_codec_dev_cfg_t dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN_OUT,
        .codec_if = es8311_if,
        .data_if = data_if,
    };
    codec_handle = esp_codec_dev_new(&dev_cfg);
    assert(codec_handle);

    /* Specify the sample configurations and open the device */
    esp_codec_dev_sample_info_t sample_cfg = {
        .bits_per_sample = EXAMPLE_AUDIO_BIT_WIDTH,
        .channel = 1,
        .channel_mask = 0x01,
        .sample_rate = EXAMPLE_AUDIO_SAMPLE_RATE,
    };
    esp_codec_dev_open(codec_handle, &sample_cfg);

    /* Set the initial volume and gain */
    esp_codec_dev_set_out_vol(codec_handle, EXAMPLE_CODEC_VOLUME);
    esp_codec_dev_set_in_gain(codec_handle, EXAMPLE_CODEC_MIC_GAIN);
}

static esp_err_t usb_uac_device_output_cb(uint8_t *buf, size_t len, void *arg)
{
    int ret = esp_codec_dev_write(codec_handle, buf, len);

#if EXAMPLE_DYNAMIC_TUNING_EN
    static uint32_t overflow_cnt = 0;
    static uint32_t underflow_cnt = 0;
    static uint32_t normal_cnt = 0;
    /* Get the current water mark */
    i2s_tuning_info_t tune_info = {};
    ESP_ERROR_CHECK(i2s_channel_tune_rate(i2s_tx_handle, NULL, &tune_info));
    /* If the water mark exceed the threshould for several times,
     * it indicates I2S is slower than UAC, increase MCLK to match the UAC speed */
    if (tune_info.water_mark >= 90) {
        overflow_cnt++;
        /* In case of tuning the MCLK frequently, we need to monitor the long term trend,
         * so we'd better to tune the frequency after several seconds since the first overflow happen.
         * Take 5 sec for example, then the count can be calculated as follow:
         * count = slot_bytes(2) * slot_num(1) * sample_rate(16000) * seconds(5) / bytes_per_written(32) = 5000 */
        if (overflow_cnt >= 5000) {
            // No spare DMA buffer, i.e., too many data pending to be sent, increase MCLK to match the UAC speed
            i2s_tuning_config_t tune_cfg = EXAMPLE_I2S_DEFAULT_TUNING_CFG(init_mclk_freq_hz, true);
            /* Suggest to disable the I2S first before tuning the rate,
             * otherwise plosive sounds might break out if we change the rate while the audio playing */
            ESP_ERROR_CHECK(i2s_channel_disable(i2s_tx_handle));
            ESP_ERROR_CHECK(i2s_channel_tune_rate(i2s_tx_handle, &tune_cfg, &tune_info));
            ESP_ERROR_CHECK(i2s_channel_enable(i2s_tx_handle));
            printf("overflow detected, current MCLK: %"PRIu32" Hz, %"PRId32" Hz changed\n", tune_info.curr_mclk_hz, tune_info.delta_mclk_hz);
            overflow_cnt = 0;
            normal_cnt = 0;
        }
    }
    /* If the water mark is below the threshould for several times,
     * it indicates I2S is faster than UAC, decrease MCLK to match the UAC speed */
    else if (tune_info.water_mark < 10) {
        underflow_cnt++;
        if (underflow_cnt >= 5000) {
            // Almost all data were sent, decrease MCLK to match the UAC speed
            i2s_tuning_config_t tune_cfg = EXAMPLE_I2S_DEFAULT_TUNING_CFG(init_mclk_freq_hz, false);
            ESP_ERROR_CHECK(i2s_channel_disable(i2s_tx_handle));
            ESP_ERROR_CHECK(i2s_channel_tune_rate(i2s_tx_handle, &tune_cfg, &tune_info));
            ESP_ERROR_CHECK(i2s_channel_enable(i2s_tx_handle));
            printf("underflow detected, current MCLK: %"PRIu32" Hz, %"PRId32" Hz changed\n", tune_info.curr_mclk_hz, tune_info.delta_mclk_hz);
            underflow_cnt = 0;
            normal_cnt = 0;
        }
    }
    /* If the water mark is within the normal range for a while, clear the counter */
    else {
        normal_cnt %= 1000;
        normal_cnt++;
        if (normal_cnt >= 1000) {
            overflow_cnt = 0;
            underflow_cnt = 0;
        }
    }
#endif  // EXAMPLE_DYNAMIC_TUNING_EN
    if (ret != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "i2s write failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t usb_uac_device_input_cb(uint8_t *buf, size_t len, size_t *bytes_read, void *arg)
{
    int ret = esp_codec_dev_read(codec_handle, buf, len);
    /* Since TX and RX are duplex, we only need to tune one of them.
     * And the MCLK is generated by the latter initialized channel,
     * in this example, TX is initialized after RX, so the MCLK comes from the TX channel.
     * Therefore, we only need to tune the TX channel.
     * You can also tune both channels if you can't tell which one is initialized first,
     * but only the channel that generates MCLK can be actually tuned */
    if (ret != ESP_CODEC_DEV_OK) {
        ESP_LOGE(TAG, "i2s read failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static void usb_uac_device_set_mute_cb(uint32_t mute, void *arg)
{
    esp_codec_dev_set_out_mute(codec_handle, !!mute);
    ESP_LOGI(TAG, "uac-device %s muted", mute ? "is" : "cancel");
}

static void usb_uac_device_set_volume_cb(uint32_t volume, void *arg)
{
    esp_codec_dev_set_out_vol(codec_handle, volume);
    ESP_LOGI(TAG, "set uac-device volume to: %"PRIu32, volume);
}

static void usb_uac_device_init(void)
{
    uac_device_config_t config = {
        .output_cb = usb_uac_device_output_cb,
        .input_cb = usb_uac_device_input_cb,
        .set_mute_cb = usb_uac_device_set_mute_cb,
        .set_volume_cb = usb_uac_device_set_volume_cb,
        .cb_ctx = NULL,
    };
    /* Init UAC device, UAC related configurations can be set by the menuconfig */
    ESP_ERROR_CHECK(uac_device_init(&config));
}

void app_main(void)
{
    /* Initialize I2S peripheral */
    i2s_driver_init();
    /* Initialize I2C peripheral */
    i2c_driver_init();
    /* Initialize ES8311 codec */
    esp_codec_es8311_init();
    /* Initialize the USB as UAC device */
    usb_uac_device_init();

    /* Print the MCLK and buffer info */
    i2s_tuning_info_t tune_info = {};
    while (1) {
        ESP_ERROR_CHECK(i2s_channel_tune_rate(i2s_tx_handle, NULL, &tune_info));
        ESP_LOGI(TAG, "MCLK: %"PRId32", delta: %"PRId32", buffer water mark: %"PRIu32"%%",
                 tune_info.curr_mclk_hz, tune_info.delta_mclk_hz, tune_info.water_mark);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
