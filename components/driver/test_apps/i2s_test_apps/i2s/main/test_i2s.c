/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "unity.h"
#include "math.h"
#include "esp_rom_gpio.h"
#include "soc/i2s_periph.h"
#include "driver/i2s_std.h"
#if SOC_I2S_SUPPORTS_PDM
#include "driver/i2s_pdm.h"
#endif
#if SOC_I2S_SUPPORTS_TDM
#include "driver/i2s_tdm.h"
#endif
#include "hal/i2s_hal.h"
#include "esp_private/i2s_platform.h"
#if SOC_PCNT_SUPPORTED
#include "driver/pulse_cnt.h"
#include "soc/pcnt_periph.h"
#endif

#include "../../test_inc/test_i2s.h"

#define I2S_TEST_MODE_SLAVE_TO_MASTER 0
#define I2S_TEST_MODE_MASTER_TO_SLAVE 1
#define I2S_TEST_MODE_LOOPBACK        2

#define I2S_TEST_MASTER_DEFAULT_PIN { \
        .mclk = MASTER_MCK_IO,  \
        .bclk = MASTER_BCK_IO,  \
        .ws = MASTER_WS_IO,     \
        .dout = DATA_OUT_IO,    \
        .din = DATA_IN_IO,      \
        .invert_flags = {       \
            .mclk_inv = false,  \
            .bclk_inv = false,  \
            .ws_inv = false,    \
        },                      \
    }

#define I2S_TEST_SLAVE_DEFAULT_PIN { \
        .mclk = -1,             \
        .bclk = SLAVE_BCK_IO,   \
        .ws = SLAVE_WS_IO,      \
        .dout = DATA_OUT_IO,    \
        .din = DATA_IN_IO,      \
        .invert_flags = {       \
            .mclk_inv = false,  \
            .bclk_inv = false,  \
            .ws_inv = false,    \
        },                      \
    }

