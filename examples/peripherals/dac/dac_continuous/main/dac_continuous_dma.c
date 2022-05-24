/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "dac_continuous.h"

#define EXAMPLE_WAVE_FREQ_HZ        2000                      // Default wave frequency 50 Hz, it can't be too high
#define EXAMPLE_CONVERT_FREQ_HZ     (EXAMPLE_ARRAY_LEN * EXAMPLE_WAVE_FREQ_HZ) // The frequency that DAC convert every data in the wave array

static uint8_t sin_wav[EXAMPLE_ARRAY_LEN];                      // Used to store sine wave values
static uint8_t tri_wav[EXAMPLE_ARRAY_LEN];                      // Used to store triangle wave values
static uint8_t saw_wav[EXAMPLE_ARRAY_LEN];                      // Used to store sawtooth wave values
static uint8_t squ_wav[EXAMPLE_ARRAY_LEN];                      // Used to store square wave values

static const char *TAG = "dac continuous(DMA)";
static const char wav_name[DAC_WAVE_MAX][15] = {"sine", "triangle", "sawtooth", "square"};

static void dac_dma_write_task(void *args)
{
    dac_channels_handle_t handle = (dac_channels_handle_t)args;
    dac_example_wave_type_t wav_sel = DAC_SINE_WAVE; // Start from sine wave

    size_t buf_len = EXAMPLE_ARRAY_LEN;

    while (1) {
        /* The wave in the buffer will be converted cyclicly
         * but take care the data buffer need to be available during the conversion */
        switch (wav_sel) {
            case DAC_SINE_WAVE:
                ESP_ERROR_CHECK(dac_channels_write_cyclically(handle, (uint8_t *)sin_wav, buf_len, NULL, 1000));
                break;
            case DAC_TRIANGLE_WAVE:
                ESP_ERROR_CHECK(dac_channels_write_cyclically(handle, (uint8_t *)tri_wav, buf_len, NULL, 1000));
                break;
            case DAC_SAWTOOTH_WAVE:
                ESP_ERROR_CHECK(dac_channels_write_cyclically(handle, (uint8_t *)saw_wav, buf_len, NULL, 1000));
                break;
            case DAC_SQUARE_WAVE:
                ESP_ERROR_CHECK(dac_channels_write_cyclically(handle, (uint8_t *)squ_wav, buf_len, NULL, 1000));
                break;
            default:
                break;
        }
        /* Switch wave every EXAMPLE_WAVE_PERIOD_SEC seconds */
        vTaskDelay(pdMS_TO_TICKS(EXAMPLE_WAVE_PERIOD_SEC * 1000));
        wav_sel++;
        wav_sel %= DAC_WAVE_MAX;
        ESP_LOGI(TAG, "%s wave start", wav_name[wav_sel]);
    }
}

static void dac_init_channel(dac_channel_mask_t mask, dac_conti_config_t *conti_cfg, dac_channels_handle_t *dac_handle)
{
    dac_channels_handle_t handle = NULL;
    dac_channels_config_t cfg = {
        .chan_sel = mask,
    };
    /* Allocate the channel group */
    ESP_ERROR_CHECK(dac_new_channels(&cfg, &handle));
    /* Enable the channels in the group */
    ESP_ERROR_CHECK(dac_channels_enable(handle));
    /* Initialize DAC DMA peripheral */
    ESP_ERROR_CHECK(dac_channels_init_continuous_mode(handle, conti_cfg));
    /* Start the DAC DMA peripheral */
    ESP_ERROR_CHECK(dac_channels_enable_continuous_mode(handle));

    *dac_handle = handle;
}

/* Unused DAC de-initialize example, to show how to delete the DAC resources */
static void __attribute__((unused)) dac_deinit_channel(dac_channels_handle_t dac_handle)
{
    /* Stop the DAC DMA peripheral */
    ESP_ERROR_CHECK(dac_channels_disable_continuous_mode(dac_handle));
    /* Deinitialize the DAC DMA peripheral */
    ESP_ERROR_CHECK(dac_channels_deinit_continuous_mode(dac_handle));
    /* Disable the DAC channels */
    ESP_ERROR_CHECK(dac_channels_disable(dac_handle));
    /* Delete the channel group */
    ESP_ERROR_CHECK(dac_del_channels(dac_handle));
}

static void example_generate_wave(void)
{
    uint32_t pnt_num = EXAMPLE_ARRAY_LEN;

    for (int i = 0; i < pnt_num; i ++) {
        sin_wav[i] = (uint8_t)((sin( i * CONST_PERIOD_2_PI / pnt_num) + 1) * (double)(EXAMPLE_DAC_AMPLITUDE) / 2 + 0.5);
        tri_wav[i] = (i > (pnt_num / 2)) ? (2 * EXAMPLE_DAC_AMPLITUDE * (pnt_num - i) / pnt_num) : (2 * EXAMPLE_DAC_AMPLITUDE * i / pnt_num);
        saw_wav[i] = (i == pnt_num) ? 0 : (i * EXAMPLE_DAC_AMPLITUDE / pnt_num);
        squ_wav[i] = (i < (pnt_num / 2)) ? EXAMPLE_DAC_AMPLITUDE : 0;
    }
}

void dac_continuous_by_dma(void)
{
    dac_channels_handle_t chan12_handle;
    dac_conti_config_t conti_cfg = {
        .freq_hz = EXAMPLE_CONVERT_FREQ_HZ,
        /* Assume the data in buffer is 'A B C D E F'
         * DAC_CHANNEL_MODE_SIMUL:
         *      - channel 0: A B C D E F
         *      - channel 1: A B C D E F
         * DAC_CHANNEL_MODE_ALTER:
         *      - channel 0: A C E
         *      - channel 1: B D F
         */
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,     // If the frequency is out of range, try 'DAC_DIGI_CLK_SRC_APLL'
        .desc_num = 10,
    };
    /* For Continuous(DMA) Mode, only one group can access the DMA periphral,
     * which means the two channels can't be initialized to DMA mode separately */
    dac_init_channel(DAC_CHANNEL_MASK_BOTH, &conti_cfg, &chan12_handle);

    example_log_info(EXAMPLE_CONVERT_FREQ_HZ, EXAMPLE_WAVE_FREQ_HZ);

    /* Generate the data buffer, the data is a sawtooth wave every 256 point,
     * With the the data frequency at 20 KHz, the sawtooth wave frequency is about 20 KHz / 256 = 78.125 Hz */
    example_generate_wave();

    /* Start to convert wave */
    xTaskCreate(dac_dma_write_task, "dac_dma_write_task", 4096, chan12_handle, 5, NULL);
}
