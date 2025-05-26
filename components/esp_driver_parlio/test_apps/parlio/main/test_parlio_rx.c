/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"
#include "driver/parlio_rx.h"
#include "driver/i2s_tdm.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"
#include "soc/soc_caps.h"
#include "soc/i2s_periph.h"
#include "soc/spi_periph.h"
#include "soc/parlio_periph.h"
#include "esp_attr.h"
#include "test_board.h"

#define TEST_SPI_HOST   SPI2_HOST
#define TEST_I2S_PORT   I2S_NUM_0
#define TEST_VALID_SIG  (PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1)

#if SOC_PARLIO_RX_CLK_SUPPORT_OUTPUT
#define TEST_OUTPUT_CLK_PIN     TEST_CLK_GPIO
#else
#define TEST_OUTPUT_CLK_PIN     -1
#endif

#define TEST_DEFAULT_UNIT_CONFIG(_clk_src, _clk_freq) {  \
    .trans_queue_depth = 10,  \
    .max_recv_size = 10 * 1024,  \
    .data_width = 1,  \
    .clk_src = _clk_src,  \
    .ext_clk_freq_hz = _clk_src == PARLIO_CLK_SRC_EXTERNAL ? _clk_freq : 0,  \
    .clk_in_gpio_num = _clk_src == PARLIO_CLK_SRC_EXTERNAL ? TEST_CLK_GPIO : -1,  \
    .exp_clk_freq_hz = _clk_freq,  \
    .clk_out_gpio_num = _clk_src == PARLIO_CLK_SRC_EXTERNAL ? -1 : TEST_OUTPUT_CLK_PIN,  \
    .valid_gpio_num = TEST_VALID_GPIO,  \
    .data_gpio_nums = {  \
        [0] = TEST_DATA0_GPIO,  \
        [1 ... (PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1)] = -1,  \
    },  \
    .flags = {  \
        .clk_gate_en = false,  \
    } \
}

#define TEST_TASK_DATA_READY_BIT     0x01
#define TEST_TASK_FINISHED_BIT       0x02

typedef struct {
    uint32_t partial_recv_cnt;
    uint32_t recv_done_cnt;
    uint32_t timeout_cnt;
} test_data_t;

#ifndef ALIGN_UP
#define ALIGN_UP(num, align)    (((num) + ((align) - 1)) & ~((align) - 1))
#endif

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_rx_partial_recv_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_data)
{
    test_data_t *test_data = (test_data_t *)user_data;
    test_data->partial_recv_cnt++;
    return false;
}

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_rx_done_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_data)
{
    test_data_t *test_data = (test_data_t *)user_data;
    test_data->recv_done_cnt++;
    return false;
}

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_rx_timeout_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_data)
{
    test_data_t *test_data = (test_data_t *)user_data;
    test_data->timeout_cnt++;
    return false;
}

static void connect_signal_internally(uint32_t gpio, uint32_t sigo, uint32_t sigi)
{
    gpio_config_t gpio_conf = {
        .pin_bit_mask = BIT64(gpio),
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pull_down_en = false,
        .pull_up_en = false,
    };
    gpio_config(&gpio_conf);
    esp_rom_gpio_connect_out_signal(gpio, sigo, false, false);
    esp_rom_gpio_connect_in_signal(gpio, sigi, false);
}

#define TEST_EOF_DATA_LEN    64