// mode: 0, master rx, slave tx. mode: 1, master tx, slave rx. mode: 2, master tx rx loop-back
// Since ESP32-S2 has only one I2S, only loop back test can be tested.
static void i2s_test_io_config(int mode)
{
    // Connect internal signals using IO matrix.
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_BCK_IO], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_WS_IO], PIN_FUNC_GPIO);
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[DATA_OUT_IO], PIN_FUNC_GPIO);

    gpio_set_direction(MASTER_BCK_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(MASTER_WS_IO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(DATA_OUT_IO, GPIO_MODE_INPUT_OUTPUT);

    switch (mode) {
#if SOC_I2S_NUM > 1
    case I2S_TEST_MODE_SLAVE_TO_MASTER: {
        esp_rom_gpio_connect_out_signal(MASTER_BCK_IO, i2s_periph_signal[0].m_rx_bck_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_BCK_IO, i2s_periph_signal[1].s_tx_bck_sig, 0);

        esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_rx_ws_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_WS_IO, i2s_periph_signal[1].s_tx_ws_sig, 0);

        esp_rom_gpio_connect_out_signal(DATA_OUT_IO, i2s_periph_signal[1].data_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(DATA_OUT_IO, i2s_periph_signal[0].data_in_sig, 0);
    }
    break;

    case I2S_TEST_MODE_MASTER_TO_SLAVE: {
        esp_rom_gpio_connect_out_signal(MASTER_BCK_IO, i2s_periph_signal[0].m_tx_bck_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_BCK_IO, i2s_periph_signal[1].s_rx_bck_sig, 0);

        esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_tx_ws_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(MASTER_WS_IO, i2s_periph_signal[1].s_rx_ws_sig, 0);

        esp_rom_gpio_connect_out_signal(DATA_OUT_IO, i2s_periph_signal[0].data_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(DATA_OUT_IO, i2s_periph_signal[1].data_in_sig, 0);
    }
    break;
#endif
    case I2S_TEST_MODE_LOOPBACK: {
        esp_rom_gpio_connect_out_signal(DATA_OUT_IO, i2s_periph_signal[0].data_out_sig, 0, 0);
        esp_rom_gpio_connect_in_signal(DATA_OUT_IO, i2s_periph_signal[0].data_in_sig, 0);
    }
    break;

    default: {
        TEST_FAIL_MESSAGE("error: mode not supported");
    }
    break;
    }
}

static void i2s_read_write_test(i2s_chan_handle_t tx_chan, i2s_chan_handle_t rx_chan)
{
#define I2S_SEND_BUF_LEN    100
#define I2S_RECV_BUF_LEN    10000

    size_t bytes_write = 0;
    size_t bytes_read = 0;

    bool is_success = false;

    uint8_t *send_buf = (uint8_t *)calloc(I2S_SEND_BUF_LEN, sizeof(uint8_t));
    TEST_ASSERT_NOT_NULL(send_buf);
    uint8_t *recv_buf = (uint8_t *)calloc(I2S_RECV_BUF_LEN, sizeof(uint8_t));
    TEST_ASSERT_NOT_NULL(recv_buf);

    for (int i = 0; i < I2S_SEND_BUF_LEN; i++) {
        send_buf[i] = i + 1;
    }

    // write data to slave
    TEST_ESP_OK(i2s_channel_write(tx_chan, send_buf, I2S_SEND_BUF_LEN, &bytes_write, 1000));
    TEST_ESP_OK(i2s_channel_read(rx_chan, recv_buf, I2S_RECV_BUF_LEN, &bytes_read, 1000));
    TEST_ASSERT_EQUAL_INT32(I2S_SEND_BUF_LEN, bytes_write);
    TEST_ASSERT_EQUAL_INT32(I2S_RECV_BUF_LEN, bytes_read);
    // test the read data right or not
    for (int i = 0, j = 0; i < (I2S_RECV_BUF_LEN - I2S_SEND_BUF_LEN); i++) {
        if (recv_buf[i] == 1) {
            for (j = 1; (j < I2S_SEND_BUF_LEN) && (recv_buf[i+j] == j + 1); j++) {}
            if (j == I2S_SEND_BUF_LEN) {
                is_success = true;
                goto finish;
            }
            i += j;
        }
    }
finish:
    free(send_buf);
    free(recv_buf);
    TEST_ASSERT(is_success);
}

// To check if the software logic of I2S driver is correct
TEST_CASE("I2S_basic_channel_allocation_reconfig_deleting_test", "[i2s]")
{

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };

    i2s_chan_info_t chan_info;

    /* TX channel basic test */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &chan_info));
    TEST_ASSERT(chan_info.mode == I2S_COMM_MODE_NONE);

    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &chan_info));
    TEST_ASSERT(chan_info.mode == I2S_COMM_MODE_STD);
    std_cfg.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
    TEST_ESP_OK(i2s_channel_reconfig_std_slot(tx_handle, &std_cfg.slot_cfg));
    std_cfg.clk_cfg.sample_rate_hz = 44100;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ASSERT(i2s_channel_get_info(tx_handle, &chan_info) == ESP_ERR_NOT_FOUND);

    /* Duplex channel basic test */
    chan_cfg.id = I2S_NUM_0;    // Specify port id to I2S port 0
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &chan_info));
    TEST_ASSERT(chan_info.pair_chan == rx_handle);
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));

    /* Repeat to check if a same port can be allocated again */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));

    /* Hold the occupation */
    TEST_ESP_OK(i2s_platform_acquire_occupation(I2S_NUM_0, "test_i2s"));
    TEST_ASSERT(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle) == ESP_ERR_NOT_FOUND);
    TEST_ESP_OK(i2s_platform_release_occupation(I2S_NUM_0));
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

static volatile bool task_run_flag;

static void i2s_read_task(void *args) {
    i2s_chan_handle_t rx_handle = (i2s_chan_handle_t)args;
    uint8_t *recv_buf = (uint8_t *)calloc(1, 2000);
    TEST_ASSERT(recv_buf);
    size_t recv_size = 0;
    esp_err_t ret = ESP_OK;
    uint32_t cnt = 1;

    while (task_run_flag) {
        ret = i2s_channel_read(rx_handle, recv_buf, 2000, &recv_size, 300);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("Read timeout count: %"PRIu32"\n", cnt++);
        }
    }

    free(recv_buf);
    vTaskDelete(NULL);
}

