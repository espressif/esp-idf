/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/*  I2S Example
 *  This example code will output 100Hz sine wave and triangle wave to 2-channel of I2S driver
 *  Every 5 seconds, it will change bits_per_sample [16, 24, 32] for i2s data
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_controller.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include <math.h>


#define SAMPLE_RATE     (36000)
#define I2S_NUM         (0)
#define WAVE_FREQ_HZ    (100)
#define PI              (3.14159265)
#define I2S_BCK_IO      (GPIO_NUM_4)
#define I2S_WS_IO       (GPIO_NUM_5)
#define I2S_DO_IO       (GPIO_NUM_18)
#define I2S_DI_IO       (-1)

#define SAMPLE_PER_CYCLE (SAMPLE_RATE/WAVE_FREQ_HZ)

static const char *TAG = "i2s_example";
static i2s_chan_handle_t tx_handle = NULL;

static void setup_triangle_sine_waves(int bits)
{
    int *samples_data = malloc(((bits + 8) / 16) * SAMPLE_PER_CYCLE * 4);
    unsigned int i, sample_val;
    double sin_float, triangle_float, triangle_step = (double) pow(2, bits) / SAMPLE_PER_CYCLE;
    size_t i2s_bytes_write = 0;
    static i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    static i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);

    printf("\r\nTest bits=%d free mem=%d, written data=%d\n", bits, esp_get_free_heap_size(), ((bits + 8) / 16)*SAMPLE_PER_CYCLE * 4);

    triangle_float = -(pow(2, bits) / 2 - 1);

    for (i = 0; i < SAMPLE_PER_CYCLE; i++) {
        sin_float = sin(i * 2 * PI / SAMPLE_PER_CYCLE);
        if (sin_float >= 0) {
            triangle_float += triangle_step;
        } else {
            triangle_float -= triangle_step;
        }

        sin_float *= (pow(2, bits) / 2 - 1);

        if (bits == 16) {
            sample_val = 0;
            sample_val += (short)triangle_float;
            sample_val = sample_val << 16;
            sample_val += (short) sin_float;
            samples_data[i] = sample_val;
        } else if (bits == 24) { //1-bytes unused
            samples_data[i * 2] = ((int) triangle_float) << 8;
            samples_data[i * 2 + 1] = ((int) sin_float) << 8;
        } else {
            samples_data[i * 2] = ((int) triangle_float);
            samples_data[i * 2 + 1] = ((int) sin_float);
        }

    }
    ESP_LOGI(TAG, "set data bit width to %d", bits);
    /* Updata clock and slot information */
    slot_cfg.data_bit_width = bits;
    i2s_stop_channel(tx_handle);
    i2s_set_slot(tx_handle, &slot_cfg);
    if (bits == 24) {
        /**
         * Because '24' has factor '3'
         * The mclk multiple must be able to divide by '3' as well
         * Otherwise the sample rate won't be accurate */
        clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_384;
        i2s_set_clock(tx_handle, &clk_cfg);
    } else {
        clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
        i2s_set_clock(tx_handle, &clk_cfg);
    }
    i2s_start_channel(tx_handle);

    ESP_LOGI(TAG, "write data");
    i2s_write_channel(tx_handle, samples_data, ((bits + 8) / 16)*SAMPLE_PER_CYCLE * 4, &i2s_bytes_write, 100);

    free(samples_data);
}

void app_main(void)
{
    //for 36Khz sample rates, we create 100Hz sine wave, every cycle need 36000/100 = 360 samples (4-bytes or 8-bytes each sample)
    //depend on bits_per_sample
    //using 6 buffers, we need 60-samples per buffer
    //if 2-channels, 16-bit each channel, total buffer is 360*4 = 1440 bytes
    //if 2-channels, 24/32-bit each channel, total buffer is 360*8 = 2880 bytes
    i2s_gpio_config_t i2s_pin = {
        .mclk = I2S_GPIO_UNUSED,
        .bclk = I2S_BCK_IO,
        .ws = I2S_WS_IO,
        .dout = I2S_DO_IO,
        .din = I2S_DI_IO
    };
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
    chan_cfg.id = I2S_NUM;
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);
#if SOC_I2S_SUPPORTS_APLL
    // APLL clock is more accurate when sample rate is high
    clk_cfg.clk_src = I2S_CLK_APLL;
#endif
    i2s_init_channel(tx_handle, &clk_cfg, &slot_cfg);
    i2s_start_channel(tx_handle);

    int test_bits = 16;
    while (1) {
        setup_triangle_sine_waves(test_bits);
        vTaskDelay(pdMS_TO_TICKS(5000));
        test_bits += 8;
        if (test_bits > 32) {
            test_bits = 16;
        }
    }
}