static void pulse_delimiter_sender_task_i2s(void *args)
{
    uint32_t *task_flags = (uint32_t *)args;
    i2s_chan_handle_t tx_chan = NULL;
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(TEST_I2S_PORT, I2S_ROLE_MASTER);
    TEST_ESP_OK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));
    i2s_tdm_config_t tx_tdm_cfg = {
        .clk_cfg  = I2S_TDM_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_TDM_PCM_SHORT_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO,
                                                          I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = TEST_CLK_GPIO,
            .ws   = TEST_VALID_GPIO,
            .dout = TEST_DATA0_GPIO,
            .din  = -1,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    TEST_ESP_OK(i2s_channel_init_tdm_mode(tx_chan, &tx_tdm_cfg));

    uint32_t buf_size = 2048;
    uint16_t *w_buf = (uint16_t *)calloc(1, buf_size);
    assert(w_buf); // Check if w_buf allocation success

    for (int i = 0; i < buf_size / 2; i += 2) {
        w_buf[i]     = 0x1234;
        w_buf[i + 1] = 0x5678;
    }

    size_t w_bytes = buf_size;

    // Preload the data into DMA buffer
    while (w_bytes == buf_size) {
        TEST_ESP_OK(i2s_channel_preload_data(tx_chan, w_buf, buf_size, &w_bytes));
    }
    // Transmission will start after enable the tx channel
    TEST_ESP_OK(i2s_channel_enable(tx_chan));

    // Connect GPIO signals
    connect_signal_internally(TEST_CLK_GPIO,
                              i2s_periph_signal[TEST_I2S_PORT].m_tx_bck_sig,
                              parlio_periph_signals.groups[0].rx_units[0].clk_in_sig);
    connect_signal_internally(TEST_VALID_GPIO,
                              i2s_periph_signal[TEST_I2S_PORT].m_tx_ws_sig,
                              parlio_periph_signals.groups[0].rx_units[0].data_sigs[TEST_VALID_SIG]);
    connect_signal_internally(TEST_DATA0_GPIO,
                              i2s_periph_signal[TEST_I2S_PORT].data_out_sig,
                              parlio_periph_signals.groups[0].rx_units[0].data_sigs[0]);

    while (!((*task_flags) & TEST_TASK_FINISHED_BIT)) {
        vTaskDelay(pdMS_TO_TICKS(1));
        *task_flags |= TEST_TASK_DATA_READY_BIT;
    }

    TEST_ESP_OK(i2s_channel_disable(tx_chan));
    TEST_ESP_OK(i2s_del_channel(tx_chan));
    free(w_buf);

    *task_flags = 0;
    while (1) {
        vTaskDelay(portMAX_DELAY);
    }
}

#if CONFIG_IDF_TARGET_ESP32C6   // TODO: IDF-9806 fix the bit shift issue in other target

static void cs_high(spi_transaction_t *trans)
{
    gpio_set_level(TEST_VALID_GPIO, 1);
}

static void cs_low(spi_transaction_t *trans)
{
    gpio_set_level(TEST_VALID_GPIO, 0);
}

#define TEST_SPI_CLK_FREQ   100000

static void level_delimiter_sender_task_spi(void *args)
{
    uint32_t *task_flags = (uint32_t *)args;
    spi_device_handle_t dev_handle;

    spi_bus_config_t bus_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = TEST_DATA0_GPIO,
        .sclk_io_num = TEST_CLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 2048,
    };
    spi_device_interface_config_t dev_cfg = {
        .command_bits = 0,
        .address_bits = 0,
        .clock_speed_hz = TEST_SPI_CLK_FREQ,
        .mode = 0,
        .duty_cycle_pos = 128,
        .spics_io_num = TEST_VALID_GPIO,
        .queue_size = 5,
        .flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_POSITIVE_CS,
        .pre_cb = cs_high,
        .post_cb = cs_low,
    };
    //Initialize the SPI bus and add device
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &dev_handle));

    // Initialize CS gpio
    gpio_set_level(TEST_VALID_GPIO, 0);
    gpio_config_t cs_cfg = {
        .pin_bit_mask = BIT64(TEST_VALID_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&cs_cfg);

    // Connect SPI signals to parlio rx signals
    connect_signal_internally(TEST_CLK_GPIO,
                              spi_periph_signal[TEST_SPI_HOST].spiclk_out,
                              parlio_periph_signals.groups[0].rx_units[0].clk_in_sig);
    connect_signal_internally(TEST_VALID_GPIO,
                              spi_periph_signal[TEST_SPI_HOST].spics_out[0],
                              parlio_periph_signals.groups[0].rx_units[0].data_sigs[TEST_VALID_SIG]);
    connect_signal_internally(TEST_DATA0_GPIO,
                              spi_periph_signal[TEST_SPI_HOST].spid_out,
                              parlio_periph_signals.groups[0].rx_units[0].data_sigs[0]);

    // Prepare the data the be transmitted
    uint8_t *data = (uint8_t *)calloc(1, TEST_EOF_DATA_LEN);
    for (int i = 0; i < TEST_EOF_DATA_LEN; i += 4) {
        data[i]     = 0x12;
        data[i + 1] = 0x34;
        data[i + 2] = 0x56;
        data[i + 3] = 0x78;
    }
    spi_transaction_t t = {
        .cmd = 0,
        .length = TEST_EOF_DATA_LEN * 8,
        .flags = 0,
        .tx_buffer = data,
        .user = NULL,
    };

    // Transmit data every 1ms, until the main test thread finished receiving
    while (!((*task_flags) & TEST_TASK_FINISHED_BIT)) {
        TEST_ESP_OK(spi_device_transmit(dev_handle, &t));
        vTaskDelay(pdMS_TO_TICKS(1));
        *task_flags |= TEST_TASK_DATA_READY_BIT;
    }

    // Remove the SPI device and free the bus
    TEST_ESP_OK(spi_bus_remove_device(dev_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST));
    // Free data buffer
    free(data);

    // Reset the flag to indicate the sending loop has quit
    *task_flags = 0;
    // Waiting to be deleted
    while (1) {
        vTaskDelay(portMAX_DELAY);
    }
}
#endif