static void i2s_write_task(void *args) {
    i2s_chan_handle_t tx_handle = (i2s_chan_handle_t)args;
    uint8_t *send_buf = (uint8_t *)calloc(1, 2000);
    TEST_ASSERT(send_buf);
    size_t send_size = 0;
    esp_err_t ret = ESP_OK;
    uint32_t cnt = 1;

    while (task_run_flag) {
        ret = i2s_channel_write(tx_handle, send_buf, 2000, &send_size, 300);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("Write timeout count: %"PRIu32"\n", cnt++);
        }
    }

    free(send_buf);
    vTaskDelete(NULL);
}

static void i2s_reconfig_task(void *args) {
    i2s_chan_handle_t tx_handle = (i2s_chan_handle_t)args;
    i2s_chan_info_t chan_info;
    TEST_ESP_OK(i2s_channel_get_info(tx_handle, &chan_info));
    i2s_chan_handle_t rx_handle = chan_info.pair_chan;
    int cnt = 1;

    while (task_run_flag) {
        /* Reconfig the slot while reading / writing */
        i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(i2s_channel_disable(tx_handle));
        TEST_ESP_OK(i2s_channel_disable(rx_handle));
        printf("[%d] Reconfiguring the slot...\n", cnt);
        TEST_ESP_OK(i2s_channel_reconfig_std_slot(tx_handle, &slot_cfg));
        TEST_ESP_OK(i2s_channel_reconfig_std_slot(rx_handle, &slot_cfg));
        TEST_ESP_OK(i2s_channel_enable(tx_handle));
        TEST_ESP_OK(i2s_channel_enable(rx_handle));
        vTaskDelay(pdMS_TO_TICKS(200));

        /* Reconfig the clock while reading / writing */
        i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE / 2);
        TEST_ESP_OK(i2s_channel_disable(tx_handle));
        TEST_ESP_OK(i2s_channel_disable(rx_handle));
        printf("[%d] Reconfiguring the clock...\n", cnt);
        TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &clk_cfg));
        TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &clk_cfg));
        TEST_ESP_OK(i2s_channel_enable(tx_handle));
        TEST_ESP_OK(i2s_channel_enable(rx_handle));
        vTaskDelay(pdMS_TO_TICKS(200));

        /* Reconfig the gpio while reading / writing */
        i2s_std_gpio_config_t gpio_cfg = {
            .mclk = MASTER_MCK_IO,
            .bclk = MASTER_WS_IO,
            .ws = MASTER_BCK_IO,
            .dout = DATA_IN_IO,
            .din = DATA_IN_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        };
        TEST_ESP_OK(i2s_channel_disable(tx_handle));
        TEST_ESP_OK(i2s_channel_disable(rx_handle));
        printf("[%d] Reconfiguring the gpio...\n", cnt);
        TEST_ESP_OK(i2s_channel_reconfig_std_gpio(tx_handle, &gpio_cfg));
        TEST_ESP_OK(i2s_channel_reconfig_std_gpio(rx_handle, &gpio_cfg));
        TEST_ESP_OK(i2s_channel_enable(tx_handle));
        TEST_ESP_OK(i2s_channel_enable(rx_handle));
        vTaskDelay(pdMS_TO_TICKS(200));

        cnt++;
    }

    vTaskDelete(NULL);
}

