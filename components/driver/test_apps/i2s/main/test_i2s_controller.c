/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * I2S test environment UT_T1_I2S:
 * We use internal signals instead of external wiring, but please keep the following IO connections, or connect nothing to prevent the signal from being disturbed.
 * connect GPIO15 and GPIO19, GPIO25(ESP32)/GPIO17(ESP32-S2) and GPIO26, GPIO21 and GPIO22(ESP32)/GPIO20(ESP32-S2)
 * Please do not connect GPIO32(ESP32) any pull-up resistors externally, it will be used to test i2s adc function.
 */


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "esp_err.h"
#include "unity.h"
#include "math.h"
#include "esp_rom_gpio.h"
#include "soc/i2s_periph.h"
#include "driver/i2s_controller.h"
#include "hal/i2s_hal.h"
#include "esp_private/i2s_platform.h"
#if SOC_PCNT_SUPPORTED
#include "driver/pcnt.h"
#include "soc/pcnt_periph.h"
#endif

#define SAMPLE_RATE     (48000)
#define SAMPLE_BITS     (16)

#if CONFIG_IDF_TARGET_ESP32
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 15
#define MASTER_WS_IO 25
#define SLAVE_BCK_IO 19
#define SLAVE_WS_IO 26
#define DATA_IN_IO 21
#define DATA_OUT_IO 22
#define ADC1_CHANNEL_4_IO 32
#elif CONFIG_IDF_TARGET_ESP32S2
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 4
#define MASTER_WS_IO 5
#define SLAVE_BCK_IO 14
#define SLAVE_WS_IO 15
#define DATA_IN_IO 19
#define DATA_OUT_IO 18
#elif CONFIG_IDF_TARGET_ESP32C3
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 4
#define MASTER_WS_IO 5
#define SLAVE_BCK_IO 14
#define SLAVE_WS_IO 15
#define DATA_IN_IO 19
#define DATA_OUT_IO 18
#elif CONFIG_IDF_TARGET_ESP32S3
#define MASTER_MCK_IO 0
#define MASTER_BCK_IO 4
#define MASTER_WS_IO 5
#define SLAVE_BCK_IO 14
#define SLAVE_WS_IO 15
#define DATA_IN_IO 19
#define DATA_OUT_IO 18
#endif

#define I2S_TEST_MODE_SLAVE_TO_MASTER 0
#define I2S_TEST_MODE_MASTER_TO_SLAVE 1
#define I2S_TEST_MODE_LOOPBACK        2

#define I2S_TEST_MASTER_DEFAULT_PIN { \
        .mclk = MASTER_MCK_IO, \
        .bclk = MASTER_BCK_IO, \
        .ws = MASTER_WS_IO,    \
        .dout = DATA_OUT_IO,   \
        .din = DATA_IN_IO      \
    };

#define I2S_TEST_SLAVE_DEFAULT_PIN { \
        .mclk = -1,             \
        .bclk = SLAVE_BCK_IO,   \
        .ws = SLAVE_WS_IO,      \
        .dout = DATA_OUT_IO,    \
        .din = DATA_IN_IO       \
    };

// This empty function is used to force the compiler link this file
void test_app_include_i2s_controller(void)
{
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
    TEST_ESP_OK(i2s_write_channel(tx_chan, send_buf, I2S_SEND_BUF_LEN, &bytes_write, 1000 / portTICK_PERIOD_MS));
    TEST_ESP_OK(i2s_read_channel(rx_chan, recv_buf, I2S_RECV_BUF_LEN, &bytes_read, 1000 / portTICK_PERIOD_MS));
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
TEST_CASE("I2S basic driver apply, delete test", "[i2s]")
{
    i2s_gpio_config_t i2s_pin = I2S_TEST_MASTER_DEFAULT_PIN;

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);

    /* TX channel basic test */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    TEST_ESP_OK(i2s_init_channel(tx_handle, &clk_cfg, &slot_cfg));
    slot_cfg.data_bit_width = I2S_DATA_BIT_WIDTH_32BIT;
    TEST_ESP_OK(i2s_set_slot(tx_handle, &slot_cfg));
    clk_cfg.sample_rate = 44100;
    TEST_ESP_OK(i2s_set_clock(tx_handle, &clk_cfg));
    TEST_ESP_OK(i2s_start_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));

    /* Duplex channel basic test */
    chan_cfg.id = I2S_NUM_0;    // Specify port id to I2S port 0
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_init_channel(tx_handle, &clk_cfg, &slot_cfg));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));

    /* Repeat to check if a same port can be applied again */
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