static bool test_delimiter(parlio_rx_delimiter_handle_t deli, bool free_running_clk, void (*sender_task_thread)(void *args))
{
    parlio_rx_unit_handle_t rx_unit = NULL;

    parlio_rx_unit_config_t config = TEST_DEFAULT_UNIT_CONFIG(PARLIO_CLK_SRC_EXTERNAL, 1000000);
    config.flags.free_clk = free_running_clk;
    TEST_ESP_OK(parlio_new_rx_unit(&config, &rx_unit));
    TEST_ESP_OK(parlio_rx_unit_enable(rx_unit, true));

    TaskHandle_t sender_task;
    /* The flag to transport finish information between main test thread and the sender thread
     * Set it as static to make sure it'll be valid in another thread */
    static uint32_t task_flags = 0;
    xTaskCreate(sender_task_thread, "sender task", 4096, &task_flags, 5, &sender_task);
    // Waiting for the data ready on line
    while ((task_flags & TEST_TASK_DATA_READY_BIT)) {
        vTaskDelay(1);
    }

    parlio_receive_config_t recv_config = {
        .delimiter = deli,
        .flags.partial_rx_en = false,
    };
    uint8_t *recv_buff = NULL;
    uint32_t alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    alignment = alignment < 4 ? 4 : alignment;
    size_t buff_size = ALIGN_UP(TEST_EOF_DATA_LEN, alignment);
    recv_buff = heap_caps_aligned_calloc(alignment, 1, buff_size, TEST_PARLIO_DMA_MEM_ALLOC_CAPS);
    bool is_success = false;
    // sample 5 times
    for (int i = 0; i < 5 && !is_success; i++) {
        TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, recv_buff, buff_size, &recv_config));
        TEST_ESP_OK(parlio_rx_unit_wait_all_done(rx_unit, 5000));
        for (int k = 0; k < TEST_EOF_DATA_LEN; k++) {
            printf("%x ", recv_buff[k]);
            if ((k & 0xf) == 0xf) {
                printf("\n");
            }
        }
        printf("\n");
        for (int j = 0; j < TEST_EOF_DATA_LEN; j++) {
            // Check if 0x12 0x34 0x56 0x78 appeared in the buffer
            if (recv_buff[j] == 0x12 && recv_buff[j + 1] == 0x34 &&
                    recv_buff[j + 2] == 0x56 && recv_buff[j + 3] == 0x78) {
                is_success = true;
                break;
            }
        }
    }
    // Indicate the test finished, no need to send data
    task_flags |= TEST_TASK_FINISHED_BIT;
    // Waiting for the sender task quit
    while (task_flags) {
        vTaskDelay(1);
    }
    // Delete the sender task
    vTaskDelete(sender_task);
    free(recv_buff);

    TEST_ESP_OK(parlio_rx_unit_disable(rx_unit));
    TEST_ESP_OK(parlio_del_rx_unit(rx_unit));
    return is_success;
}