TEST_CASE("I2S_thread_concurrent_safety_test", "[i2s]")
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = MASTER_MCK_IO,
            .bclk = MASTER_BCK_IO,
            .ws = MASTER_WS_IO,
            .dout = DATA_OUT_IO,
            .din = DATA_OUT_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    /* Enable the channels before creating reading/writing task*/
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));

    task_run_flag = true;
    /* reading task to keep reading */
    xTaskCreate(i2s_read_task, "i2s_read_task", 4096, rx_handle, 5, NULL);
    /* writing task to keep writing */
    xTaskCreate(i2s_write_task, "i2s_write_task", 4096, tx_handle, 5, NULL);
    /* reconfig taks to reconfigurate the settings every 200 ms */
    xTaskCreate(i2s_reconfig_task, "i2s_reconfig_task", 4096, tx_handle, 6, NULL);

    /* Wait 3 seconds to see if any failures occur */
    vTaskDelay(pdMS_TO_TICKS(4000));

    /* Stop those three tasks */
    task_run_flag = false;

    /* Wait for the three thread deleted */
    vTaskDelay(pdMS_TO_TICKS(1000));

    /* Disable the channels, they will keep waiting until the current reading / writing finished */
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    /* Delete the channels */
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

static bool whether_contains_exapected_data(uint16_t *src, uint32_t src_len, uint32_t src_step, uint32_t start_val, uint32_t val_step)
{
    uint32_t val = start_val;
    uint32_t index_step = 1;
    for (int i = 0; val < 100 && i < src_len; i += index_step) {
        if (src[i] == val) {
            if (val == start_val && i < src_len - 8) {
                printf("start index: %d ---> \n%d %d %d %d %d %d %d %d\n", i,
                        src[i], src[i+1], src[i+2], src[i+3],
                        src[i+4], src[i+5], src[i+6], src[i+7]);
            }
            index_step = src_step;
            val += val_step;
        } else {
            index_step = 1;
            val = start_val;
        }
    }

    return val >= 100;
}

/**
 * @brief Test mono and stereo mode of I2S by loopback
 * @note  Only rx channel distinguish left mono and right mono, tx channel does not
 * @note  1. Check switch mono/stereo by 'i2s_set_clk'
 *        2. Check rx right mono and left mono (requiring tx works in stereo mode)
 *        3. Check tx mono (requiring rx works in stereo mode)
 */