TEST_CASE("I2S memory leak test", "[i2s]")
{
    i2s_gpio_config_t i2s_pin = I2S_TEST_MASTER_DEFAULT_PIN;

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);

    /* The first operation will always take some memory */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));

    int memory_left = esp_get_free_heap_size();
    printf("\r\nHeap size before: %d\n", memory_left);
    for (int i = 0; i < 100; i++) {
        TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
        TEST_ESP_OK(i2s_del_channel(tx_handle));
        TEST_ESP_OK(i2s_del_channel(rx_handle));
        TEST_ASSERT(memory_left == esp_get_free_heap_size());
    }
    printf("\r\nHeap size after: %d\n", esp_get_free_heap_size());
}

TEST_CASE("I2S loopback test", "[i2s]")
{
    i2s_gpio_config_t i2s_pin = I2S_TEST_MASTER_DEFAULT_PIN;

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);
    chan_cfg.id = I2S_NUM_0;
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    TEST_ESP_OK(i2s_init_channel(tx_handle, &clk_cfg, &slot_cfg));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));
    i2s_test_io_config(I2S_TEST_MODE_LOOPBACK);

    TEST_ESP_OK(i2s_start_channel(tx_handle));
    TEST_ESP_OK(i2s_start_channel(rx_handle));

    i2s_read_write_test(tx_handle, rx_handle);

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

#if SOC_I2S_NUM > 1
TEST_CASE("I2S master write slave read test", "[i2s]")
{
    i2s_gpio_config_t mst_pin = I2S_TEST_MASTER_DEFAULT_PIN;
    i2s_gpio_config_t slv_pin = I2S_TEST_SLAVE_DEFAULT_PIN;

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t mst_chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &mst_pin);
    mst_chan_cfg.id = I2S_NUM_0;
    i2s_chan_config_t slv_chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_SLAVE, I2S_COMM_MODE_STD, &slv_pin);
    slv_chan_cfg.id = I2S_NUM_1;
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);

    TEST_ESP_OK(i2s_new_channel(&mst_chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_new_channel(&slv_chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_init_channel(tx_handle, &clk_cfg, &slot_cfg));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));
    i2s_test_io_config(I2S_TEST_MODE_MASTER_TO_SLAVE);

    TEST_ESP_OK(i2s_start_channel(tx_handle));
    TEST_ESP_OK(i2s_start_channel(rx_handle));

    i2s_read_write_test(tx_handle, rx_handle);

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