#if CONFIG_IDF_TARGET_ESP32C6   // TODO: IDF-9806 fix the bit shift issue in other target
// This test case uses level delimiter
TEST_CASE("parallel_rx_unit_level_delimiter_test_via_spi", "[parlio_rx]")
{
    parlio_rx_level_delimiter_config_t lvl_deli_cfg = {
        .valid_sig_line_id = TEST_VALID_SIG,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .eof_data_len = TEST_EOF_DATA_LEN,
        .timeout_ticks = 0,
        .flags = {
            .active_low_en = 0,
        },
    };
    parlio_rx_delimiter_handle_t deli = NULL;
    TEST_ESP_OK(parlio_new_rx_level_delimiter(&lvl_deli_cfg, &deli));
    bool is_success = test_delimiter(deli, false, level_delimiter_sender_task_spi);
    TEST_ESP_OK(parlio_del_rx_delimiter(deli));
    TEST_ASSERT(is_success);
}
#endif

// This test case uses pulse delimiter
TEST_CASE("parallel_rx_unit_pulse_delimiter_test_via_i2s", "[parlio_rx]")
{
    parlio_rx_pulse_delimiter_config_t pls_deli_cfg = {
        .valid_sig_line_id = TEST_VALID_SIG,
        .sample_edge = PARLIO_SAMPLE_EDGE_NEG,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .eof_data_len = TEST_EOF_DATA_LEN,
        .timeout_ticks = 0,
        .flags = {
            .start_bit_included = 0,
            .end_bit_included = 0,
            .has_end_pulse = 0,
            .pulse_invert = 0,
        },
    };
    parlio_rx_delimiter_handle_t deli = NULL;
    TEST_ESP_OK(parlio_new_rx_pulse_delimiter(&pls_deli_cfg, &deli));
    bool is_success = test_delimiter(deli, true, pulse_delimiter_sender_task_i2s);
    TEST_ESP_OK(parlio_del_rx_delimiter(deli));
    TEST_ASSERT(is_success);
}

TEST_CASE("parallel_rx_unit_install_uninstall", "[parlio_rx]")
{
    printf("install rx units exhaustively\r\n");
    parlio_rx_unit_handle_t units[SOC_PARLIO_GROUPS * SOC_PARLIO_RX_UNITS_PER_GROUP];
    int k = 0;
    parlio_rx_unit_config_t config = TEST_DEFAULT_UNIT_CONFIG(PARLIO_CLK_SRC_DEFAULT, 1000000);
    for (int i = 0; i < SOC_PARLIO_GROUPS; i++) {
        for (int j = 0; j < SOC_PARLIO_RX_UNITS_PER_GROUP; j++) {
            TEST_ESP_OK(parlio_new_rx_unit(&config, &units[k++]));
        }
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, parlio_new_rx_unit(&config, &units[0]));

    for (int i = 0; i < k; i++) {
        TEST_ESP_OK(parlio_del_rx_unit(units[i]));
    }

    // clock from external
    config.clk_src = PARLIO_CLK_SRC_EXTERNAL;
    // clock gpio must be set when the clock is input from external
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, parlio_new_rx_unit(&config, &units[0]));

    // clock from internal
    config.clk_src = PARLIO_CLK_SRC_DEFAULT;
    config.clk_out_gpio_num = TEST_CLK_GPIO;
#if SOC_PARLIO_RX_CLK_SUPPORT_OUTPUT
    TEST_ESP_OK(parlio_new_rx_unit(&config, &units[0]));
    TEST_ESP_OK(parlio_del_rx_unit(units[0]));
#else
    // failed because of not support output the clock to a gpio
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, parlio_new_rx_unit(&config, &units[0]));
    config.clk_out_gpio_num = -1;