TEST_CASE("I2S_mono_stereo_loopback_test", "[i2s]")
{
#define WRITE_BUF_LEN  2000
#define READ_BUF_LEN   4000
#define RETEY_TIMES    3

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    bool is_failed = false;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 8;
    chan_cfg.dma_frame_num = 128;
    i2s_std_config_t tx_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        // In stereo mode
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = MASTER_MCK_IO,
            .bclk = MASTER_BCK_IO,
            .ws = MASTER_WS_IO,
            .dout = DATA_OUT_IO,
            .din = DATA_OUT_IO,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    i2s_std_config_t rx_std_cfg = tx_std_cfg;
    rx_std_cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_MONO;
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_RIGHT;

    /* TX channel basic test */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &rx_std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &tx_std_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));

    uint16_t *w_buf = calloc(1, WRITE_BUF_LEN);
    uint16_t *r_buf = calloc(1, READ_BUF_LEN);
    size_t w_bytes = 0;
    size_t r_bytes = 0;
    uint32_t retry = 0;
    for (int n = 0; n < WRITE_BUF_LEN / 2; n++) {
        w_buf[n] = n%100;
    }

    /* rx right mono test
     * tx format: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ...
     * rx receive: 0x01[R] 0x03[R] ... */
    TEST_ESP_OK(i2s_channel_write(tx_handle, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_channel_read(rx_handle, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
#if CONFIG_IDF_TARGET_ESP32
        /* The data of tx/rx channels are flipped on ESP32 */
        for (int n = 0; n < READ_BUF_LEN / 2; n += 2) {
            int16_t temp = r_buf[n];
            r_buf[n] = r_buf[n+1];
            r_buf[n+1] = temp;
        }
#endif
        /* Expected: 1 3 5 7 9 ... 97 99 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 1, 2)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("rx right mono test failed\n");
        is_failed = true;
        goto err;
    }
    printf("rx right mono test passed\n");

    /* rx left mono test
     * tx format: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ...
     * rx receive: 0x00[R] 0x02[R] ... */
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_LEFT;
    TEST_ESP_OK(i2s_channel_reconfig_std_slot(rx_handle, &rx_std_cfg.slot_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_write(tx_handle, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_channel_read(rx_handle, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
#if CONFIG_IDF_TARGET_ESP32
        /* The data of tx/rx channels are flipped on ESP32 */
        for (int n = 0; n < READ_BUF_LEN / 2; n += 2) {
            int16_t temp = r_buf[n];
            r_buf[n] = r_buf[n+1];
            r_buf[n+1] = temp;
        }
#endif
        /* Expected: 2 4 6 8 10 ... 96 98 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 2, 2)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("rx left mono test failed\n");
        is_failed = true;
        goto err;
    }
    printf("rx left mono test passed\n");

    /* tx/rx stereo test
     * tx format: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ...
     * rx receive: 0x00[L] 0x01[R] 0x02[L] 0x03[R] ... */
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    rx_std_cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    rx_std_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;
    TEST_ESP_OK(i2s_channel_reconfig_std_slot(rx_handle, &rx_std_cfg.slot_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_write(tx_handle, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_channel_read(rx_handle, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
        /* Expected: 1 2 3 4 ... 98 99 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 1, 1, 1)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("tx/rx stereo test failed\n");
        is_failed = true;
        goto err;
    }
    printf("tx/rx stereo test passed\n");

#if !CONFIG_IDF_TARGET_ESP32 // the 16 bit channel sequence on ESP32 is incorrect
    /* tx mono rx stereo test
     * tx format: 0x01[L] 0x01[R] 0x02[L] 0x02[R] ...
     * rx receive: 0x01[L] 0x01[R] 0x02[L] 0x02[R] ... */
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    i2s_std_slot_config_t std_slot = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO);
    TEST_ESP_OK(i2s_channel_reconfig_std_slot(tx_handle, &std_slot));
    TEST_ESP_OK(i2s_channel_reconfig_std_slot(rx_handle, &rx_std_cfg.slot_cfg));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));
    TEST_ESP_OK(i2s_channel_write(tx_handle, w_buf, WRITE_BUF_LEN, &w_bytes, portMAX_DELAY));
    for (retry = 0; retry < RETEY_TIMES; retry++) {
        TEST_ESP_OK(i2s_channel_read(rx_handle, r_buf, READ_BUF_LEN, &r_bytes, portMAX_DELAY));
        /* Expected: 1 x 2 x 3 x ... 98 x 99 */
        if (whether_contains_exapected_data(r_buf, READ_BUF_LEN / 2, 2, 1, 1)) {
            break;
        }
    }
    if (retry >= RETEY_TIMES) {
        printf("tx mono rx stereo test failed\n");
        is_failed = true;
        goto err;
    }
    printf("tx mono rx stereo test passed\n");
#endif

err:
    if (is_failed) {
        for (int i = 0; i < READ_BUF_LEN / 2; i++) {
            printf("%x ", r_buf[i]);
        }
        printf("\n");
    }
    free(w_buf);
    free(r_buf);
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
    TEST_ASSERT_FALSE(is_failed);
}

TEST_CASE("I2S_memory_leak_test", "[i2s]")
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };

    /* The first operation will always take some memory */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    std_cfg.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
    TEST_ESP_OK(i2s_channel_reconfig_std_slot(tx_handle, &std_cfg.slot_cfg));
    std_cfg.clk_cfg.sample_rate_hz = 44100;
    TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));

    int memory_left = esp_get_free_heap_size();
    printf("\r\nHeap size before: %d\n", memory_left);
    for (int i = 0; i < 30; i++) {
        TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
        TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
        std_cfg.slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
        TEST_ESP_OK(i2s_channel_reconfig_std_slot(tx_handle, &std_cfg.slot_cfg));
        std_cfg.clk_cfg.sample_rate_hz = 44100;
        TEST_ESP_OK(i2s_channel_reconfig_std_clock(tx_handle, &std_cfg.clk_cfg));
        TEST_ESP_OK(i2s_del_channel(tx_handle));
        TEST_ESP_OK(i2s_del_channel(rx_handle));
        TEST_ASSERT(memory_left == esp_get_free_heap_size());
    }
    printf("\r\nHeap size after: %"PRIu32"\n", esp_get_free_heap_size());
}

TEST_CASE("I2S_loopback_test", "[i2s]")
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    i2s_test_io_config(I2S_TEST_MODE_LOOPBACK);

    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));

    i2s_read_write_test(tx_handle, rx_handle);

    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

#if SOC_I2S_NUM > 1
TEST_CASE("I2S_master_write_slave_read_test", "[i2s]")
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t mst_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_chan_config_t slv_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_SLAVE);

    i2s_std_config_t std_mst_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };

    i2s_std_config_t std_slv_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_SLAVE_DEFAULT_PIN,
    };

    TEST_ESP_OK(i2s_new_channel(&mst_chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_new_channel(&slv_chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_mst_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_slv_cfg));
    i2s_test_io_config(I2S_TEST_MODE_MASTER_TO_SLAVE);

    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));

    i2s_read_write_test(tx_handle, rx_handle);

    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