TEST_CASE("I2S master read slave write test", "[i2s]")
{
    i2s_gpio_config_t mst_pin = I2S_TEST_MASTER_DEFAULT_PIN;
    i2s_gpio_config_t slv_pin = I2S_TEST_SLAVE_DEFAULT_PIN;

    i2s_chan_handle_t tx_handle;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t mst_chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &mst_pin);
    mst_chan_cfg.id = I2S_NUM_0;
    i2s_chan_config_t slv_chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_SLAVE, I2S_COMM_MODE_STD, &slv_pin);
    slv_chan_cfg.id = I2S_NUM_1;
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);

    TEST_ESP_OK(i2s_new_channel(&mst_chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_new_channel(&slv_chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(i2s_init_channel(tx_handle, &clk_cfg, &slot_cfg));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));
    i2s_test_io_config(I2S_TEST_MODE_SLAVE_TO_MASTER);

    TEST_ESP_OK(i2s_start_channel(tx_handle));
    TEST_ESP_OK(i2s_start_channel(rx_handle));

    i2s_read_write_test(tx_handle, rx_handle);

    TEST_ESP_OK(i2s_del_channel(tx_handle));
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
#endif

/*------------------------------ Clock Test --------------------------------*/
#if SOC_PCNT_SUPPORTED
#define TEST_I2S_PERIOD_MS      100
static void i2s_test_common_sample_rate(i2s_chan_handle_t rx_chan, i2s_clk_config_t* clk_cfg)
{
    TEST_ASSERT_NOT_NULL(rx_chan);
    TEST_ASSERT_NOT_NULL(clk_cfg);

    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_cfg = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = MASTER_WS_IO,
        .ctrl_gpio_num = -1,
        .channel = PCNT_CHANNEL_0,
        .unit = PCNT_UNIT_0,
        .pos_mode = PCNT_COUNT_INC,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_DIS,   // Keep the counter value on the negative edge
        .lctrl_mode = PCNT_MODE_KEEP,
        .hctrl_mode = PCNT_MODE_KEEP,
        .counter_h_lim = (int16_t)0x7fff,
        .counter_l_lim = (int16_t)0x8000,
    };
    TEST_ESP_OK(pcnt_unit_config(&pcnt_cfg));

    // Reconfig GPIO signal
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[MASTER_WS_IO], PIN_FUNC_GPIO);
    gpio_set_direction(MASTER_WS_IO, GPIO_MODE_INPUT_OUTPUT);
    esp_rom_gpio_connect_out_signal(MASTER_WS_IO, i2s_periph_signal[0].m_rx_ws_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(MASTER_WS_IO, pcnt_periph_signals.groups[0].units[0].channels[0].pulse_sig, 0);

    // pcnt_set_filter_value(PCNT_UNIT_0, 10);
    pcnt_filter_disable(PCNT_UNIT_0);

    // Test common sample rate
    uint32_t test_freq[15] = {8000,  11025, 12000, 16000, 22050, 24000,
                            32000, 44100, 48000, 64000, 88200, 96000,
                            128000, 144000, 196000};
    int16_t real_pulse = 0;
    for (int i = 0; i < 15; i++) {
        int16_t expt_pulse = (int16_t)((float)test_freq[i] * (TEST_I2S_PERIOD_MS / 1000.0));
        clk_cfg->sample_rate = test_freq[i];
        TEST_ESP_OK(i2s_set_clock(rx_chan, clk_cfg));
        TEST_ESP_OK(i2s_start_channel(rx_chan));
        vTaskDelay(1); // Waiting for hardware totally started
        // pcnt will count the pulse number on WS signal in 100ms
        TEST_ESP_OK(pcnt_counter_clear(PCNT_UNIT_0));
        TEST_ESP_OK(pcnt_counter_resume(PCNT_UNIT_0));
        vTaskDelay(pdMS_TO_TICKS(TEST_I2S_PERIOD_MS));
        TEST_ESP_OK(pcnt_counter_pause(PCNT_UNIT_0));
        TEST_ESP_OK(pcnt_get_counter_value(PCNT_UNIT_0, &real_pulse));
        printf("[%d Hz] %d pulses, expected %d, err %d\n", test_freq[i], real_pulse, expt_pulse, real_pulse - expt_pulse);
        TEST_ESP_OK(i2s_stop_channel(rx_chan));
        // Check if the error between real pulse number and expected pulse number is within 1%
        TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);
    }
}

TEST_CASE("I2S D2CLK clock test", "[i2s]")
{
    i2s_gpio_config_t i2s_pin = I2S_TEST_MASTER_DEFAULT_PIN;

    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);
    chan_cfg.id = I2S_NUM_0;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));

    i2s_test_common_sample_rate(rx_handle, (i2s_clk_config_t *)&clk_cfg);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}

#if SOC_I2S_SUPPORTS_APLL
TEST_CASE("I2S APLL clock test", "[i2s]")
{
    i2s_gpio_config_t i2s_pin = I2S_TEST_MASTER_DEFAULT_PIN;

    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(SAMPLE_BITS, I2S_SLOT_MODE_STEREO);
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);
    chan_cfg.id = I2S_NUM_0;
    clk_cfg.clk_src = I2S_CLK_APLL;

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));

    i2s_test_common_sample_rate(rx_handle, (i2s_clk_config_t *)&clk_cfg);
    TEST_ESP_OK(i2s_del_channel(rx_handle));
}
#endif // SOC_I2S_SUPPORTS_APLL
#endif // SOC_PCNT_SUPPORTED