#endif
    config.data_width = 3;
    // data width should be power of 2
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, parlio_new_rx_unit(&config, &units[0]));

    config.data_width = 4;
    TEST_ESP_OK(parlio_new_rx_unit(&config, &units[0]));
    TEST_ESP_OK(parlio_rx_unit_enable(units[0], true));
    // delete unit before it's disabled is not allowed
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, parlio_del_rx_unit(units[0]));
    TEST_ESP_OK(parlio_rx_unit_disable(units[0]));
    TEST_ESP_OK(parlio_del_rx_unit(units[0]));
}
#define TEST_PAYLOAD_SIZE   5120

// This test case uses soft delimiter
TEST_CASE("parallel_rx_unit_receive_transaction_test", "[parlio_rx]")
{
    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_delimiter_handle_t deli = NULL;

    parlio_rx_unit_config_t config = TEST_DEFAULT_UNIT_CONFIG(PARLIO_CLK_SRC_DEFAULT, 1000000);
    config.flags.free_clk = 1;
    TEST_ESP_OK(parlio_new_rx_unit(&config, &rx_unit));

    parlio_rx_soft_delimiter_config_t sft_deli_cfg = {
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .eof_data_len = TEST_PAYLOAD_SIZE,
        .timeout_ticks = 0,
    };
    TEST_ESP_OK(parlio_new_rx_soft_delimiter(&sft_deli_cfg, &deli));

    parlio_rx_event_callbacks_t cbs = {
        .on_partial_receive = test_parlio_rx_partial_recv_callback,
        .on_receive_done = test_parlio_rx_done_callback,
    };
    test_data_t test_data = {
        .partial_recv_cnt = 0,
        .recv_done_cnt = 0,
    };
    TEST_ESP_OK(parlio_rx_unit_register_event_callbacks(rx_unit, &cbs, &test_data));
    TEST_ESP_OK(parlio_rx_unit_enable(rx_unit, true));

    parlio_receive_config_t recv_config = {
        .delimiter = deli,
        .flags.partial_rx_en = false,
    };
    uint8_t *payload = NULL;
    uint32_t alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    alignment = alignment < 4 ? 4 : alignment;
    size_t payload_size = ALIGN_UP(TEST_PAYLOAD_SIZE, alignment);
    payload = heap_caps_aligned_calloc(alignment, 1, payload_size, TEST_PARLIO_DMA_MEM_ALLOC_CAPS);
    TEST_ASSERT(payload);

    printf("Testing one normal transaction...\n");
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, true));
    TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, payload, TEST_PAYLOAD_SIZE, &recv_config));
    TEST_ESP_OK(parlio_rx_unit_wait_all_done(rx_unit, 5000));
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, false));
    TEST_ASSERT_EQUAL_UINT32(2, test_data.partial_recv_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, test_data.recv_done_cnt);
    memset(&test_data, 0, sizeof(test_data_t));

    printf("Testing normal transactions in queue...\n");
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, true));
    // push 5 repeated transactions to the queue
    for (int i = 0; i < 5; i++) {
        TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, payload, TEST_PAYLOAD_SIZE, &recv_config));
    }
    TEST_ESP_OK(parlio_rx_unit_wait_all_done(rx_unit, 5000));
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, false));
    TEST_ASSERT_EQUAL_UINT32(10, test_data.partial_recv_cnt);
    TEST_ASSERT_EQUAL_UINT32(5, test_data.recv_done_cnt);
    memset(&test_data, 0, sizeof(test_data_t));

    printf("Testing resume transactions in queue after enabling...\n");
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, true));
    // push 5 repeated transactions to the queue
    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, payload, TEST_PAYLOAD_SIZE, &recv_config));
    }
    TEST_ESP_OK(parlio_rx_unit_disable(rx_unit));
    memset(&test_data, 0, sizeof(test_data_t));
    TEST_ESP_OK(parlio_rx_unit_enable(rx_unit, false));
    TEST_ESP_OK(parlio_rx_unit_wait_all_done(rx_unit, 5000));
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, false));
    TEST_ASSERT_GREATER_THAN(2, test_data.partial_recv_cnt);
    TEST_ASSERT_GREATER_THAN(1, test_data.recv_done_cnt);
    memset(&test_data, 0, sizeof(test_data_t));

    printf("Testing the infinite transaction...\n");
    recv_config.flags.partial_rx_en = true;
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, true));
    TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, payload, TEST_PAYLOAD_SIZE, &recv_config));
    // Won't receive done semaphore in infinite transaction
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, parlio_rx_unit_wait_all_done(rx_unit, 500));
    TEST_ESP_OK(parlio_rx_soft_delimiter_start_stop(rx_unit, deli, false));
    TEST_ASSERT_GREATER_THAN(6, test_data.partial_recv_cnt);
    TEST_ASSERT_GREATER_THAN(3, test_data.recv_done_cnt);
    memset(&test_data, 0, sizeof(test_data_t));

    TEST_ESP_OK(parlio_rx_unit_disable(rx_unit));
    TEST_ESP_OK(parlio_del_rx_delimiter(deli));
    TEST_ESP_OK(parlio_del_rx_unit(rx_unit));
    free(payload);
};