TEST_CASE("I2S_master_read_slave_write_test", "[i2s]")
{
    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t mst_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_chan_config_t slv_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_SLAVE);
    i2s_std_config_t std_mst_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };

    i2s_std_config_t std_slv_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_SLAVE_DEFAULT_PIN,
    };

    TEST_ESP_OK(i2s_new_channel(&mst_chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_new_channel(&slv_chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &std_slv_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_mst_cfg));
    i2s_test_io_config(I2S_TEST_MODE_SLAVE_TO_MASTER);

    TEST_ESP_OK(i2s_channel_enable(tx_handle));
    TEST_ESP_OK(i2s_channel_enable(rx_handle));

    i2s_read_write_test(tx_handle, rx_handle);

    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_channel_disable(rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
#endif

/*------------------------------ Clock Test --------------------------------*/
#if SOC_PCNT_SUPPORTED
#define TEST_I2S_PERIOD_MS      100
static void i2s_test_common_sample_rate(i2s_chan_handle_t rx_chan, i2s_std_clk_config_t* clk_cfg)
{
    TEST_ASSERT_NOT_NULL(rx_chan);
    TEST_ASSERT_NOT_NULL(clk_cfg);

    /* Prepare configuration for the PCNT unit */
    pcnt_unit_handle_t pcnt_unit = NULL;
    pcnt_channel_handle_t pcnt_chan = NULL;

    pcnt_unit_config_t unit_config = {
        .high_limit = (int16_t)0x7fff,
        .low_limit = (int16_t)0x8000,
    };
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = MASTER_WS_IO,
        .level_gpio_num = -1,
    };
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &pcnt_unit));
    TEST_ESP_OK(pcnt_unit_set_glitch_filter(pcnt_unit, NULL));
    TEST_ESP_OK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    TEST_ESP_OK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_unit_enable(pcnt_unit));

    // Reconfig GPIO signal
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_WS_IO], PIN_FUNC_GPIO);
    gpio_set_direction(MASTER_WS_IO, GPIO_MODE_INPUT_OUTPUT);
    esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_rx_ws_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(MASTER_WS_IO, pcnt_periph_signals.groups[0].units[0].channels[0].pulse_sig, 0);

    // Test common sample rate
    uint32_t test_freq[15] = {8000,  11025, 12000, 16000, 22050, 24000,
                            32000, 44100, 48000, 64000, 88200, 96000,
                            128000, 144000, 196000};
    int real_pulse = 0;
    for (int i = 0; i < 15; i++) {
        int expt_pulse = (int)((float)test_freq[i] * (TEST_I2S_PERIOD_MS / 1000.0));
        clk_cfg->sample_rate_hz = test_freq[i];
        TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_chan, clk_cfg));
        TEST_ESP_OK(i2s_channel_enable(rx_chan));
        vTaskDelay(1); // Waiting for hardware totally started
        // pcnt will count the pulse number on WS signal in 100ms
        TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
        TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
        vTaskDelay(pdMS_TO_TICKS(TEST_I2S_PERIOD_MS));
        TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
        TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &real_pulse));
        printf("[%"PRIu32" Hz] %d pulses, expected %d, err %d\n", test_freq[i], real_pulse, expt_pulse, real_pulse - expt_pulse);
        TEST_ESP_OK(i2s_channel_disable(rx_chan));
        // Check if the error between real pulse number and expected pulse number is within 1%
        TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);
    }
    TEST_ESP_OK(pcnt_del_channel(pcnt_chan));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_disable(pcnt_unit));
    TEST_ESP_OK(pcnt_del_unit(pcnt_unit));
}