static void i2s_event_monitor(void *args)
{
    i2s_chan_handle_t rx_handle = *((i2s_chan_handle_t *)args);
    QueueHandle_t evt_que = i2s_get_event_queue(rx_handle, 16);
    TEST_ASSERT_NOT_NULL(evt_que);
    i2s_event_t evnet;
    while (1) {
        xQueueReceive(evt_que, &evnet, portMAX_DELAY);
        if (evnet.type == I2S_EVENT_RX_Q_OVF) {
            break;
        }
    }
    vTaskDelete(NULL);
}

TEST_CASE("I2S package lost test", "[i2s]")
{
    /* Steps of calculate appropriate parameters of I2S buffer:
     * Known by user: sample_rate = 144k, data_bit_width = 32, slot_num = 2, polling_cycle = 10 ms
     * 1. dma_buffer_size = dma_frame_num * slot_num * data_bit_width / 8 <= 4092
     *    dma_frame_num <= 511, dma_frame_num is as big as possible.
     *    interrupt_interval = dma_frame_num / sample_rate = 3.549 ms
     * 2. dma_desc_num > polling_cycle / interrupt_interval = cell(2.818) = 3
     * 3. recv_buffer_size > dma_desc_num * dma_buffer_size = 3 * 4092 = 12276 bytes */
    #define TEST_RECV_BUF_LEN   12276
    i2s_gpio_config_t i2s_pin = I2S_TEST_MASTER_DEFAULT_PIN;
    i2s_chan_handle_t rx_handle;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_CONFIG(I2S_ROLE_MASTER, I2S_COMM_MODE_STD, &i2s_pin);
    i2s_std_slot_config_t slot_cfg = I2S_STD_PHILIP_SLOT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO);
    slot_cfg.dma_desc_num = 3;
    slot_cfg.dma_frame_num = 511;
    i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_CONFIG(SAMPLE_RATE);

    TEST_ESP_OK(i2s_new_channel(&chan_cfg, NULL, &rx_handle));
    TEST_ESP_OK(i2s_init_channel(rx_handle, &clk_cfg, &slot_cfg));

    TaskHandle_t h_monitor_task;
    xTaskCreate(i2s_event_monitor, "event monitor task", 4096, &rx_handle, 5, &h_monitor_task);

    uint32_t test_freq[] = {16000, 32000, 48000, 64000, 96000, 128000, 144000};
    uint32_t test_num = sizeof(test_freq) / sizeof(uint32_t);
    uint8_t *data = (uint8_t *)calloc(TEST_RECV_BUF_LEN, sizeof(uint8_t));
    size_t bytes_read = 0;
    int i;
    for (i = 0; i < test_num; i++) {
        printf("Testing %d Hz sample rate\n", test_freq[i]);
        clk_cfg.sample_rate = test_freq[i];
        TEST_ESP_OK(i2s_set_clock(rx_handle, &clk_cfg));
        TEST_ESP_OK(i2s_start_channel(rx_handle));
        for (int j = 0; j < 10; j++) {
            TEST_ESP_OK(i2s_read_channel(rx_handle, (void *)data, TEST_RECV_BUF_LEN, &bytes_read, portMAX_DELAY));
            // To simulate 10ms delay caused by other statements like data process
            vTaskDelay(1);
        }
        TEST_ESP_OK(i2s_stop_channel(rx_handle));
        if (eTaskGetState(h_monitor_task) == eDeleted) {
            printf("package lost detected at %d Hz\n", test_freq[i]);
            goto finish;
        }
    }
    vTaskDelete(h_monitor_task);
finish:
    TEST_ESP_OK(i2s_del_channel(rx_handle));
    free(data);
    // Test failed if package lost within 96000
    TEST_ASSERT(i == test_num);
}