TEST_CASE("parallel_rx_unit_receive_timeout_test", "[parlio_rx]")
{
    printf("init a gpio to simulate valid signal\r\n");
    gpio_config_t test_gpio_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT64(TEST_VALID_GPIO),
    };
    TEST_ESP_OK(gpio_config(&test_gpio_conf));

    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_delimiter_handle_t timeout_deli = NULL;

    parlio_rx_unit_config_t config = TEST_DEFAULT_UNIT_CONFIG(PARLIO_CLK_SRC_DEFAULT, 1000000);
    config.flags.free_clk = 1;
    config.flags.clk_gate_en = 1;
    TEST_ESP_OK(parlio_new_rx_unit(&config, &rx_unit));

    parlio_rx_level_delimiter_config_t lvl_deli_cfg = {
        .valid_sig_line_id = TEST_VALID_SIG,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .eof_data_len = TEST_PAYLOAD_SIZE,
        .timeout_ticks = 400,
        .flags = {
            .active_low_en = 0,
        },
    };
    TEST_ESP_OK(parlio_new_rx_level_delimiter(&lvl_deli_cfg, &timeout_deli));

    parlio_rx_event_callbacks_t cbs = {
        .on_timeout = test_parlio_rx_timeout_callback,
    };
    test_data_t test_data = {
        .timeout_cnt = 0,
    };
    TEST_ESP_OK(parlio_rx_unit_register_event_callbacks(rx_unit, &cbs, &test_data));
    TEST_ESP_OK(parlio_rx_unit_enable(rx_unit, true));

    parlio_receive_config_t recv_config = {
        .delimiter = timeout_deli,
        .flags.partial_rx_en = false,
    };
    uint8_t *payload = NULL;
    uint32_t alignment = cache_hal_get_cache_line_size(CACHE_LL_LEVEL_INT_MEM, CACHE_TYPE_DATA);
    alignment = alignment < 4 ? 4 : alignment;
    size_t payload_size = ALIGN_UP(TEST_PAYLOAD_SIZE, alignment);
    payload = heap_caps_aligned_calloc(alignment, 1, payload_size, TEST_PARLIO_DMA_MEM_ALLOC_CAPS);
    TEST_ASSERT(payload);

    printf("Testing the timeout callback...\n");
    // push 5 repeated transactions to the queue
    for (int i = 0; i < 5; i++) {
        TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, payload, TEST_PAYLOAD_SIZE, &recv_config));
        gpio_set_level(TEST_VALID_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(TEST_VALID_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(5));
        printf("Transaction %d finished\n", i);
    }
    TEST_ASSERT_TRUE(test_data.timeout_cnt);

    TEST_ESP_OK(parlio_rx_unit_disable(rx_unit));
    TEST_ESP_OK(parlio_del_rx_delimiter(timeout_deli));
    TEST_ESP_OK(parlio_del_rx_unit(rx_unit));
    TEST_ESP_OK(gpio_reset_pin(TEST_VALID_GPIO));
    free(payload);
}
