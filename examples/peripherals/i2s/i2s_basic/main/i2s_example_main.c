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
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_attr.h"
#include <math.h>

#define EXAMPLE_SAMPLE_RATE         (36000)
#define EXAMPLE_DATA_BIT_WIDTH      (I2S_DATA_BIT_WIDTH_16BIT)

#define I2S_NUM         (0)
#define WAVE_FREQ_HZ    (100)
#define PI              (3.14159265)
#define I2S_BCK_IO      (GPIO_NUM_4)
#define I2S_WS_IO       (GPIO_NUM_5)
#define I2S_DO_IO       (GPIO_NUM_18)
#define I2S_DI_IO       (GPIO_NUM_18) /// Loopback internally if data_out and data_in signal are bound to a same GPIO

#define SAMPLE_PER_CYCLE (EXAMPLE_SAMPLE_RATE/WAVE_FREQ_HZ)

static i2s_chan_handle_t tx_handle = NULL;
static i2s_chan_handle_t rx_handle = NULL;

static volatile int is_overflow = 0;

static uint32_t* example_generate_triangle_sine_waves(int bits, uint32_t *buf_len)
{
    uint32_t len = ((bits + 8) / 16)*SAMPLE_PER_CYCLE * 4;
    uint32_t *samples_data = malloc(len);

    double triangle_float = -(pow(2, bits) / 2 - 1);
    double triangle_step = (double) pow(2, bits) / SAMPLE_PER_CYCLE;

    for (int i = 0; i < SAMPLE_PER_CYCLE; i++) {
        double sin_float = sin(i * 2 * PI / SAMPLE_PER_CYCLE);
        if (sin_float >= 0) {
            triangle_float += triangle_step;
        } else {
            triangle_float -= triangle_step;
        }
        sin_float *= (pow(2, bits) / 2 - 1);
        if (bits == 16) {
            samples_data[i] = ((short)triangle_float << 16) | (short)sin_float;
        } else if (bits == 24) { //1-bytes unused
            samples_data[i * 2] = ((int) triangle_float) << 8;
            samples_data[i * 2 + 1] = ((int) sin_float) << 8;
        } else {
            samples_data[i * 2] = ((int) triangle_float);
            samples_data[i * 2 + 1] = ((int) sin_float);
        }

    }
    *buf_len = len;
    return samples_data;
}

static IRAM_ATTR bool i2s_rx_queue_overflow_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx)
{
    is_overflow++;
    return false;
}

static void example_i2s_read_task(void * args)
{
    uint32_t *rx_buf = calloc(1, 8192);
    size_t bytes_read = 0;
    uint32_t cnt = 0;

    while (1) {
        if (i2s_channel_read(rx_handle, rx_buf, 8192, &bytes_read, 1000) == ESP_OK) {
            if (cnt == 0) {
                printf("\n[i2s read] %d bytes are read successfully\n", bytes_read);
                printf("----------------------------------------------\n");
                printf("[0] %4x [1] %4x [2] %4x [3] %4x\n\n", rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);
            }
            cnt++;
            cnt %= 10;
            /* If the polling time is too long, there will be data dropped event */
            // vTaskDelay(10);
        } else {
            printf("[i2s read] %d bytes are read, timeout triggered\n\n", bytes_read);
        }
    }
    vTaskDelete(NULL);
}

static void example_i2s_write_task(void * args)
{
    uint32_t buf_len = 0;
    uint32_t *tx_buf =  example_generate_triangle_sine_waves(EXAMPLE_DATA_BIT_WIDTH, &buf_len);
    size_t bytes_written = 0;
    uint32_t cnt = 0;

    while (1) {
        if (i2s_channel_write(tx_handle, tx_buf, buf_len, &bytes_written, 1000) == ESP_OK) {
            if (cnt == 0) {
                printf("[i2s write] %d bytes are written successfully\n", bytes_written);
            }
            cnt++;
            cnt %= 20;
        } else {
            printf("[i2s write] %d bytes are written, timeout triggered\n", bytes_written);
        }
    }
    vTaskDelete(NULL);
}

static void example_i2s_init_std_duplex(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    /* Giving both tx and rx handle will make the i2s works in full-duplex mode and can share the bclk and ws signal */
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(EXAMPLE_SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(EXAMPLE_DATA_BIT_WIDTH, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_BCK_IO,
            .ws   = I2S_WS_IO,
            .dout = I2S_DO_IO,
            .din  = I2S_DI_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
#if SOC_I2S_SUPPORTS_APLL
    // APLL clock is more accurate when sample rate is high
    std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;
#endif
    /* Initialize the tx channel handle to standard mode */
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    /* Initialize the rx channel handle to standard mode */
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));

    printf("I2S tx and rx channels have been initialized to standard duplex mode\n");

    i2s_event_callbacks_t cbs = {
        .on_recv = NULL,
        .on_recv_q_ovf = i2s_rx_queue_overflow_callback,
        .on_sent = NULL,
        .on_send_q_ovf = NULL,
    };
    ESP_ERROR_CHECK(i2s_channel_register_event_callback(rx_handle, &cbs, NULL));

    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));
    printf("I2S tx and rx channels enabled\n");
}

void app_main(void)
{
    //for 36Khz sample rates, we create 100Hz sine wave, every cycle need 36000/100 = 360 samples (4-bytes or 8-bytes each sample)
    example_i2s_init_std_duplex();
    xTaskCreate(example_i2s_write_task, "i2s write task", 4096, NULL, 5, NULL);
    xTaskCreate(example_i2s_read_task, "i2s read task", 8192, NULL, 5, NULL);

    while (1) {
        if (is_overflow > 0) {
            printf("[i2s monitor] RX message Queue overflowed\n");
            is_overflow--;
        }
        vTaskDelay(1);
    }
}