TEST_CASE("I2S_default_PLL_clock_test", "[i2s]")
{
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));

    i2s_test_common_sample_rate(rx_handle, &std_cfg.clk_cfg);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

#if SOC_I2S_SUPPORTS_APLL
TEST_CASE("I2S_APLL_clock_test", "[i2s]")
{
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };
    std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));

    i2s_test_common_sample_rate(rx_handle, &std_cfg.clk_cfg);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
#endif // SOC_I2S_SUPPORTS_APLL
#endif // SOC_PCNT_SUPPORTED

static IRAM_ATTR bool i2s_rx_queue_overflow_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx)
{
    int *cnt = (int *)user_ctx;
    (*cnt)++;
    return false;
}

TEST_CASE("I2S_package_lost_test", "[i2s]")
{
    /* Steps of calculate appropriate parameters of I2S buffer:
     * Known by user: sample_rate = 144k, data_bit_width = 32, slot_num = 2, polling_cycle = 10 ms
     * 1. dma_buffer_size = dma_frame_num * slot_num * data_bit_width / 8 <= 4092
     *    dma_frame_num <= 511, dma_frame_num is as big as possible.
     *    interrupt_interval = dma_frame_num / sample_rate = 3.549 ms
     * 2. dma_desc_num > polling_cycle / interrupt_interval = cell(2.818) = 3
     * 3. recv_buffer_size > dma_desc_num * dma_buffer_size = 3 * 4092 = 12276 bytes */
    #define TEST_RECV_BUF_LEN   12276
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 3;
    chan_cfg.dma_frame_num = 511;
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
    i2s_event_callbacks_t cbs = {
        .on_recv = NULL,
        .on_recv_q_ovf = i2s_rx_queue_overflow_callback,
        .on_sent = NULL,
        .on_send_q_ovf = NULL,
    };
    int count = 0;
    TEST_ESP_OK(i2s_channel_register_event_callback(rx_handle, &cbs, &count));

    uint32_t test_freq[] = {16000, 32000, 48000, 64000, 96000, 128000, 144000};
    uint32_t test_num = sizeof(test_freq) / sizeof(uint32_t);
    uint8_t *data = (uint8_t *)calloc(TEST_RECV_BUF_LEN, sizeof(uint8_t));
    size_t bytes_read = 0;
    int i;
    for (i = 0; i < test_num; i++) {
        printf("Testing %"PRIu32" Hz sample rate\n", test_freq[i]);
        std_cfg.clk_cfg.sample_rate_hz = test_freq[i];
        std_cfg.clk_cfg.sample_rate_hz = test_freq[i];
        TEST_ESP_OK(i2s_channel_reconfig_std_clock(rx_handle, &std_cfg.clk_cfg));
        TEST_ESP_OK(i2s_channel_enable(rx_handle));
        for (int j = 0; j < 10; j++) {
            TEST_ESP_OK(i2s_channel_read(rx_handle, (void *)data, TEST_RECV_BUF_LEN, &bytes_read, portMAX_DELAY));
            // To simulate 10ms delay caused by other statements like data process
            vTaskDelay(1);
        }
        TEST_ESP_OK(i2s_channel_disable(rx_handle));
        if (count > 0) {
            printf("package lost detected at %"PRIu32" Hz\n", test_freq[i]);
            goto finish;
        }
    }
finish:
    TEST_ESP_OK(i2s_del_channel(rx_handle));
    free(data);
    // Test failed if package lost within 96000
    TEST_ASSERT(i == test_num);
}
